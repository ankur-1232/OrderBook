#include "httplib.h"
#include "json.hpp"
#include <bits/stdc++.h>

using json = nlohmann::json;

struct Order {
	int id;
	std::string symbol;
	double price;
	double qty;
	std::string side;
};

std::vector<Order> orders;
int nextId = 1;

int main() {
	httplib::Server srv;
	srv.set_default_headers( {{"Access-Control-Allow-Origin", "*"}} );

	// Pre-populate for testing
	orders.push_back({1, "AAPL", 185.5, 100, "BUY"});
	orders.push_back( {2, "TSLA", 2450.0, 10, "SELL" } );
	nextId = 3;

	// srv.Get( "", []() {} ); 
	srv.Get( "/orders", [](const httplib::Request&, httplib::Response& res) {

		std::string out;
		for (auto& o : orders) {
			out += std::to_string(o.id) + "," + o.symbol + "," + std::to_string(o.price) + "," + std::to_string(o.qty) + "," + o.side;
			
			if (&o != &orders.back()) { out += "\n"; }
		}

		res.set_content(out, "text/plain");
	} ); 

	// I will need lohmann/json:
	// curl -o json.hpp https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
	
	srv.Post( "/orders", [](const httplib::Request& req, httplib::Response& res) {

		auto j = json::parse(req.body);

		// Order o {};
		Order o {nextId++, j["symbol"], j["price"], j["qty"], j["side"] };
		orders.push_back(o);

		json resp;
		resp["id"] = o.id;
		resp["symbol"] = o.symbol;
		resp["price"] = o.price;
		resp["qty"] = o.qty;
		resp["side"] = o.side;

		res.set_content(resp.dump(), "application/json");

	} );	



	std::cout << "Listening on :8080\n";
	srv.listen("127.0.0.1", 8080);
}


