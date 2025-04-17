#ifndef VENDING_MACHINE_MICROSERVICES_ORDERBEVERAGEHANDLER_H
#define VENDING_MACHINE_MICROSERVICES_ORDERBEVERAGEHANDLER_H

#include <iostream>
#include <string>
#include <regex>
#include <future>

#include "../../gen-cpp/OrderBeverageService.h"
#include "../../gen-cpp/WeatherService.h"
#include "../../gen-cpp/BeveragePreferenceService.h" // ✅ NEW

#include "../ClientPool.h"
#include "../ThriftClient.h"
#include "../logger.h"

namespace vending_machine {

class OrderBeverageServiceHandler : public OrderBeverageServiceIf {
 public:
  // ✅ UPDATED constructor
  OrderBeverageServiceHandler(
    ClientPool<ThriftClient<WeatherServiceClient>>*,
    ClientPool<ThriftClient<BeveragePreferenceServiceClient>>*
  );

  ~OrderBeverageServiceHandler() override = default;

  void PlaceOrder(std::string& _return, const int64_t city) override;

 private:
  ClientPool<ThriftClient<WeatherServiceClient>>* _weather_client_pool;
  ClientPool<ThriftClient<BeveragePreferenceServiceClient>>* _beverage_client_pool; // ✅ NEW
};

// ✅ Updated constructor
OrderBeverageServiceHandler::OrderBeverageServiceHandler(
    ClientPool<ThriftClient<WeatherServiceClient>>* weather_client_pool,
    ClientPool<ThriftClient<BeveragePreferenceServiceClient>>* beverage_client_pool
) {
  _weather_client_pool = weather_client_pool;
  _beverage_client_pool = beverage_client_pool;
}

// ✅ Updated PlaceOrder() implementation
void OrderBeverageServiceHandler::PlaceOrder(std::string& _return, const int64_t city) {
  printf("PlaceOrder\n");

  // 1. Get weather info
  auto weather_client_wrapper = _weather_client_pool->Pop();
  if (!weather_client_wrapper) {
    ServiceException se;
    se.errorCode = ErrorCode::SE_THRIFT_CONN_ERROR;
    se.message = "Failed to connect to weather-service";
    throw se;
  }

  auto weather_client = weather_client_wrapper->GetClient();
  WeatherType::type weatherType = WeatherType::type::COLD;

  try {
    weatherType = weather_client->GetWeather(city);
  } catch (...) {
    _weather_client_pool->Push(weather_client_wrapper);
    LOG(error) << "Failed to send call GetWeather to weather-client";
    throw;
  }
  _weather_client_pool->Push(weather_client_wrapper);

  // 2. Decide BeverageType
  BeverageType::type btype = (weatherType == WeatherType::type::WARM) ?
                              BeverageType::type::COLD : BeverageType::type::HOT;

  // 3. Call BeveragePreferenceService
  auto beverage_client_wrapper = _beverage_client_pool->Pop();
  if (!beverage_client_wrapper) {
    ServiceException se;
    se.errorCode = ErrorCode::SE_THRIFT_CONN_ERROR;
    se.message = "Failed to connect to beverage-preference-service";
    throw se;
  }

  auto beverage_client = beverage_client_wrapper->GetClient();
  std::string beverage_name;

  try {
    beverage_client->GetBeverage(beverage_name, btype);
  } catch (...) {
    _beverage_client_pool->Push(beverage_client_wrapper);
    LOG(error) << "Failed to call GetBeverage on beverage-preference-service";
    throw;
  }
  _beverage_client_pool->Push(beverage_client_wrapper);

  // 4. Return to nginx
  _return = beverage_name;
}

} // namespace vending_machine

#endif // VENDING_MACHINE_MICROSERVICES_ORDERBEVERAGEHANDLER_H

