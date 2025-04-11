#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cmath>
#include <iomanip>
#include <string>
#include <ctime>

using namespace std;

const int MAX_SIZE = 100;
const int MAX_LOCATIONS = 10;
const int INF = 999999;
const int HASH_TABLE_SIZE = 101; 

struct Shoe {
    string name;
    string category;
    double price;
    int sizes[4];
    int stock[4]; 
    string color;
};

struct Location {
    string name;
    Location* next;
};

struct Route {
    int destination;
    int distance;
    int shipping_cost[3];
    Route* next;
};

struct User {
    string username;
    string password;
    bool isAdmin;
    User* next; 
};

struct OrderItem {
    Shoe shoe;
    int size;
};

struct Order {
    string username;
    string date;
    string estimated_arrival;
    double total_price;
    int destination;
    double shipping_cost;
    string status;
    OrderItem items[MAX_SIZE];
    int item_count;
    Order* next;
};

struct OrderQueue {
    Order* front;
    Order* rear;
};

struct OrderStack {
    Order* top;
};

class Graph {
public:
    int num_locations;
    Location* locations[MAX_LOCATIONS];
    Route* routes[MAX_LOCATIONS];

    Graph(int num_locations) {
        this->num_locations = num_locations;
        for (int i = 0; i < num_locations; ++i) {
            locations[i] = nullptr;
            routes[i] = nullptr;
        }
    }

    void addLocation(int index, string name) {
        Location* newLocation = new Location{name, nullptr};
        locations[index] = newLocation;
    }

    void addRoute(int source, int destination, int distance, int siCepat_cost, int JNE_cost, int JNT_cost) {
        Route* newRoute = new Route{destination, distance, {siCepat_cost, JNE_cost, JNT_cost}, routes[source]};
        routes[source] = newRoute;
    }

    void displayGraph() {
        for (int i = 0; i < num_locations; ++i) {
            cout << "Location " << i << " (" << locations[i]->name << "):" << endl;
            Route* route = routes[i];
            while (route != nullptr) {
                cout << "  -> " << route->destination << " (" << locations[route->destination]->name << "), distance: " << route->distance 
                     << ", siCepat: " << route->shipping_cost[0] 
                     << ", JNE: " << route->shipping_cost[1] 
                     << ", JNT: " << route->shipping_cost[2] << endl;
                route = route->next;
            }
            cout << endl;
        }
    }

    void displayAdjacencyList() {
        cout << "== ADJACENCY LIST ==" << endl;
        for (int i = 0; i < num_locations; ++i) {
            cout << locations[i]->name << " -> ";
            Route* route = routes[i];
            while (route != nullptr) {
                cout << locations[route->destination]->name << " (distance: " << route->distance << "), ";
                route = route->next;
            }
            cout << endl;
        }
    }

    int getShippingCost(int start, int end, int courier) {
        int distance[MAX_LOCATIONS];
        int shippingCost[MAX_LOCATIONS];
        bool visited[MAX_LOCATIONS];

        for (int i = 0; i < num_locations; ++i) {
            distance[i] = INF;
            shippingCost[i] = INF;
            visited[i] = false;
        }

        distance[start] = 0;
        shippingCost[start] = 0;

        for (int i = 0; i < num_locations - 1; ++i) {
            int minDistance = INF;
            int minIndex = -1;

            for (int j = 0; j < num_locations; ++j) {
                if (!visited[j] && distance[j] < minDistance) {
                    minDistance = distance[j];
                    minIndex = j;
                }
            }

            if (minIndex == -1)
                break;

            visited[minIndex] = true;

            Route* route = routes[minIndex];
            while (route != nullptr) {
                int newDist = distance[minIndex] + route->distance;
                if (newDist < distance[route->destination]) {
                    distance[route->destination] = newDist;
                    shippingCost[route->destination] = shippingCost[minIndex] + route->shipping_cost[courier];
                }
                route = route->next;
            }
        }

        if (distance[end] == INF) {
            return -1;
        } else {
            return shippingCost[end];
        }
    }

    string getLocationName(int index) {
        if (index >= 0 && index < num_locations && locations[index] != nullptr) {
            return locations[index]->name;
        }
        return "Unknown";
    }
};

Shoe inventory[MAX_SIZE];
int inventory_count = 0;

OrderItem cart[MAX_SIZE];
int cart_count = 0;

OrderQueue orderQueue = {nullptr, nullptr};
OrderStack orderStack = {nullptr};

User* users[HASH_TABLE_SIZE] = {nullptr}; 
int user_count = 0;
int logged_in_user_index = -1;

Graph g(5); 

int hashFunction(string username) {
    int hash = 0;
    for (char c : username) {
        hash = (hash * 31 + c) % HASH_TABLE_SIZE;
    }
    return hash;
}

User* findUser(string username) {
    int index = hashFunction(username);
    User* current = users[index];
    while (current != nullptr) {
        if (current->username == username) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

bool registerUser() {
    string username, password;
    cout << "Masukkan username: ";
    cin >> username;
    cout << "Masukkan password: ";
    cin >> password;

    if (findUser(username) != nullptr) {
        cout << "Username sudah digunakan. Silakan pilih username lain." << endl;
        return false;
    }

    User* newUser = new User{username, password, false, nullptr};
    int index = hashFunction(username);
    newUser->next = users[index];
    users[index] = newUser;
    user_count++;
    cout << "Registrasi berhasil!" << endl;
    return true;
}

bool loginUser() {
    string username, password;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    User* user = findUser(username);
    if (user != nullptr && user->password == password) {
        logged_in_user_index = hashFunction(username);
        return true;
    }
    return false;
}

bool loginAdmin() {
    string username, password;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    User* user = findUser(username);
    if (user != nullptr && user->password == password && user->isAdmin) {
        logged_in_user_index = hashFunction(username);
        return true;
    }
    return false;
}

void createAdmin() {
    if (findUser("admin") != nullptr) {
        return; 
    }

    User* newAdmin = new User{"admin", "admin", true, nullptr};
    int index = hashFunction("admin");
    newAdmin->next = users[index];
    users[index] = newAdmin;
    user_count++;
}

void sortInventoryBySize() {
    for (int i = 0; i < inventory_count - 1; ++i) {
        for (int j = 0; j < inventory_count - i - 1; ++j) {
            if (inventory[j].sizes[0] > inventory[j + 1].sizes[0]) {
                Shoe temp = inventory[j];
                inventory[j] = inventory[j + 1];
                inventory[j + 1] = temp;
            }
        }
    }
}

void simpanKeFile() {
    ofstream file("inventaris.txt");
    if (file.is_open()) {
        for (int i = 0; i < inventory_count; ++i) {
            file << inventory[i].name << "," << inventory[i].category << ",";
            for (int j = 0; j < 4; ++j) {
                file << inventory[i].sizes[j];
                if (j < 3) file << "/";
            }
            file << ",";
            for (int j = 0; j < 4; ++j) {
                file << inventory[i].stock[j];
                if (j < 3) file << "/";
            }
            file << "," << inventory[i].color << "," << inventory[i].price << endl;
        }
        file.close();
    } else {
        cout << "Gagal membuka file untuk disimpan." << endl;
    }
}

void bacaDariFile() {
    ifstream file("inventaris.txt");
    if (file.is_open()) {
        while (!file.eof()) {
            Shoe new_shoe;
            string sizes, stocks;
            getline(file, new_shoe.name, ',');
            getline(file, new_shoe.category, ',');
            getline(file, sizes, ',');

            try {
                for (int i = 0; i < 4; ++i) {
                    new_shoe.sizes[i] = stoi(sizes.substr(0, sizes.find('/')));
                    sizes = sizes.substr(sizes.find('/') + 1);
                }
            } catch (const invalid_argument& e) {
                cerr << "Error: Invalid size format in file" << endl;
                continue;
            }

            getline(file, stocks, ',');
            try {
                for (int i = 0; i < 4; ++i) {
                    new_shoe.stock[i] = stoi(stocks.substr(0, stocks.find('/')));
                    stocks = stocks.substr(stocks.find('/') + 1);
                }
            } catch (const invalid_argument& e) {
                cerr << "Error: Invalid stock format in file" << endl;
                continue;
            }

            getline(file, new_shoe.color, ',');
            file >> new_shoe.price;
            file.ignore();
            if (!new_shoe.name.empty()) {
                inventory[inventory_count++] = new_shoe;
            }
        }
        file.close();
    } else {
        cout << "Gagal membuka file inventaris." << endl;
    }
}

void lihat_inventaris() {
    system("cls");
    cout << endl;
    cout << "== DAFTAR SEPATU ==" << endl;
    sortInventoryBySize();

    cout << setw(30) << left << "Nama" 
         << setw(20) << left << "Kategori" 
         << setw(20) << left << "Ukuran (4)" 
         << setw(20) << left << "Stok (4)" 
         << setw(15) << left << "Warna" 
         << setw(15) << left << "Harga" << endl;
    cout << "=========================================================================================" << endl;

    for (int i = 0; i < inventory_count; ++i) {
        cout << setw(30) << left << inventory[i].name
             << setw(20) << left << inventory[i].category
             << setw(20) << left << to_string(inventory[i].sizes[0]) + "/" + to_string(inventory[i].sizes[1]) + "/" + to_string(inventory[i].sizes[2]) + "/" + to_string(inventory[i].sizes[3])
             << setw(20) << left << to_string(inventory[i].stock[0]) + "/" + to_string(inventory[i].stock[1]) + "/" + to_string(inventory[i].stock[2]) + "/" + to_string(inventory[i].stock[3])
             << setw(15) << left << inventory[i].color
             << setw(15) << left << fixed << setprecision(2) << inventory[i].price << endl;
    }
}

void kelola_inventaris() {
    cout << endl;
    cout << "== MENU ADMIN ==" << endl;
    int choice;
    do {
        cout << "1. Tambah Sepatu" << endl;
        cout << "2. Hapus Sepatu" << endl;
        cout << "3. Lihat Inventaris" << endl;
        cout << "4. Kembali" << endl;
        cout << ">> ";
        cin >> choice;
        cin.ignore(); 
        if (choice == 1) {
            cout << "== TAMBAH SEPATU ==" << endl;
            if (inventory_count < MAX_SIZE) {
                Shoe new_shoe;
                cout << "Masukkan nama sepatu     : ";
                getline(cin, new_shoe.name);
                cout << "Masukkan kategori sepatu : ";
                getline(cin, new_shoe.category);
                for (int i = 0; i < 4; ++i) {
                    cout << "Masukkan ukuran sepatu " << i + 1 << " : ";
                    cin >> new_shoe.sizes[i];
                    cout << "Masukkan stok ukuran " << new_shoe.sizes[i] << " : ";
                    cin >> new_shoe.stock[i];
                }
                cout << "Masukkan warna sepatu    : ";
                cin.ignore(); 
                getline(cin, new_shoe.color);
                cout << "Masukkan harga sepatu    : ";
                cin >> new_shoe.price;
                inventory[inventory_count++] = new_shoe;
                sortInventoryBySize();
                simpanKeFile();
                cout << "Sepatu berhasil ditambahkan!" << endl;
            } else {
                cout << "Inventaris penuh." << endl;
            }
        } else if (choice == 2) {
            if (inventory_count > 0) {
                cout << "== HAPUS SEPATU ==" << endl;
                int delete_index;
                cout << "Masukkan nomor sepatu yang ingin dihapus (1-" << inventory_count << "): ";
                cin >> delete_index;
                if (delete_index > 0 && delete_index <= inventory_count) {
                    for (int i = delete_index - 1; i < inventory_count - 1; ++i) {
                        inventory[i] = inventory[i + 1];
                    }
                    --inventory_count;
                    sortInventoryBySize();
                    simpanKeFile();
                    cout << "Sepatu berhasil dihapus!" << endl;
                } else {
                    cout << "Nomor sepatu tidak valid." << endl;
                }
            } else {
                cout << "Inventaris kosong." << endl;
            }
        } else if (choice == 3) {
            lihat_inventaris();
        }
    } while (choice != 4);
}

void tambah_ke_keranjang() {
    cout << endl;
    cout << "== KERANJANG BELANJA ==" << endl;
    string name;
    int size;
    cout << "Masukkan nama sepatu yang ingin ditambahkan ke keranjang   : ";
    getline(cin, name);
    cout << "Masukkan ukuran sepatu yang ingin ditambahkan ke keranjang : ";
    cin >> size;
    bool found = false;

    name.erase(0, name.find_first_not_of(' '));
    name.erase(name.find_last_not_of(' ') + 1);

    for (int i = 0; i < inventory_count; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (inventory[i].name == name && inventory[i].sizes[j] == size) {
                if (inventory[i].stock[j] > 0) {
                    cart[cart_count].shoe = inventory[i];
                    cart[cart_count].size = size;
                    cart_count++;
                    inventory[i].stock[j]--; 
                    simpanKeFile();
                    found = true;
                    cout << "Sepatu berhasil ditambahkan ke keranjang!" << endl;
                } else {
                    cout << "Stok sepatu habis." << endl;
                }
                break;
            }
        }
        if (found) break;
    }
    if (!found) {
        cout << "Sepatu tidak ditemukan di inventaris." << endl;
    }
}

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    string date = to_string(ltm->tm_mday) + "/" +
                  to_string(1 + ltm->tm_mon) + "/" +
                  to_string(1900 + ltm->tm_year);

    return date;
}

string getEstimatedArrivalDate(int days) {
    time_t now = time(0);
    now += days * 24 * 60 * 60; 
    tm* ltm = localtime(&now);

    string date = to_string(ltm->tm_mday) + "/" +
                  to_string(1 + ltm->tm_mon) + "/" +
                  to_string(1900 + ltm->tm_year);

    return date;
}

void enqueueOrder(OrderQueue& queue, Order* new_order) {
    if (queue.rear == nullptr) {
        queue.front = queue.rear = new_order;
    } else {
        queue.rear->next = new_order;
        queue.rear = new_order;
    }
}

Order* dequeueOrder(OrderQueue& queue) {
    if (queue.front == nullptr) {
        return nullptr;
    }
    Order* temp = queue.front;
    queue.front = queue.front->next;
    if (queue.front == nullptr) {
        queue.rear = nullptr;
    }
    return temp;
}

void pushOrder(OrderStack& stack, Order* new_order) {
    new_order->next = nullptr; 
    if (stack.top == nullptr) {
        stack.top = new_order;
    } else {
        new_order->next = stack.top;
        stack.top = new_order;
    }
}

void check_out() {
    cout << endl;
    cout << "== CHECK OUT ==" << endl;
    double total_price = 0;
    for (int i = 0; i < cart_count; ++i) {
        total_price += cart[i].shoe.price;
    }
    cout << "Total harga pesanan : " << fixed << setprecision(2) << total_price << endl;
    if (total_price == 0) {
        cout << "Keranjang belanja anda kosong! Silahkan tambah barang terlebih dahulu" << endl;
        cout << " " << endl;
        cin.get();
        return;
    }

    int destination;
    cout << "Pilih destinasi pengiriman:" << endl;
    cout << "0. Jakarta" << endl;
    cout << "1. Bogor" << endl;
    cout << "2. Depok" << endl;
    cout << "3. Tangerang" << endl;
    cout << "4. Bekasi" << endl;
    cout << ">> ";
    cin >> destination;

    int courier;
    cout << "Pilih kurir pengiriman:" << endl;
    cout << "0. siCepat" << endl;
    cout << "1. JNE" << endl;
    cout << "2. JNT" << endl;
    cout << ">> ";
    cin >> courier;

    double shipping_cost = g.getShippingCost(0, destination, courier); 

    if (shipping_cost == -1) {
        cout << "Tidak ada rute untuk destinasi pengiriman yang dipilih." << endl;
        return;
    }

    double total_with_shipping = total_price + shipping_cost;
    cout << "Total harga pesanan termasuk ongkir : " << total_with_shipping << endl;

    string estimated_arrival = getEstimatedArrivalDate(3); 
    cout << "Estimasi kedatangan pesanan: " << estimated_arrival << endl;

    cout << "Item pesanan: " << endl;
    for (int i = 0; i < cart_count; ++i) {
        cout << "  " << cart[i].shoe.name << " (Ukuran: " << cart[i].size << ", Harga: " << cart[i].shoe.price << ")" << endl;
    }

    Order* new_order = new Order{users[logged_in_user_index]->username, getCurrentDate(), estimated_arrival, total_price, destination, shipping_cost, "Pending", {}, cart_count, nullptr};
    for (int i = 0; i < cart_count; ++i) {
        new_order->items[i] = cart[i];
    }
    enqueueOrder(orderQueue, new_order);
    pushOrder(orderStack, new_order);

    cart_count = 0;
    simpanKeFile();
    cout << "Check out berhasil! Pesanan Anda sedang menunggu konfirmasi admin." << endl;
}

void lihat_order_history() {
    cout << endl;
    cout << "== RIWAYAT PEMESANAN ==" << endl;
    if (orderStack.top == nullptr) {
        cout << "Belum ada riwayat pemesanan." << endl;
        return;
    }

    Order* current = orderStack.top;
    while (current != nullptr) {
        Order* temp = current->next; 
        if (current->username == users[logged_in_user_index]->username) {
            cout << "Username             : " << current->username << endl
                 << "Tanggal              : " << current->date << endl
                 << "Estimasi Kedatangan  : " << current->estimated_arrival << endl
                 << "Total Harga          : " << fixed << setprecision(2) << current->total_price << endl
                 << "Destinasi            : " << g.getLocationName(current->destination) << endl
                 << "Ongkir               : " << fixed << setprecision(2) << current->shipping_cost << endl
                 << "Status               : " << current->status << endl;
            cout << "Items:" << endl;
            for (int i = 0; i < current->item_count; ++i) {
                cout << "  " << current->items[i].shoe.name << " (Ukuran: " << current->items[i].size << ", Harga: " << current->items[i].shoe.price << ")" << endl;
            }
            cout << "---------------------------------------" << endl;
        }
        current = temp;
    }
}

void cek_harga_ongkir() {
    cout << "PILIH DESTINASI PENGIRIMAN" << endl;
    cout << "0. Jakarta" << endl;
    cout << "1. Bogor" << endl;
    cout << "2. Depok" << endl;
    cout << "3. Tangerang" << endl;
    cout << "4. Bekasi" << endl;
    int destination;
    cout << ">> ";
    cin >> destination;

    int courier;
    cout << "Pilih kurir pengiriman:" << endl;
    cout << "0. siCepat" << endl;
    cout << "1. JNE" << endl;
    cout << "2. JNT" << endl;
    cout << ">> ";
    cin >> courier;

    double shipping_cost = g.getShippingCost(0, destination, courier); 

    if (shipping_cost == -1) {
        cout << "Tidak ada rute untuk destinasi pengiriman yang dipilih." << endl;
    } else {
        cout << "Ongkos kirim: " << shipping_cost << endl;
    }
}

void acceptOrder(Order* order) {
    order->status = "Accepted";
    cout << "Pesanan diterima." << endl;
}

void rejectOrder(Order* order) {
    order->status = "Rejected";
    cout << "Pesanan ditolak." << endl;
}

void processOrders() {
    cout << endl;
    cout << "== PROSES PESANAN ==" << endl;
    Order* current;
    while ((current = dequeueOrder(orderQueue)) != nullptr) {
        cout << "Username             : " << current->username << endl
            << "Tanggal              : " << current->date << endl
            << "Estimasi Kedatangan  : " << current->estimated_arrival << endl
            << "Total Harga          : " << fixed << setprecision(2) << current->total_price << endl
            << "Destinasi            : " << g.getLocationName(current->destination) << endl
            << "Ongkir               : " << fixed << setprecision(2) << current->shipping_cost << endl
            << "Status               : " << current->status << endl;
        cout << "Items:" << endl;
        for (int i = 0; i < current->item_count; ++i) {
            cout << "  " << current->items[i].shoe.name << " (Ukuran: " << current->items[i].size << ", Harga: " << current->items[i].shoe.price << ")" << endl;
        }
        cout << "---------------------------------------" << endl;

        int choice;
        cout << "1. Terima pesanan" << endl;
        cout << "2. Tolak pesanan" << endl;
        cout << ">> ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            acceptOrder(current);
        } else if (choice == 2) {
            rejectOrder(current);
        } else {
            cout << "Pilihan tidak valid." << endl;
        }
        pushOrder(orderStack, current);
    }
}

void viewAllOrderHistory() {
    cout << endl;
    cout << "== SELURUH RIWAYAT PEMESANAN ==" << endl;
    if (orderStack.top == nullptr) {
        cout << "Belum ada riwayat pemesanan." << endl;
    } else {
        Order* current = orderStack.top;
        while (current != nullptr) {
            cout << "Username             : " << current->username << endl
                 << "Tanggal              : " << current->date << endl
                 << "Estimasi Kedatangan  : " << current->estimated_arrival << endl
                 << "Total Harga          : " << fixed << setprecision(2) << current->total_price << endl
                 << "Destinasi            : " << g.getLocationName(current->destination) << endl
                 << "Ongkir               : " << fixed << setprecision(2) << current->shipping_cost << endl
                 << "Status               : " << current->status << endl;
            cout << "Items:" << endl;
            for (int i = 0; i < current->item_count; ++i) {
                cout << "  " << current->items[i].shoe.name << " (Ukuran: " << current->items[i].size << ", Harga: " << current->items[i].shoe.price << ")" << endl;
            }
            cout << "---------------------------------------" << endl;
            current = current->next;
        }
    }
    cout << "Tekan [Enter] untuk kembali ke menu admin...";
    cin.ignore();
}

void cari_sepatu() {
    cout << "Masukkan nama sepatu yang ingin dicari: ";
    string keyword;
    cin.ignore();
    getline(cin, keyword);

    keyword.erase(0, keyword.find_first_not_of(' '));
    keyword.erase(keyword.find_last_not_of(' ') + 1);

    cout << "Hasil pencarian untuk '" << keyword << "':" << endl;
    bool found = false;
    for (int i = 0; i < inventory_count; ++i) {
        if (inventory[i].name.find(keyword) != string::npos) {
            cout << setw(30) << left << inventory[i].name
                 << setw(20) << left << inventory[i].category
                 << setw(20) << left << to_string(inventory[i].sizes[0]) + "/" + to_string(inventory[i].sizes[1]) + "/" + to_string(inventory[i].sizes[2]) + "/" + to_string(inventory[i].sizes[3])
                 << setw(20) << left << to_string(inventory[i].stock[0]) + "/" + to_string(inventory[i].stock[1]) + "/" + to_string(inventory[i].stock[2]) + "/" + to_string(inventory[i].stock[3])
                 << setw(15) << left << inventory[i].color
                 << setw(15) << left << fixed << setprecision(2) << inventory[i].price << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "Tidak ada sepatu yang sesuai dengan kata kunci." << endl;
    }
}

void hapus_dari_keranjang() {
    cout << "Masukkan nama sepatu yang ingin dihapus dari keranjang: ";
    string name;
    getline(cin, name);

    name.erase(0, name.find_first_not_of(' '));
    name.erase(name.find_last_not_of(' ') + 1);

    int found_index = -1;
    for (int i = 0; i < cart_count; ++i) {
        if (cart[i].shoe.name == name) {
            found_index = i;
            break;
        }
    }

    if (found_index != -1) {
        string filename = "inventaris.txt";
        std::ifstream file_in(filename);
        std::ofstream file_out("temp.txt"); 

        string line;
        while (std::getline(file_in, line)) {
            std::string sepatu_nama, sepatu_kategori, sepatu_ukuran, sepatu_stok, sepatu_warna, sepatu_harga;
            std::istringstream iss(line);
            std::getline(iss, sepatu_nama, ',');
            std::getline(iss, sepatu_kategori, ',');
            std::getline(iss, sepatu_ukuran, ',');
            std::getline(iss, sepatu_stok, ',');
            std::getline(iss, sepatu_warna, ',');
            std::getline(iss, sepatu_harga, ',');

            if (sepatu_nama == cart[found_index].shoe.name) {
                char* stok_cstr = new char[sepatu_stok.length() + 1];
                strcpy(stok_cstr, sepatu_stok.c_str());

                int size_index = cart[found_index].size - cart[found_index].shoe.sizes[0];
                char* token = strtok(stok_cstr, "/");
                for (int i = 0; token != nullptr && i < 4; ++i) {
                    if (i == size_index) {
                        int new_stock = std::stoi(token) + 1;
                        sepatu_stok.replace(sepatu_stok.find(token), token - stok_cstr + strlen(token), std::to_string(new_stock));
                    }
                    token = strtok(nullptr, "/");
                }

                delete[] stok_cstr;

                file_out << sepatu_nama << "," << sepatu_kategori << "," << sepatu_ukuran << "," << sepatu_stok << "," << sepatu_warna << "," << sepatu_harga << "\n";
            } else {
                file_out << line << "\n";
            }
        }

        file_in.close();
        file_out.close();

        std::remove(filename.c_str()); 
        std::rename("temp.txt", filename.c_str()); 

        for (int i = found_index; i < cart_count - 1; ++i) {
            cart[i] = cart[i + 1];
        }
        cart_count--;

        cout << "Item berhasil dihapus dari keranjang!" << endl;
    } else {
        cout << "Item tidak ditemukan dalam keranjang." << endl;
    }
}

void menuUser() {
    int choice;
    do {
        cout << endl;
        cout << "=============================" << endl;
        cout << "        MENU USER           " << endl;
        cout << "=============================" << endl;
        cout << "1. Lihat Sepatu" << endl;
        cout << "2. Tambah ke Keranjang" << endl;
        cout << "3. Hapus dari Keranjang" << endl;
        cout << "4. Check Out" << endl;
        cout << "5. Riwayat Pemesanan" << endl;
        cout << "6. Cek Harga Ongkir" << endl;
        cout << "7. Cari Sepatu" << endl;
        cout << "8. Logout" << endl;
        cout << "Pilih: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            lihat_inventaris();
            break;
        case 2:
            tambah_ke_keranjang();
            system("pause");
            break;
        case 3:
            hapus_dari_keranjang();
            system("pause");
            break;
        case 4:
            check_out();
            system("pause");
            break;
        case 5:
            lihat_order_history();
            system("pause");
            break;
        case 6:
            cek_harga_ongkir();
            system("pause");
            break;
        case 7:
            cari_sepatu();
            system("pause");
            break;
        case 8:
            cout << "Logout berhasil. Kembali ke menu utama..." << endl;
            return;
        default:
            cout << "Pilihan tidak valid." << endl;
            system("pause");
        }
    } while (true);
}

void menuAdmin() {
    int choice;
    do {
        cout << endl;
        cout << "=============================" << endl;
        cout << "       MENU ADMIN           " << endl;
        cout << "=============================" << endl;
        cout << "1. Kelola Inventaris" << endl;
        cout << "2. Proses Pesanan" << endl;
        cout << "3. Tampilkan Rute" << endl; 
        cout << "4. Lihat Semua Riwayat Pemesanan" << endl; 
        cout << "5. Logout" << endl;
        cout << "Pilih: ";
        cin >> choice;
        cin.ignore(); 

        switch (choice) {
        case 1:
            kelola_inventaris();
            system("pause");
            break;
        case 2:
            processOrders();
            system("pause");
            break;
        case 3:
            g.displayAdjacencyList();
            system("pause");
            break;
        case 4: 
            viewAllOrderHistory();
            system("pause");
            break;
        case 5:
            cout << "Logout berhasil. Kembali ke menu utama..." << endl;
            return;
        default:
            cout << "Pilihan tidak valid." << endl;
            system("pause");
        }
    } while (true);
}

int main() {
    bacaDariFile();
    createAdmin();

    g.addLocation(0, "Jakarta");
    g.addLocation(1, "Bogor");
    g.addLocation(2, "Depok");
    g.addLocation(3, "Tangerang");
    g.addLocation(4, "Bekasi");

    g.addRoute(0, 1, 150, 10000, 12000, 14000);
    g.addRoute(0, 2, 500, 12000, 14000, 16000);
    g.addRoute(1, 0, 150, 10000, 12000, 14000);
    g.addRoute(1, 3, 300, 14000, 16000, 18000);
    g.addRoute(2, 0, 500, 12000, 14000, 16000);
    g.addRoute(2, 3, 400, 16000, 18000, 20000);
    g.addRoute(3, 1, 300, 14000, 16000, 18000); 
    g.addRoute(3, 2, 400, 16000, 18000, 20000);
    g.addRoute(3, 4, 100, 18000, 20000, 22000);
    g.addRoute(4, 3, 100, 18000, 20000, 22000); 

    int choice;
    do {
        system("cls");
        cout << endl;
        cout << "=============================" << endl;
        cout << " SELAMAT DATANG DI SAPATOOSS " << endl;
        cout << "=============================" << endl;
        cout << "1. Register User Baru" << endl;
        cout << "2. Login User" << endl;
        cout << "3. Login Admin" << endl;
        cout << "4. Keluar" << endl;
        cout << "Pilih: ";
        cin >> choice;
        cin.ignore(); 

        switch (choice) {
        case 1:
            registerUser();
            system("cls");
            break;
        case 2:
            if (loginUser()) {
                menuUser();
            } else {
                cout << "Username atau password salah. Akses ditolak." << endl;
                system("pause");
            }
            break;
        case 3:
            if (loginAdmin()) {
                menuAdmin();
            } else {
                cout << "Username atau password salah. Akses ditolak." << endl;
                system("pause");
            }
            break;
        case 4:
            cout << "Terima kasih, datang kembali ! :)" << endl;
            system("pause");
            break;
        default:
            cout << "Pilihan tidak valid." << endl;
            system("pause");
        }
    } while (choice != 4);

    return 0;
}