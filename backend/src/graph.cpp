#include "graph.hpp"
#include <queue>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <iostream>
#include <cmath>

using namespace std;

//Haversine Formula: Used to calculate the great-circle distance between two points on the Earth's surface, considering the curvature of the Earth.
// Haversine distance calculation
double haversineDistance(double lat1, double lon1, double lat2, double lon2) 
{
    //Haversine Distance  formula components
    const double R = 6371000; // Earth radius in meters
    const double phi1 = lat1 * M_PI / 180;
    const double phi2 = lat2 * M_PI / 180;
    const double deltaPhi = (lat2 - lat1) * M_PI / 180;
    const double deltaLambda = (lon2 - lon1) * M_PI / 180;

    //Haversine distance formula
    const double a = sin(deltaPhi / 2) * sin(deltaPhi / 2) +
                     cos(phi1) * cos(phi2) *
                     sin(deltaLambda / 2) * sin(deltaLambda / 2);

    const double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    //multiplying the radian of earth with angle to get distance in meters
    return R * c;
}

// Calculates the angle (bearing) between three points: previous, current, and next
double calculateAngle(const pair<double, double>& prev, const pair<double, double>& curr, 
                      const pair<double, double>& next, double prevAngle) {
    double lat2 = curr.first * M_PI / 180;// Convert current latitude to radians
    double lon2 = curr.second * M_PI / 180;
    double lat3 = next.first * M_PI / 180;
    double lon3 = next.second * M_PI / 180;

    // Calculate the angle between the current and next point using the spherical law of cosines
    double angle = atan2(
        sin(lon3 - lon2) * cos(lat3),
        cos(lat2) * sin(lat3) - sin(lat2) * cos(lat3) * cos(lon3 - lon2)
    );

     // Convert the angle from radians to degrees and adjust for heading normalization
    double bearing = fmod((angle * 180 / M_PI + 360), 360);
    double angleDiff = bearing - prevAngle;

    // Normalize the bearing angle to keep it within the range [-180, 180]
    if (angleDiff > 180) {
        bearing -= 360;
    } else if (angleDiff < -180) {
        bearing += 360;
    }

    return bearing;
}

//Uses the Haversine distance between the current node and goal node as a heuristic for estimating how close the node is to the goal, helping prioritize nodes in pathfinding.
// Heuristic function to estimate the distance between the current node and the goal
// A* heuristic function
double heuristic(int64_t node, int64_t goal, const unordered_map<int64_t, pair<double, double>>& nodes) {
    try {
        const auto& node_coords = nodes.at(node);
        const auto& goal_coords = nodes.at(goal);
        return haversineDistance(
            node_coords.first, node_coords.second,
            goal_coords.first, goal_coords.second
        );
    } catch (const out_of_range&) {
        throw runtime_error("Invalid node ID in heuristic calculation");
    }
}

void loadFromJSON(const json& data, 
                  unordered_map<int64_t, pair<double, double>>& nodes, 
                  unordered_map<int64_t, vector<pair<int64_t, double>>>& edges, 
                  unordered_map<pair<int64_t, int64_t>, double>& distanceCache) {
    try {
        // Clear existing data
        nodes.clear();
        edges.clear();
        distanceCache.clear();

        // Pre-allocate space
        auto& elements = data["elements"];
        nodes.reserve(elements.size());

        // First pass: Load all nodes
        for (auto& element : elements) {
            if (element["type"] == "node") {
                int64_t id = element["id"];
                double lat = element["lat"];
                double lon = element["lon"];
                // Validate coordinates
                if (lat < -90 || lat > 90 || lon < -180 || lon > 180) {
                    throw invalid_argument("Invalid coordinates in node " + to_string(id));
                }
                
                nodes[id] = {lat, lon};
            }
        }

        // Second pass: Create edges
        size_t edge_count = 0;
        for (const auto& element : elements) {
            if (element["type"] == "way") {
                const auto& nodeRefs = element["nodes"];
                const size_t size = nodeRefs.size();
                
                // Skip ways with less than 2 nodes
                if (size < 2) continue;
                
                // Pre-count edges for this way
                edge_count += size - 1;
                
                // Create edges between consecutive nodes
                for (size_t i = 0; i < size - 1; ++i) {
                    int64_t src = nodeRefs[i];
                    int64_t dst = nodeRefs[i + 1];
                    
                    // Validate node existence
                    if (!nodes.count(src) || !nodes.count(dst)) {
                        continue; // Skip invalid node references
                    }
                    
                    const auto& src_coords = nodes[src];
                    const auto& dst_coords = nodes[dst];
                    
                    // Calculate and cache distance
                    auto distance = haversineDistance(
                        src_coords.first, src_coords.second,
                        dst_coords.first, dst_coords.second
                    );
                    
                    // Create bidirectional edges
                    edges[src].push_back({dst, distance});
                    edges[dst].push_back({src, distance});
                    
                    // Cache distances for both directions
                    distanceCache[{src, dst}] = distance;
                    distanceCache[{dst, src}] = distance;
                }
            }
        }

        // Reserve space for edge vectors based on average connectivity
        if (!nodes.empty()) {
            size_t avg_edges_per_node = (edge_count * 2) / nodes.size();
            for (auto& [_, edge_vec] : edges) {
                edge_vec.reserve(avg_edges_per_node);
            }
        }

    } catch (const json::exception& e) {
        throw runtime_error("JSON parsing error: " + string(e.what()));
    } catch (const exception& e) {
        throw runtime_error("Error loading graph: " + string(e.what()));
    }
}

vector<json> findPath(const json& start, const json& end, 
                      const unordered_map<int64_t, pair<double, double>>& nodes, 
                      unordered_map<int64_t, vector<pair<int64_t, double>>>& edges, 
                      unordered_map<pair<int64_t, int64_t>, double>& distanceCache) {
    // Validate input nodes
    if (!nodes.count(start["id"]) || !nodes.count(end["id"])) {
        return {};
    }

    // Custom comparator for the priority queue
    struct CompareNode {
        bool operator()(const pair<double, int64_t>& a, 
                       const pair<double, int64_t>& b) const {
            return a.first > b.first;
        }
    };

    // Initialize data structures
    unordered_map<int64_t, double> gScore;
    unordered_map<int64_t, int64_t> prev;
    priority_queue<pair<double, int64_t>, vector<pair<double, int64_t>>, CompareNode> pq;

    // Initialize scores
    gScore.reserve(nodes.size());
    for (const auto& node : nodes) {
        gScore[node.first] = numeric_limits<double>::infinity();
    }
    gScore[start["id"]] = 0;

    // Initialize priority queue with start node
    pq.push({heuristic(start["id"], end["id"], nodes), start["id"]});

    // A* algorithm
    while (!pq.empty()) {
        int64_t current = pq.top().second;
        pq.pop();

        // Found the destination
        if (current == end["id"]) break;

        // Look at all neighbors
        auto it = edges.find(current);
        if (it == edges.end()) continue;

        for (const auto& [next, weight] : it->second) {
            double newScore = gScore[current] + weight;
            
            if (newScore < gScore[next]) {
                prev[next] = current;
                gScore[next] = newScore;
                double priority = newScore + heuristic(next, end["id"], nodes);
                pq.push({priority, next});
            }
        }
    }

    // Check if path exists
    if (gScore[end["id"]] == numeric_limits<double>::infinity()) {
        return {};
    }

    // Reconstruct path
    vector<json> path;
    path.reserve(nodes.size() / 4);  // Reasonable initial capacity
    
    double prevAngle = 0.0;
    for (int64_t at = end["id"]; at != start["id"]; ) {
        auto it = prev.find(at);
        if (it == prev.end()) return {}; 
        
        int64_t prevNode = it->second;
        double distance = distanceCache[{prevNode, at}];
        double angle = calculateAngle(nodes.at(prevNode), nodes.at(at), nodes.at(it->second), prevAngle);
        
        path.push_back(json{
            {"id", at},
            {"lat", nodes.at(at).first},
            {"lon", nodes.at(at).second},
            {"type", "node"},
            {"distance", distance},
            {"angle", angle}
        });
        prevAngle = angle;
        at = it->second;
    }
    path.push_back(json{
        {"id", start["id"]},
        {"lat", nodes.at(start["id"]).first},
        {"lon", nodes.at(start["id"]).second},
        {"type", "node"},
        {"distance", 0.0},
        {"angle", 0.0}
    });
    
    reverse(path.begin(), path.end());
    return path;
}

json getPathState(const unordered_map<int64_t, pair<double, double>>& nodes, 
                  const unordered_map<int64_t, vector<pair<int64_t, double>>>& edges, 
                  const unordered_map<pair<int64_t, int64_t>, double>& distanceCache) {
    json state = {
        {"node_count", nodes.size()},
        {"edge_count", edges.size()},
        {"cache_size", distanceCache.size()}
    };

    // Add some basic statistics
    if (!nodes.empty()) {
        size_t total_edges = 0;
        size_t max_edges = 0;
        for (const auto& [_, edge_list] : edges) {
            total_edges += edge_list.size();
            max_edges = max(max_edges, edge_list.size());
        }

        state["average_edges_per_node"] = static_cast<double>(total_edges) / nodes.size();
        state["max_edges_per_node"] = max_edges;
    }

    return state;
}

string printGraph(const unordered_map<int64_t, pair<double, double>>& nodes, 
                  const unordered_map<int64_t, vector<pair<int64_t, double>>>& edges) {
    stringstream ss;
    ss << "Graph Structure:\n";
    ss << "===============\n\n";
    
    // Print nodes
    ss << "Nodes (" << nodes.size() << " total):\n";
    ss << "-----------------\n";
    for (const auto& [id, coords] : nodes) {
        ss << "Node " << id << ": (" << fixed << setprecision(6) 
           << coords.first << ", " << coords.second << ")\n";
    }
    
    // Print edges
    ss << "\nEdges:\n";
    ss << "-----------------\n";
    for (const auto& [src, destinations] : edges) {
        ss << "From Node " << src << ":\n";
        for (const auto& [dst, distance] : destinations) {
            ss << "  → Node " << dst << " (distance: " << fixed << setprecision(2) 
               << distance << "m)\n";
        }
    }
    
    // Print some statistics
    ss << "\nGraph Statistics:\n";
    ss << "-----------------\n";
    ss << "Total Nodes: " << nodes.size() << "\n";
    ss << "Total Edges: " << edges.size() << "\n";
    ss << "Cached Distances: " << distanceCache.size() << "\n";
    
    // Calculate average connectivity
    if (!nodes.empty()) {
        size_t total_edges = 0;
        size_t max_edges = 0;
        for (const auto& [_, edge_list] : edges) {
            total_edges += edge_list.size();
            max_edges = max(max_edges, edge_list.size());
        }
        double avg_edges = static_cast<double>(total_edges) / nodes.size();
        ss << "Average Edges per Node: " << fixed << setprecision(2) 
           << avg_edges << "\n";
        ss << "Max Edges for a Node: " << max_edges << "\n";
    }
    
    return ss.str();
}

bool verifyGraph(const unordered_map<int64_t, pair<double, double>>& nodes, 
                 const unordered_map<int64_t, vector<pair<int64_t, double>>>& edges) {
    // Check for invalid coordinates
    for (const auto& [id, coords] : nodes) {
        if (coords.first < -90 || coords.first > 90 || coords.second < -180 || coords.second > 180) {
            cout << "Invalid coordinates for node " << id << endl;
            return false;
        }
    }
    
    // Verify edge consistency
    for (const auto& [src, destinations] : edges) {
        // Check if source node exists
        if (!nodes.count(src)) {
            cout << "Edge references non-existent source node " << src << endl;
            return false;
        }
        
        // Check each destination
        for (const auto& [dst, distance] : destinations) {
            if (!nodes.count(dst)) {
                cout << "Edge references non-existent destination node " << dst << endl;
                return false;
            }
            
            // Verify distance is positive and reasonable
            if (distance <= 0 || distance > 1000000) { // 1000km seems reasonable max
                cout << "Suspicious distance " << distance 
                     << "m between nodes " << src << " and " << dst << endl;
                return false;
            }
            
            // Verify bidirectional edge exists
            bool found_reverse = false;
            auto it = edges.find(dst);
            if (it != edges.end()) {
                for (const auto& [rev_dst, rev_dist] : it->second) {
                    if (rev_dst == src) {
                        found_reverse = true;
                        // Check if distances match
                        if (abs(rev_dist - distance) > 0.01) {
                            cout << "Inconsistent distances for bidirectional edge "
                                 << src << " <-> " << dst << endl;
                            return false;
                        }
                        break;
                    }
                }
            }
            if (!found_reverse) {
                cout << "Missing reverse edge for " << src << " -> " << dst << endl;
                return false;
            }
        }
    }
    
    return true;
}
