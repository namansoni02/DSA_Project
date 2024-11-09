*************************************************  KINDLY COMMENT DOWN THE PART YOU ARE REMOVING, FOR EASE OF UNDERSTANDING FOR OTHERS - DO NOT REMOVE THE PART - JUST COMMENT IT *********************************************************************

#include "graph.hpp"
#include <queue>
#include <unordered_map>
#include <vector>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

// Haversine distance calculation
double haversineDistance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371000; // Earth radius in meters
    const double phi1 = lat1 * M_PI / 180;
    const double phi2 = lat2 * M_PI / 180;
    const double deltaPhi = (lat2 - lat1) * M_PI / 180;
    const double deltaLambda = (lon2 - lon1) * M_PI / 180;

    const double a = sin(deltaPhi / 2) * sin(deltaPhi / 2) +
                     cos(phi1) * cos(phi2) *
                     sin(deltaLambda / 2) * sin(deltaLambda / 2);

    const double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;
}

// Function to calculate the angle between two points
double calculateAngle(const pair<double, double>& prev, const pair<double, double>& curr, 
                      const pair<double, double>& next, double prevAngle) {
    double lat2 = curr.first * M_PI / 180;
    double lon2 = curr.second * M_PI / 180;
    double lat3 = next.first * M_PI / 180;
    double lon3 = next.second * M_PI / 180;

    double angle = atan2(
        sin(lon3 - lon2) * cos(lat3),
        cos(lat2) * sin(lat3) - sin(lat2) * cos(lat3) * cos(lon3 - lon2)
    );

    double bearing = fmod((angle * 180 / M_PI + 360), 360);
    double angleDiff = bearing - prevAngle;

    if (angleDiff > 180) {
        bearing -= 360;
    } else if (angleDiff < -180) {
        bearing += 360;
    }

    return bearing;
}

// Dijkstra's algorithm implementation to find the shortest path
vector<json> findPathDijkstra(const json& start, const json& end, 
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
    pq.push({0, start["id"]});

    // Dijkstra's algorithm
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
                pq.push({newScore, next});
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

// Function to load graph data from JSON
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
