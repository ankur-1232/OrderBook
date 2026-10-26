#include "httplib.h"
#include "nlohmann/json.hpp"
#include <bits/stdc++.h>

using json = nlohmann::json;

struct Level {
    double price;
    double size;
};

// struct Candle {};

std::vector<Level> bids;
std::vector<Level> asks;
// std::vector<Candle> candles;

void insertBid(Level l) {
    bids.push_back(l);
    std::sort( bids.begin(), bids.end(), [](const Level& a, const Level& b) { return a.price > b.price; } );
}

void insertAsk(Level l) {
    asks.push_back(l);
    std::sort( asks.begin(), asks.end(), [](const Level& a, const Level& b) { return a.price < b.price; }  );
}

// void matchBook() {}

std::string jsonLevels(const std::vector<Level>& levels) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& lv : levels) {
        arr.push_back({{"price", lv.price}, {"size", lv.size}});
    }
    return arr.dump();
}   
//
// std::string jsonCandles() {}

int main() {
    httplib::Server svr;

    //Static files
    svr.set_mount_point("/", "./public");

    // GET /orderbook
    svr.Get("/orderbook", [](const httplib::Request&, httplib::Response& res) {
        json resp;
        resp["bids"]  = nlohmann::json::parse(jsonLevels(bids));
        resp["asks"]  = nlohmann::json::parse(jsonLevels(asks));
        res.set_content(resp.dump(), "application/json");
    });

    svr.Post( "/order", [](const httplib::Request& req, httplib::Response& res ) {
        json body = json::parse(req.body);
        std::string side = body["side"].get<std::string>();
        double price     = body["price"].get<double>();
        double size      = body["size"].get<double>();

        if (side == "buy") 
            insertBid({price, size}) ;
        else 
            insertAsk({price, size});

        // matchBook();
        res.set_content("{\"ok\":true}", "application/json");
        
    } );

    std::cout << "Listening on port 8080..." << std::endl;
    svr.listen("0.0.0.0", 8080);
}
