#ifndef VENDING_MACHINE_MICROSERVICES_BEVERAGEPREFERENCESERVICEHANDLER_H
#define VENDING_MACHINE_MICROSERVICES_BEVERAGEPREFERENCESERVICEHANDLER_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "../../gen-cpp/BeveragePreferenceService.h"
#include "../logger.h"

namespace vending_machine {

class BeveragePreferenceServiceHandler : public BeveragePreferenceServiceIf {
 public:
  BeveragePreferenceServiceHandler() {
    // Seed the random number generator
    srand(static_cast<unsigned int>(time(nullptr)));
  }

  ~BeveragePreferenceServiceHandler() override = default;

  void getBeverage(std::string& _return, const BeverageType::type btype) override {
    std::vector<std::string> hot_beverages = {"Espresso", "Latte", "Cappuccino"};
    std::vector<std::string> cold_beverages = {"Lemonade", "Iced Tea", "Cold Brew"};

    if (btype == BeverageType::type::HOT) {
      _return = hot_beverages[rand() % hot_beverages.size()];
    } else if (btype == BeverageType::type::COLD) {
      _return = cold_beverages[rand() % cold_beverages.size()];
    } else {
      _return = "Unknown beverage type";
      LOG(error) << "Invalid BeverageType: " << btype;
    }

    LOG(info) << "Selected beverage: " << _return;
  }
};

}  // namespace vending_machine

#endif // VENDING_MACHINE_MICROSERVICES_BEVERAGEPREFERENCESERVICEHANDLER_H

