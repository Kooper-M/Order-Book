#include <vector>
#include <ctime>
#include <unordered_map>
#include <list>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <print>

struct Order {
    uint64_t id;
    std::time_t time = std::time(nullptr);

    bool buyOrSell;

    uint64_t quantity;
    uint64_t price; 

    Order(bool bOs, uint64_t qty, uint64_t p) {
        buyOrSell = bOs;
        quantity = qty;
        price = p;
    }

    void printOrder() {
        std::string buyOrSellOut;
        if (buyOrSell) {
            buyOrSellOut = "Buy";
        } else {
            buyOrSellOut = "Sell";
        }
        std::print("({} Order, id: {} Price: {} Quantity: {})", buyOrSellOut, id, price, quantity);
    }
};

struct priceLevel {
    double price;
    std::list<Order> orders;

    priceLevel(int price) : price(price) {}
};

class OrderBook {
    public:
        void insertOrder(Order);
        std::optional<std::list<Order>::iterator> findOrder(uint64_t); 
        void removeOrder(uint64_t);
        std::vector<priceLevel>::iterator findPriceLevel(std::vector<priceLevel>&, uint64_t price);


        void printBook(bool) const;
        uint64_t newID();
        
    private:
        std::vector<priceLevel> askLevels_;
        std::vector<priceLevel> bidLevels_;
        std::unordered_map<uint64_t, std::list<Order>::iterator> orderMap_;
        uint64_t currID = 100;

        double askMin_ = INT_MAX;
        double bidMax_ = 0;
};