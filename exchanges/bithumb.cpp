#include "../utils/restapi.h"
#include "../utils/unique_json.hpp"
#include "bithumb.h"
#include "../symbols.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <array>
#include <cmath>
#include <ctime>

std::vector<std::string> BITHUMB_SYMBOLS = {
  "BTC",
  "XRP",
  "LTC",
  "BCH",
  "XMR",
  "EOS",
  "QTUM",
  "ETC",
  "ZCASH",
  "BCG",
  "DASH",
  "ETH"
};

std::unordered_map<std::string, int> BITHUMB_SYMBOL_MAP = {
  {"BTC", 0},
  {"XRP", 1},
  {"LTC", 2},
  {"BCH", 3},
  {"XMR", 4},
  {"EOS", 5},
  {"QTUM",6},
  {"ETC", 7},
  {"ZCASH", 8},
  {"BCG", 9},
  {"DASH", 10},
  {"ETH", 11},
};

RestApi& Bithumb::query_handle(std::ofstream& log_file) {
  static RestApi query("https://api.bithumb.com", nullptr, log_file);
  return query;
}

Bithumb::Bithumb (std::ofstream& _log_file) : log_file(_log_file){
  symbols = BITHUMB_SYMBOLS;
  symbol_map = BITHUMB_SYMBOL_MAP;
  for(int i=0; i<BITHUMB_SYMBOLS.size(); i++)
    price.push_back(quote_t(std::make_pair(0, 0)));
  update_quotes();
}

double Bithumb::get_time() const {
  return 0;
}

void Bithumb::update_quotes() {
  auto &exchange = query_handle(log_file);

  std::string url;
  url = "/public/ticker/all";
  unique_json root { exchange.get_request(url) };
  int quote_array_size = BITHUMB_SYMBOLS.size();
  for(int i=0; i<quote_array_size; i++) {
    json_t *json_object = json_object_get(json_object_get(root.get(), "data"), BITHUMB_SYMBOLS[i].c_str());

    std::string symbol = BITHUMB_SYMBOLS[i];
    if(BITHUMB_SYMBOL_MAP.find(symbol) == BITHUMB_SYMBOL_MAP.end()) continue;

    const char *quote = json_string_value(json_object_get(json_object, "buy_price"));
    double symbol_bid_price = quote ? std::stod(quote) : 0.0;

    quote = json_string_value(json_object_get(json_object, "sell_price"));
    double symbol_ask_price = quote ? std::stod(quote) : 0.0;

    price[BITHUMB_SYMBOL_MAP[symbol]] = std::make_pair(symbol_bid_price, symbol_ask_price);
  }
}

const quote_t& Bithumb::get_quote(std::string symbol) const {
  return price[BITHUMB_SYMBOL_MAP[symbol]];
}

const quote_array_t& Bithumb::get_quote_array() const {
  return price;
}
