#include "exchanges/binance.h"
#include "exchanges/bithumb.h"
#include "arbitrage.h"

#include <iomanip>
#include <map>
#include <sstream>
#include <chrono>
#include <thread>

int main() {
  std::string log_filename = std::string("output/bluebird_log_") + "xx" + ".log";
  std::ofstream log_file(log_filename, std::ofstream::trunc);
  log_file.precision(2);
  log_file << std::fixed;

  Binance binance(log_file);
  Bithumb bithumb(log_file);
  Arbitrage<Binance, Bithumb> arbitrage(log_file);

  std::cout << std::endl;

  while(1) {
    std::cout << "\033[2J\033[1;1H";
    std::map<double, std::string, std::greater<double>> output;
    auto table = arbitrage.get_profit_table();
    for(int i=0; i<bithumb.symbols.size(); i++) {
      for(int j=0; j<bithumb.symbols.size(); j++) {
        double profit_percentage = table[i][j];
        if(profit_percentage > 0 && profit_percentage < 10000) {
          std::stringstream ss;
          ss << std::setw(14) << bithumb.symbols[i] + " -> " + bithumb.symbols[j] + " :";
          ss << std::setw(8) <<"\033[32m" << std::to_string(profit_percentage)  << "\033[0m" << std::endl;
          output[profit_percentage] = ss.str();
          //std::cout << " -------------------------------------- " << std::endl;
        }
      }
    }
    for (auto const& x : output)
      std::cout << x.second;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  log_file.close();
}
