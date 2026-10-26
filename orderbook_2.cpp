#include "httplib.h"
#include "nlohmann/json.hpp"
#include <bits/stdc++.h>

using json = nlohmann::json;


class OrderBook {

public:
	enum class OrderType { Market, Limit, Stop, GoodTillCancelled, FillOrKill_Limit };
	enum class Side { Buy, Sell };


	class Order {

	public:
		Order( int id, OrderType type, Side side, double price, int quantity )
		: id(id), type(type), side(side), price(price), quantity(quantity) {}

		int 		getId() 	const 	{ return id; }
		OrderType 	getType() 	const 	{ return type; }
		Side 		getSide() 	const 	{ return side; }
		double 		getPrice() 	const 	{ return price; }
		int 		getQuantity() 	const 	{ return quantity; }

		// Method to update quantity

	private:
		int 		id;
		OrderType 	type;
		Side 		side;
		double 		price;
		int 		quantity;
	};

	void placeOrder(const Order& order) {
		std::lock_guard<std::mutex> lock(mtx_);

		if (order.getSide() == Side::Buy) {
			buyOrder.push_back(order);
			std::sort(buyOrder.begin(), buyOrder.end(), [](const Order& a, const Order& b) { return a.getPrice() > b.getPrice(); });
		}
		else {
			sellOrder.push_back(order);
			std::sort(sellOrder.begin(), sellOrder.end(), [](const Order& a, const Order& b) { return a.getPrice() < b.getPrice(); });
		}

		// matchOrders();
	}
	
json toJson() const {
    std::lock_guard<std::mutex> lock(mtx_);

    json bids = json::array(), asks = json::array();
    for (const auto& o : buyOrder) {
        bids.push_back({
            {"id",       o.getId()},
            {"type",     o.getType()},
            {"side",     o.getSide()},
            {"price",    o.getPrice()},
            {"quantity", o.getQuantity()}
        });
    }
    for (const auto& o : sellOrder) {
        asks.push_back({
            {"id",       o.getId()},
            {"type",     o.getType()},
            {"side",     o.getSide()},
            {"price",    o.getPrice()},
            {"quantity", o.getQuantity()}
        });
    }
    return {{"bids", bids}, {"asks", asks}};
}


	// void cancelOrder() {}

	// void matchOrder() {}

	// void printOrders() const {}

private:
	std::vector<Order> buyOrder;
	std::vector<Order> sellOrder;
	mutable std::mutex mtx_;

	// Helper method to find a match for a given order
	
	// Helper method to execute an order
	
	// Helper method to print an individual order

};

NLOHMANN_JSON_SERIALIZE_ENUM(OrderBook::OrderType, {
    { OrderBook::OrderType::Market,            "market" },
    { OrderBook::OrderType::Limit,             "limit" },
    { OrderBook::OrderType::Stop,              "stop" },
    { OrderBook::OrderType::GoodTillCancelled, "gtc" },
    { OrderBook::OrderType::FillOrKill_Limit,  "fok_limit" }
})

NLOHMANN_JSON_SERIALIZE_ENUM(OrderBook::Side, {
    { OrderBook::Side::Buy,  "buy" },
    { OrderBook::Side::Sell, "sell" }
})


int main() {

	httplib::Server svr;
	OrderBook orderbook;

	svr.set_mount_point( "/", "./public" );

	svr.Get( "/orderbook", [&orderbook](const httplib::Request&, httplib::Response& res) {
		res.set_content(orderbook.toJson().dump(), "application/json");
	} );
	
	svr.Post( "/order", [&orderbook](const httplib::Request& req, httplib::Response& res) {

		json body = json::parse(req.body);

		int 			id 	 = body["id"].get<int>();
		OrderBook::OrderType 	type 	 = body["type"].get<OrderBook::OrderType>();
		OrderBook::Side 	side 	 = body["side"].get<OrderBook::Side>();
		double 			price 	 = body["price"].get<double>();
		int 			quantity = body["quantity"].get<int>();

		OrderBook::Order new_order(id, type, side, price, quantity);
		orderbook.placeOrder(new_order);

		res.set_content("{\"ok\":true}", "application/json");
	} );

	//...


	std::cout << "Listening on port 8080..." << std::endl;
	svr.listen("0.0.0.0", 8080);
}
