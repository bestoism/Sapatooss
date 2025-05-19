# Sapatooss

Sapatooss is a C++ console-based application for managing a shoe store's inventory and customer orders. The system supports user and admin authentication, inventory management, shopping cart, order processing, shipping cost calculation, and order history tracking.

## Features

- **User & Admin Authentication**
- **Inventory Management:** Add, remove, and view shoes
- **Shopping Cart:** Add and remove items, checkout
- **Order Processing:** Admin can accept or reject orders
- **Shipping Cost Calculation:** Multiple couriers and destinations
- **Order History:** Track all orders for users and admin

## Data Structures Used

- Array for inventory and cart
- Linked list for users (hash table buckets)
- Queue for order processing
- Stack for order history
- Graph for shipping routes

## How to Run

1. **Compile the program:**
   ```sh
   g++ Sapatooss.cpp -o Sapatooss.exe
   ```
2. **Run the executable:**
   ```sh
   ./Sapatooss.exe
   ```
3. Make sure `inventaris.txt` is in the same directory for inventory persistence.

## Default Admin Account

- **Username:** `admin`
- **Password:** `admin`

## Notes

- All data is stored locally in `inventaris.txt`.
- The application runs in the terminal/command prompt.
- Shipping routes are pre-defined for Jabodetabek cities.
