#include "../utils/restapi.h"
#include "../utils/unique_json.hpp"
#include "coinone.h"
#include "../symbols.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <array>
#include <cmath>
#include <ctime>


std::vector<std::string> COINONE_SYMBOLS = {
  "BTC",
  "BCH",
  "ETH",
  "ETC",
  "XRP",
  "QTUM",
  "IOTA",
  "LTC",
  "BTG",
};

std::unordered_map<std::string, int> COINONE_SYMBOL_MAP = {
  {"BTC", 0},
  {"BCH", 1},
  {"ETH", 2},
  {"ETC", 3},
  {"XRP", 4},
  {"QTUM", 5},
  {"IOTA", 6},
  {"LTC", 7},
  {"BTG", 8},
};

std::unordered_map<std::string, std::string>  COINONE_UNIVERSAL_SYMBOL_CORRESPONDENCE_MAP = {
  {"BTC", "BTC"},
  {"BCH", "BCH"},
  {"ETH", "ETH"},
  {"ETC", "ETC"},
  {"XRP", "XRP"},
  {"QTUM", "QTUM"},
  {"IOTA", "IOTA"},
  {"LTC", "LTC"},
  {"BTG", "BTG"},
};

RestApi& Coinone::query_handle(std::ofstream& log_file) {
  static RestApi query("https://api.coinone.co.kr", nullptr, log_file);
  return query;
}

Coinone::Coinone (std::ofstream& _log_file) : log_file(_log_file){
  symbols = COINONE_SYMBOLS;
  symbol_map = COINONE_SYMBOL_MAP;
  universal_symbol_correspondence_map = COINONE_UNIVERSAL_SYMBOL_CORRESPONDENCE_MAP;
  for(int i=0; i<COINONE_SYMBOLS.size(); i++)
    price.push_back(quote_t(std::make_pair(0, 0)));
  update_quotes();
}

double Coinone::get_time() const {
  return 0;
}

void Coinone::update_quotes() {
  auto &exchange = query_handle(log_file);

  std::string url, base_url;
  base_url = "/orderbook";
  for(int i=0; i<symbols.size(); i++) {
    std::string symbol = COINONE_SYMBOLS[i];
    url = base_url + "?currency=" + symbol;
    unique_json root { exchange.get_request(url) };

    json_t *json_object = json_array_get(json_object_get(root.get(), "bid"), 0);
    const char *quote = json_string_value(json_object_get(json_object, "price"));
    double symbol_bid_price = quote ? std::stod(quote) : 0.0;

    json_object = json_array_get(json_object_get(root.get(), "ask"), 0);
    quote = json_string_value(json_object_get(json_object, "price"));
    double symbol_ask_price = quote ? std::stod(quote) : 0.0;

    // std::cout << symbol << " " << symbol_bid_price << " : " << symbol_ask_price << std::endl;
    price[COINONE_SYMBOL_MAP[symbol]] = std::make_pair(symbol_bid_price, symbol_ask_price);
  }
}

const quote_t& Coinone::get_quote(std::string symbol) const {
  return price[COINONE_SYMBOL_MAP[symbol]];
}

const quote_array_t& Coinone::get_quote_array() const {
  return price;
}
