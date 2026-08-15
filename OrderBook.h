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

    Order(bool bOs, uint64_t p, uint64_t qty) {
        buyOrSell = bOs;
        price = p;
        quantity = qty;
    }

    void printOrder() {
        std::string buyOrSellOut;
        if (buyOrSell) {
            buyOrSellOut = "Buy";
        } else {
            buyOrSellOut = "Sell";
        }
        std::print("({} Order | id: {} | Price: {} | Quantity: {})", buyOrSellOut, id, price, quantity);
    }
};

struct priceLevel {
    double price;
    std::list<Order> orders;

    priceLevel(int price) : price(price) {}
};

struct Trade {
    uint64_t tradeId;
    uint64_t buyId;
    uint64_t sellId;
    uint64_t price;
    uint64_t quantity;

    Trade(uint64_t id, Order incomingOrder, Order restingOrder, uint64_t quantity_traded) {
        tradeId = id;
        Order* buyOrder;
        Order* sellOrder;

        std::cout << "hi" << std::endl;
        if (incomingOrder.buyOrSell) {
            buyOrder = &incomingOrder;
            sellOrder = &restingOrder;
        } else {
            sellOrder = &incomingOrder;
            buyOrder = &restingOrder;
        }

        buyId = buyOrder->id;
        sellId = sellOrder->id;
        price = sellOrder->price;
        quantity = quantity_traded;
    }
    
    void print() const {
        std::print("(Trade Id: {} | Buy ID: {} | Sell ID: {} | Price: {} | Quantity: {})\n", 
            tradeId, buyId, sellId, price, quantity);
    }
};

class OrderBook {
    public:
        void insertOrder(bool, uint64_t, uint64_t);
        std::optional<std::list<Order>::iterator> findOrder(uint64_t); 
        bool removeOrder(uint64_t);
        std::vector<priceLevel>::iterator findPriceLevel(std::vector<priceLevel>&, uint64_t price);
        const std::vector<Trade> getTrades();
        const std::unordered_map<uint64_t, std::list<Order>::iterator> getOrders();


        void printBook(bool) const;
        void printTrades() const;
        
    private:
        std::vector<priceLevel> askLevels_;
        std::vector<priceLevel> bidLevels_;
        std::vector<Trade> trades_;
        std::unordered_map<uint64_t, std::list<Order>::iterator> orderMap_;
        uint64_t currID = 100;
        uint64_t nextTradeId_ = 1;

        double askMin_ = INT_MAX;
        double bidMax_ = 0;

        uint64_t newID();
        void matchOrder(Order &order);
        uint64_t execute(Order&, Order&);
};