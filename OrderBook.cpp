#include "OrderBook.h"

std::vector<priceLevel>::iterator OrderBook::findPriceLevel(std::vector<priceLevel>& levels, uint64_t price_level_to_find) {
    auto it_price_level = levels.end();

    uint64_t distance_from_front = 0;
    uint64_t distance_from_back = 0;

    if (!levels.empty()) {
        distance_from_front = abs(levels.front().price - price_level_to_find);
        distance_from_back = abs(levels.back().price - price_level_to_find);
    }

    //I think I could end these searches sooner maybe by checking when the level >< price we are seraching for, lowkey i need to find it for insertion

    if (distance_from_front <= distance_from_back) {
        it_price_level = std::find_if(levels.begin(), levels.end(), [&](const priceLevel& p) {
            return p.price >= price_level_to_find;

            //350 - to find

            //360 <- call insert on this spot
            //340
            //330
        });
    } else {
        it_price_level = std::find_if(levels.rbegin(), levels.rend(), [&](const priceLevel& p) {
            return p.price <= price_level_to_find;
        }).base();

        if (it_price_level != levels.end()) {
            ++it_price_level;
        }
        //345 - to find

        //355
        //350
        //340 <- we call insert on this
    }

    return it_price_level;
}

void OrderBook::insertOrder(Order order) {
    order.id = newID();
    auto& levels = order.buyOrSell ? bidLevels_ : askLevels_;
    auto price_level_to_find = order.price;
    auto id_to_insert = order.id;

    auto it_price_level = findPriceLevel(levels, price_level_to_find);
    //we need to save these iterators m8 also it will most likely never be end or it will I guess.
    //CHECK IF THIS CAN BE THE END...

    if (it_price_level == levels.end()) {
        std::cout << "End Price Level Insertion: ";
        
        levels.emplace_back(price_level_to_find);
        auto &orders = levels.back().orders;

        orders.push_back(std::move(order));

        orderMap_.insert({id_to_insert, std::prev(orders.end())}); 

    } else if (it_price_level->price == price_level_to_find) {
        std::cout << "Price Level Exists Insertion: ";

        it_price_level->orders.push_back(std::move(order));
        orderMap_.insert({id_to_insert, std::prev(it_price_level->orders.end())}); 
    } else {
        //WHAT IF THIS IS INSERTING AT THE END WE NEED TO PUSH BACK INSTEAD OF INSERTING
        std::cout << "Front or Middle Price Level Insertion: ";
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
        std::print("Order with id {} does not exist!", id);
        return std::nullopt;
    } else {
        return orderMap_.at(id); 
    }
}

uint64_t OrderBook::newID() {
    return (currID++);
}

void OrderBook::removeOrder(uint64_t id) {
    auto order = findOrder(id);
    if (!order.has_value()) {
        return;
    }

    auto order_it = order.value();
    uint64_t price = order_it->price;

    auto& levels = (order_it->buyOrSell) ? bidLevels_ : askLevels_;
    auto price_level_it = findPriceLevel(levels, price);

    //need to remove from levels and ordermap
    price_level_it->orders.erase(orderMap_.at(id));
    orderMap_.erase(id);
}

void OrderBook::printBook(bool buyOrSell) const {
    auto& levels = buyOrSell ? bidLevels_ : askLevels_;
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