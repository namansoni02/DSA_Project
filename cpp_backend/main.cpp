#include <iostream>
#include <string>
#include <vector>
#include<bits/stdc++.h>
using namespace std;

// Class for menu items
class MenuItem {
public:
    string name;
    int price;

    MenuItem(string n, int p) : name(n), price(p) {}
};

// Class for restaurants
class Restaurant {
public:
    string name;
    vector<MenuItem> menu;

    // Constructor to initialize the restaurant with a name and a menu
    Restaurant(string n, vector<MenuItem> m) : name(n), menu(m) {}

    // Display the menu of the restaurant
    void displayMenu() const {
        cout << "Menu for " << name << ":\n";
        for (size_t i = 0; i < menu.size(); i++) {
            cout << "(" << i + 1 << ") " << menu[i].name << " - ₹" << menu[i].price << "\n";
        }
    }
};

// Class for order items, storing selected items and their quantities
class OrderedItem {
public:
    MenuItem item;
    int quantity;

    OrderedItem(MenuItem it, int qty) : item(it), quantity(qty) {}
};

// Class for the order, storing selected items and calculating the bill
class Order {
public:
    vector<OrderedItem> selectedItems;

    // Add item to the order with a specific quantity
    void addItem(const MenuItem& item, int quantity) {
        selectedItems.push_back(OrderedItem(item, quantity));
    }

    // Display the final bill
    void displayBill() const {
        int total = 0;
        cout << "\nYour Bill:\n";
        for (size_t i = 0; i < selectedItems.size(); i++) {
            cout << selectedItems[i].item.name << " (x" << selectedItems[i].quantity << ") - ₹" 
                 << (selectedItems[i].item.price * selectedItems[i].quantity) << "\n";
            total += selectedItems[i].item.price * selectedItems[i].quantity;
        }
        cout << "Total Amount: ₹" << total << "\n";
    }
};

// Function to create a restaurant with predefined menu items based on the name
Restaurant createRestaurant(const string& name) {
    vector<MenuItem> items;

    if (name == "Maharaja’s Feast") {
        items.push_back(MenuItem("Dal Makhani", 190));
        items.push_back(MenuItem("Chicken Tikka Masala", 250));
        items.push_back(MenuItem("Basmati Rice", 90));
        items.push_back(MenuItem("Naan Bread", 25));
        items.push_back(MenuItem("Ras Malai", 60));
        items.push_back(MenuItem("Tandoori Paneer", 200));
        items.push_back(MenuItem("Masala Papad", 20));
    } else if (name == "Spice Bazaar") {
        items.push_back(MenuItem("Butter Chicken", 250));
        items.push_back(MenuItem("Paneer Tikka", 180));
        items.push_back(MenuItem("Garlic Naan", 30));
        items.push_back(MenuItem("Biryani", 220));
        items.push_back(MenuItem("Mango Lassi", 50));
        items.push_back(MenuItem("Dal Fry", 120));
        items.push_back(MenuItem("Tandoori Roti", 20));
    } else if (name == "Saffron Garden") {
        items.push_back(MenuItem("Chicken Korma", 260));
        items.push_back(MenuItem("Rogan Josh", 270));
        items.push_back(MenuItem("Vegetable Pulao", 150));
        items.push_back(MenuItem("Tandoori Roti", 20));
        items.push_back(MenuItem("Gulab Jamun", 40));
        items.push_back(MenuItem("Paneer Butter Masala", 200));
        items.push_back(MenuItem("Lassi", 50));
    } else if (name == "Curry Culture") {
        items.push_back(MenuItem("Malai Kofta", 210));
        items.push_back(MenuItem("Lamb Vindaloo", 280));
        items.push_back(MenuItem("Prawn Curry", 300));
        items.push_back(MenuItem("Aloo Gobi", 160));
        items.push_back(MenuItem("Masala Chai", 30));
        items.push_back(MenuItem("Tandoori Mushroom", 190));
        items.push_back(MenuItem("Naan Bread", 25));
    } else if (name == "Masala Mystique") {
        items.push_back(MenuItem("Palak Paneer", 180));
        items.push_back(MenuItem("Fish Curry", 270));
        items.push_back(MenuItem("Samosas", 40));
        items.push_back(MenuItem("Jeera Rice", 90));
        items.push_back(MenuItem("Jalebi", 50));
        items.push_back(MenuItem("Roti Basket", 80));
        items.push_back(MenuItem("Aloo Paratha", 60));
    } else if (name == "The Royal Tandoor") {
        items.push_back(MenuItem("Tandoori Chicken", 270));
        items.push_back(MenuItem("Lamb Seekh Kebab", 250));
        items.push_back(MenuItem("Paneer Butter Masala", 210));
        items.push_back(MenuItem("Garlic Butter Naan", 35));
        items.push_back(MenuItem("Kheer", 60));
        items.push_back(MenuItem("Aloo Tikki", 40));
        items.push_back(MenuItem("Veg Biryani", 160));
    } else if (name == "Taste of Tradition") {
        items.push_back(MenuItem("Mutton Biryani", 300));
        items.push_back(MenuItem("Baingan Bharta", 160));
        items.push_back(MenuItem("Raita", 30));
        items.push_back(MenuItem("Aloo Paratha", 50));
        items.push_back(MenuItem("Jaljeera", 30));
        items.push_back(MenuItem("Shahi Paneer", 210));
        items.push_back(MenuItem("Nimbu Pani", 20));
    } else if (name == "Chutney & Charcoal") {
        items.push_back(MenuItem("Veg Seekh Kebabs", 150));
        items.push_back(MenuItem("Bhuna Gosht", 280));
        items.push_back(MenuItem("Prawn Biryani", 310));
        items.push_back(MenuItem("Lachha Paratha", 40));
        items.push_back(MenuItem("Rasgulla", 40));
        items.push_back(MenuItem("Chicken Malai Tikka", 240));
        items.push_back(MenuItem("Pani Puri", 30));
    } else if (name == "Bombay Bistro") {
        items.push_back(MenuItem("Vada Pav", 20));
        items.push_back(MenuItem("Chicken Kolhapuri", 260));
        items.push_back(MenuItem("Misal Pav", 50));
        items.push_back(MenuItem("Pav Bhaji", 80));
        items.push_back(MenuItem("Falooda", 60));
        items.push_back(MenuItem("Sabudana Khichdi", 70));
        items.push_back(MenuItem("Masala Dosa", 90));
    } else if (name == "The Curry Leaf") {
        items.push_back(MenuItem("Hyderabadi Biryani", 280));
        items.push_back(MenuItem("Chicken Chettinad", 260));
        items.push_back(MenuItem("Pesarattu", 80));
        items.push_back(MenuItem("Appam", 40));
        items.push_back(MenuItem("Payasam", 50));
        items.push_back(MenuItem("Mutton Curry", 300));
        items.push_back(MenuItem("Masala Dosa", 80));
    } else {
        // Default case for unknown restaurant
        cout << "Restaurant not found! Creating an empty menu.\n";
    }

    return Restaurant(name, items);
}

int main() {
    string user = "SIR";
    int ResChoice, itemChoice, quantity;
    char addMore, chooseAgain;

    // Create restaurant menus
    vector<Restaurant> restaurants;
    restaurants.push_back(createRestaurant("Spice Bazaar")); 
    restaurants.push_back(createRestaurant("Saffron Garden"));
    restaurants.push_back(createRestaurant("Curry Culture"));
    restaurants.push_back(createRestaurant("Maharaja’s Feast"));
    restaurants.push_back(createRestaurant("Masala Mystique"));
    restaurants.push_back(createRestaurant("The Royal Tandoor"));
    restaurants.push_back(createRestaurant("Taste of Tradition"));
    restaurants.push_back(createRestaurant("Chutney & Charcoal"));
    restaurants.push_back(createRestaurant("Bombay Bistro"));
    restaurants.push_back(createRestaurant("The Curry Leaf"));

    Order order;

    cout << "***********************************" << endl;
    cout << "            WELCOME " << user << " !" << endl;
    cout << "***********************************" << endl;

    do {
        // Display list of restaurants
        cout << "\nPlease select Your Favourite Restaurant:\n";
        for (size_t i = 0; i < restaurants.size(); i++) {
            cout << "(" << i + 1 << ") " << restaurants[i].name << "\n";
        }
        cout << "Enter the number of your choice (or 0 to finish): ";
        cin >> ResChoice;

        if (ResChoice > 0 && ResChoice <= restaurants.size()) {
            Restaurant selectedRestaurant = restaurants[ResChoice - 1];
            selectedRestaurant.displayMenu();

            do {
                cout << "Select the menu item number to order (or 0 to change restaurant): ";
                cin >> itemChoice;

                if (itemChoice == 0) break;  // Change restaurant

                if (itemChoice > 0 && itemChoice <= selectedRestaurant.menu.size()) {
                    cout << "How many " << selectedRestaurant.menu[itemChoice - 1].name << " do you want? ";
                    cin >> quantity;
                    order.addItem(selectedRestaurant.menu[itemChoice - 1], quantity);
                    cout << quantity << " " << selectedRestaurant.menu[itemChoice - 1].name << " added to your order!\n";
                } else {
                    cout << "Invalid choice, please try again.\n";
                }

                cout << "Would you like to add another item? (y/n): ";
                cin >> addMore;

            } while (addMore == 'y' || addMore == 'Y');

        } else {
            cout << "Invalid choice, please try again.\n";
        }

        cout << "Would you like to choose another restaurant? (y/n): ";
        cin >> chooseAgain;

    } while (chooseAgain == 'y' || chooseAgain == 'Y');

    // Display final bill
    order.displayBill();

    return 0;
}
