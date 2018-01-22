#include "exchanges/binance.h"
#include "exchanges/bithumb.h"
#include "exchanges/coinone.h"
#include "arbitrage.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

template <class MarketA, class MarketB>
void Arbitrage<MarketA, MarketB>::calculate_profits() {
  // std::cout << std::ios::fixed << std::setprecision(9);
  market_a.update_quotes();
  market_b.update_quotes();
  const quote_array_t& market_a_quotes = market_a.get_quote_array();
  const quote_array_t& market_b_quotes = market_b.get_quote_array();
  double starting_eth=1.0, xrp, krw, qtum, ending_eth, profit_percentage;
  for(int i=0; i<market_a_index.size(); i++) {
    for(int j=0; j<market_a_index.size(); j++) {
      xrp = starting_eth / market_a_quotes[market_a_index[i]].ask();
      krw = xrp * market_b_quotes[market_b_index[i]].bid();
      qtum = krw / market_b_quotes[market_b_index[j]].ask();
      ending_eth = qtum * market_a_quotes[market_a_index[j]].bid();
      profit_percentage = 100.0 * (ending_eth - starting_eth) / starting_eth;
      profit[i][j] = profit_percentage;

      // std::cout << "BUY " << xrp << " OF " << market_a.symbols[market_a_index[i]] << " AT " << market_a_quotes[market_a_index[i]].ask() << std::endl;
      // std::cout << "SELL " << " FOR " << krw << " AT " << market_b_quotes[market_b_index[i]].bid() << std::endl;
      // std::cout << "BUY " << qtum << " OF " << market_b.symbols[market_b_index[j]] << " AT " << market_b_quotes[market_b_index[j]].ask() << std::endl;
      // std::cout << "SELL " << " FOR " << ending_eth << " AT " << market_a_quotes[market_a_index[j]].bid() << std::endl;
      // std::cout << "------------------------------------------------" << std::endl;
      // std::cout << std::endl;

    }
  }
}

template <class MarketA, class MarketB>
Arbitrage<MarketA, MarketB>::Arbitrage(std::ofstream& log_file) :
                          market_a(log_file), market_b(log_file) {
  for(int i=0; i<20; i++)
    profit.push_back(std::vector<double>(20, -1));
  generate_correspondence_table();
  // for(int i=0; i<market_a_index.size(); i++)
  //   std::cout << market_a_index[i] << market_b_index[i] << std::endl;
  std::cout << "Done Arbitrage Initialization.." << std::endl;
}

template <class MarketA, class MarketB>
const profit_table& Arbitrage<MarketA, MarketB>::get_profit_table () {
  calculate_profits();
  return profit;
}

template <class MarketA, class MarketB>
void Arbitrage<MarketA, MarketB>::generate_correspondence_table () {
  for(int i=0; i<market_a.symbols.size(); i++) {
    std::string symbol = market_a.symbols[i];
    auto it =
      std::find_if (market_a.universal_symbol_correspondence_map.begin(), market_a.universal_symbol_correspondence_map.end(),
        [symbol](const std::pair<std::string, std::string> & t) -> bool {
          return t.second == symbol;
        }
      );
    std::string universal_symbol = it->first;
    if(market_b.universal_symbol_correspondence_map.find(universal_symbol) != market_b.universal_symbol_correspondence_map.end()) {
      universal_symbols.push_back(universal_symbol);

      std::string market_a_symbol = market_a.universal_symbol_correspondence_map[universal_symbol];
      market_a_index.push_back(market_a.symbol_map[market_a_symbol]);

      std::string market_b_symbol = market_b.universal_symbol_correspondence_map[universal_symbol];
      market_b_index.push_back(market_b.symbol_map[market_b_symbol]);
    }
  }
}

template class Arbitrage<Binance, Bithumb>;
template class Arbitrage<Binance, Coinone>;
