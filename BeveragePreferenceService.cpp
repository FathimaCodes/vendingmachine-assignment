#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <signal.h>

#include "json.hpp"

#include "../utils.h"
#include "BeveragePreferenceHandler.h"

using json = nlohmann::json;
using apache::thrift::server::TThreadedServer;
using apache::thrift::transport::TServerSocket;
using apache::thrift::transport::TFramedTransportFactory;
using apache::thrift::protocol::TBinaryProtocolFactory;

using namespace vending_machine;

// Signal handler
void sigintHandler(int sig) {
    exit(EXIT_SUCCESS);
}

// Entry point
int main(int argc, char **argv) {
  // 1: handle Ctrl+C
  signal(SIGINT, sigintHandler);

  // 2: init logger
  init_logger();

  // 3: load config
  json config_json;
  if (load_config_file("config/service-config.json", &config_json) != 0) {
    exit(EXIT_FAILURE);
  }

  // 4: get this service's port
  int my_port = config_json["beverage-preference-service"]["port"];

  // 5: set up and start Thrift server
  TThreadedServer server(
      std::make_shared<BeveragePreferenceServiceProcessor>(
          std::make_shared<BeveragePreferenceServiceHandler>()),
      std::make_shared<TServerSocket>("0.0.0.0", my_port),
      std::make_shared<TFramedTransportFactory>(),
      std::make_shared<TBinaryProtocolFactory>());

  std::cout << "Starting the beverage-preference server ..." << std::endl;
  server.serve();
  return 0;
}

