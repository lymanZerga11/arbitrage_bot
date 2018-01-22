#include "../utils/restapi.h"
#include "../utils/unique_json.hpp"
#include "binance.h"
#include "../symbols.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <array>
#include <cmath>
#include <ctime>


std::vector<std::string> BINANCE_SYMBOLS = {
  "BTCETH",
  "XRPETH",
  "LTCETH",
  "BCCETH",
  "XMRETH",
  "EOSETH",
  "QTUMETH",
  "ETCETH",
  "ZECETH",
  "BTGETH",
  "DASHETH",
  "USDTETH",
  "IOTAETH"
};

std::unordered_map<std::string, int> BINANCE_SYMBOL_MAP = {
  {"BTCETH", 0},
  {"XRPETH", 1},
  {"LTCETH", 2},
  {"BCCETH", 3},
  {"XMRETH", 4},
  {"EOSETH", 5},
  {"QTUMETH",6},
  {"ETCETH", 7},
  {"ZECETH", 8},
  {"BTGETH", 9},
  {"DASHETH", 10},
  {"USDTETH", 11},
  {"IOTAETH", 12}
};

std::unordered_map<std::string, std::string> BINANCE_UNIVERSAL_SYMBOL_CORRESPONDENCE_MAP = {
  {"BTC", "BTCETH"},
  {"XRP", "XRPETH"},
  {"LTC", "LTCETH"},
  {"BCH", "BCCETH"},
  {"XMR", "XMRETH"},
  {"EOS", "EOSETH"},
  {"QTUM", "QTUMETH"},
  {"ETC", "ETCETH"},
  {"ZEC", "ZECETH"},
  {"BTG", "BTGETH"},
  {"DASH", "DASHETH"},
  {"USDT", "USDTETH"},
  {"IOTA", "IOTAETH"}
};

RestApi& Binance::query_handle(std::ofstream& log_file) {
  static RestApi query("https://www.binance.com", nullptr, log_file);
  return query;
}

Binance::Binance (std::ofstream& _log_file) : log_file(_log_file){
  symbols = BINANCE_SYMBOLS;
  symbol_map = BINANCE_SYMBOL_MAP;
  universal_symbol_correspondence_map = BINANCE_UNIVERSAL_SYMBOL_CORRESPONDENCE_MAP;
  for(int i=0; i<BINANCE_SYMBOLS.size(); i++)
    price.push_back(quote_t(std::make_pair(0, 0)));
  update_quotes();
}

double Binance::get_time() const {
  auto &exchange = query_handle(log_file);

  std::string url;
  url = "/api/v1/time";
  unique_json root { exchange.get_request(url) };
  double cur_time = json_integer_value(json_object_get(root.get(), "serverTime"));

  return cur_time;
}

void Binance::update_quotes() {
  auto &exchange = query_handle(log_file);

  std::string url;
  url = "/api/v3/ticker/bookTicker";
  unique_json root { exchange.get_request(url) };
  int quote_array_size = json_array_size(root.get());
  for(int i=0; i<quote_array_size; i++) {
    json_t *json_object = json_array_get(root.get(), i);

    std::string symbol = json_string_value(json_object_get(json_object, "symbol"));
    if(BINANCE_SYMBOL_MAP.find(symbol) == BINANCE_SYMBOL_MAP.end()) continue;

    const char *quote = json_string_value(json_object_get(json_object, "bidPrice"));
    double symbol_bid_price = quote ? std::stod(quote) : 0.0;

    quote = json_string_value(json_object_get(json_object, "askPrice"));
    double symbol_ask_price = quote ? std::stod(quote) : 0.0;

    price[BINANCE_SYMBOL_MAP[symbol]] = std::make_pair(symbol_bid_price, symbol_ask_price);
  }
}

const quote_t& Binance::get_quote(std::string symbol) const {
  return price[BINANCE_SYMBOL_MAP[symbol]];
}

const quote_array_t& Binance::get_quote_array() const {
  return price;
}
