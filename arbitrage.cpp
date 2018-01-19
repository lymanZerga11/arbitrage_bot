#include "exchanges/binance.h"
#include "exchanges/bithumb.h"
#include "arbitrage.h"
#include <vector>
#include <iostream>
#include <iomanip>

template <class MarketA, class MarketB>
void Arbitrage<MarketA, MarketB>::calculate_profits() {
  // std::cout << std::ios::fixed << std::setprecision(9);
  market_a.update_quotes();
  market_b.update_quotes();
  const quote_array_t& market_a_quotes = market_a.get_quote_array();
  const quote_array_t& market_b_quotes = market_b.get_quote_array();
  double starting_eth=1.0, xrp, krw, qtum, ending_eth, profit_percentage;
  for(int i=0; i<market_a.symbols.size(); i++) {
    for(int j=0; j<market_a.symbols.size(); j++) {
      xrp = starting_eth / market_a_quotes[i].ask();
      krw = xrp * market_b_quotes[i].bid();
      qtum = krw / market_b_quotes[j].ask();
      ending_eth = qtum * market_a_quotes[j].bid();
      profit_percentage = 100.0 * (ending_eth - starting_eth) / starting_eth;
      profit[i][j] = profit_percentage;
    }
  }
}

template <class MarketA, class MarketB>
Arbitrage<MarketA, MarketB>::Arbitrage(std::ofstream& log_file) :
                          market_a(log_file), market_b(log_file) {
  for(int i=0; i<20; i++)
    profit.push_back(std::vector<double>(20, -1));
  std::cout << "Done Arbitrage Initialization.." << std::endl;
}

template <class MarketA, class MarketB>
const profit_table& Arbitrage<MarketA, MarketB>::get_profit_table () {
  calculate_profits();
  return profit;
}

template class Arbitrage<Binance, Bithumb>;
