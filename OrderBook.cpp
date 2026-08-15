#include "OrderBook.h"
std::vector<priceLevel>::iterator OrderBook::findPriceLevel(std::vector<priceLevel>& levels, uint64_t price_level_to_find) {
    return std::lower_bound(
        levels.begin(),
        levels.end(),
        price_level_to_find,
        [&](const priceLevel& level, uint64_t price) {
            return level.price < price;
        }
    );
}

void OrderBook::insertOrder(bool BOS, uint64_t p, uint64_t q) {
    Order order(BOS, p, q);

    order.id = newID();
    matchOrder(order);
    if (order.quantity == 0) {
        return;
    }
    
    auto& levels = order.buyOrSell ? bidLevels_ : askLevels_;
    auto price_level_to_find = order.price;
    auto id_to_insert = order.id;

    auto it_price_level = findPriceLevel(levels, price_level_to_find);
    //we need to save these iterators m8 also it will most likely never be end or it will I guess.
    //CHECK IF THIS CAN BE THE END...

    if (it_price_level == levels.end()) {
        //std::cout << "End Price Level Insertion: ";
        
        levels.emplace_back(price_level_to_find);
        auto &orders = levels.back().orders;

        orders.push_back(std::move(order));

        orderMap_.insert({id_to_insert, std::prev(orders.end())}); 

    } else if (it_price_level->price == price_level_to_find) {
        //std::cout << "Price Level Exists Insertion: ";

        it_price_level->orders.push_back(std::move(order));
        orderMap_.insert({id_to_insert, std::prev(it_price_level->orders.end())}); 
    } else {
        //WHAT IF THIS IS INSERTING AT THE END WE NEED TO PUSH BACK INSTEAD OF INSERTING
        //std::cout << "Front or Middle Price Level Insertion: ";
        auto id = order.id;

        it_price_level = levels.emplace(it_price_level, price_level_to_find);
        
        it_price_level->orders.push_back(std::move(order));
        orderMap_.insert({id, std::prev((it_price_level)->orders.end())});
        
    }

    std::cout << std::endl;
}

std::optional<std::list<Order>::iterator> OrderBook::findOrder(uint64_t id) {
    auto it = orderMap_.find(id);
    if (it == orderMap_.end()) {
        std::println("Order with id {} does not exist!", id);
        return std::nullopt;
    } else {
        return orderMap_.at(id); 
    }
}

uint64_t OrderBook::newID() {
    return (currID++);
}

bool OrderBook::removeOrder(uint64_t id) {
    auto order = findOrder(id);
    if (!order.has_value()) {
        return false;
    }

    auto order_it = order.value();
    uint64_t price = order_it->price;

    auto& levels = (order_it->buyOrSell) ? bidLevels_ : askLevels_;
    auto price_level_it = findPriceLevel(levels, price);
    //need to remove from levels and ordermap
    price_level_it->orders.erase(orderMap_.at(id));
    orderMap_.erase(id);

    //std::println("Price {} level empty: {}", price_level_it->price, price_level_it->orders.empty());

    if (price_level_it->orders.empty()) {
        levels.erase(price_level_it);
    }

    return true;
}

void OrderBook::printBook(bool buyOrSell) const {
    auto& levels = buyOrSell ? bidLevels_ : askLevels_;
    if (levels.empty()) {
        std::print("The book is empty!\n");
    }
    for (auto it = levels.begin(); it != levels.end(); ++it) {
        std::print("{} - ", it->price);
        auto orders = it->orders;
        for (auto it3 = orders.begin(); it3 != orders.end(); ++it3) {
            it3->printOrder();
            if (std::next(it3) != orders.end())
                std::cout << ", ";
        }
        std::cout << std::endl;
    }
}

void OrderBook::printTrades() const {
    for (const auto& t : trades_) {
        t.print();
    }
}

void OrderBook::matchOrder(Order &order) {
    //auto orderId = order.id;
    auto orderPrice = order.price;
    //auto orderQuantity = order.quantity;
    auto buyOrSell = order.buyOrSell;
    auto& levels = (buyOrSell) ? askLevels_ : bidLevels_;  

    if (levels.empty()) {
        std::print("Other Map is empty \n");
        return;
    }
    
    bool orderMatched = false;

    std::println("ID after before execution: {}", order.id);
    if (buyOrSell) {
        //if this is a buy order
        //buy 10 @ 103
        //look in the lowest level under 103
        //lower bound looks for the first false, so where is the order price where the order price is greater than or equal the level price
        /*
        it = std::lower_bound(levels.begin(), levels.end(), orderPrice, [](const priceLevel& level, uint64_t price) {
            return level.price > price;
        });
        */
        while (!levels.empty() && order.quantity > 0 && levels.front().price <= orderPrice)  {
            orderMatched = true;
            std::println("Lowest Price to buy at {}", levels.front().price);

            auto& levelOrders = levels.front().orders;
            auto& oldestOrder = levelOrders.front();

            order.quantity = execute(order, oldestOrder); 
        } 
    } else {
        //if this is a sell order
        //sell 10 @ 30
        //look for the highest order above 30
        //
        while (!levels.empty() && order.quantity > 0 && levels.back().price >= orderPrice) {
            orderMatched = true;
            auto& level = levels.back();
            std::println("Highest Price to sell at {}", level.price);
            
            auto& oldestOrder = level.orders.front();
            order.quantity = execute(order, oldestOrder);
        }
        /*
        it = std::lower_bound(levels.rbegin(), levels.rend(), orderPrice, [&](const priceLevel& level, uint64_t price) {
            return level.price < orderPrice;
        }).base();
        */
    }

    if (!orderMatched) {
        std::println("Order could not be matched");
    }
}

uint64_t OrderBook::execute(Order &order1, Order &order2) {
    //we have a sell order at 10 units, and another sell order of 10
    //we have a buy order of 15 units 

    //need to split order between these two
    uint64_t quantity_to_remove = order1.quantity;
    std::print("Quant to remove: {}\n", quantity_to_remove);
    if (quantity_to_remove >= order2.quantity) {
        std::println("Order1 ID: {}, Order2 ID: {}", order1.id, order2.id);
        trades_.emplace_back(Trade(nextTradeId_++, order1, order2, order2.quantity));

        quantity_to_remove -= order2.quantity;
        order2.quantity = 0;
        
        removeOrder(order2.id);
    } else {
        std::println("Order1 ID: {}, Order2 ID: {}", order1.id, order2.id);
        trades_.emplace_back(Trade(nextTradeId_++, order1, order2, quantity_to_remove));
        order2.quantity -= quantity_to_remove;
        quantity_to_remove = 0;
        std::print("How much is left of the other order: {}\n", order2.quantity);
    }

    return quantity_to_remove;
}

const std::vector<Trade>& OrderBook::getTrades() {
    return trades_;
}

const std::unordered_map<uint64_t, std::list<Order>::iterator>& OrderBook::getOrders() {
    return orderMap_;
}
