#include <pybind11/pybind11.h>
#include "OrderBook.h"
#include <format>

namespace py = pybind11;

PYBIND11_MODULE(OrderBook, m) {
    m.doc() = "orderbook plugin";

    py::class_<OrderBook>(m, "OrderBook")
        .def(py::init<>())
        .def("insertOrder", &OrderBook::insertOrder)
        .def("getTrades", [](OrderBook& book) {
            py::list result;

            for (const Trade& trade : book.getTrades()) {
                py::dict d;
                d["tradeId"] = trade.tradeId;
                d["tradePrice"] = trade.price;
                d["tradeQuantity"] = trade.quantity;

                result.append(d);
            }
            
            return result;
        })
        
        .def("getOrders", [](OrderBook& book) {
            py::list result;

            for (const auto& [id, order] : book.getOrders()) {
                py::dict d;
                d["Id"] = order->id;
                d["Side"] = (order->buyOrSell) ? "Buy" : "Sell";
                d["Price"] = order->price;
                d["Quantity"] = order->quantity;

                result.append(d);
            }

            return result;
        })
        
        .def("getOrder", [](OrderBook& book, uint64_t id) -> py::object {
            auto order = book.findOrder(id);

            if (!order.has_value()) {
                return py::none();
            } 

            auto orderIt = order.value();

            py::dict d;
            d["Side"] = (orderIt->buyOrSell) ? "Buy" : "Sell";
            d["Price"] = orderIt->price;
            d["Quantity"] = orderIt->quantity;

            return d;
        }, py::arg("id"))

        .def("removeOrder", &OrderBook::removeOrder)
        ;
}
