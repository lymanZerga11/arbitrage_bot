#ifndef ARBITRAGE_H
#define ARBITRAGE_H

#include <vector>

typedef std::vector<std::vector<double>> profit_table;

template <class MarketA, class MarketB>
class Arbitrage {
private:
  MarketA market_a;
  MarketB market_b;
  profit_table profit;
  void calculate_profits();

public:
  Arbitrage                               (std::ofstream& _log_file);
  Arbitrage                               (const Arbitrage &) =delete;
  Arbitrage& operator=                    (const Arbitrage &) =delete;
  const profit_table& get_profit_table    ();

};

#endif
