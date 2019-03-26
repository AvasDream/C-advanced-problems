#include "controller/MyController.hpp"
#include "oatpp/core/base/Environment.hpp"
#include "oatpp/network/server/Server.hpp"
#include <iostream>
#include "Logger.hpp"
#include "AppComponent.hpp"

void run() {

  // Register AppComponents in the Environment
  // Components will be unregistered once method run() return
  AppComponent components;

  auto router = components.httpRouter.getObject();

  //create controller
  auto myController = MyController::createShared();

  // add all endpoints of controller to router
  myController->addEndpointsToRouter(router);

  // create server which passes connections retrieved from ConnectionProvider to ConnectionHandler
  oatpp::network::server::Server server(components.serverConnectionProvider.getObject(),
                                        components.serverConnectionHandler.getObject());

  OATPP_LOGD("Server", "Running on port %s...", components.serverConnectionProvider.getObject()->getProperty("port").toString()->c_str());

  // run server
  server.run();
}

int main(int argc, const char * argv[]) {

  // Init Environment. Init object counters, and make basic sanity checks
  oatpp::base::Environment::init();
  oatpp::base::Environment::setLogger(new Logger());
  run();
  oatpp::base::Environment::setLogger(nullptr);
  // Output how many objects were created during application run and how many objects may have been leaked
  std::cout << "\nEnvironment:\n";
  std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
  std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";

  oatpp::base::Environment::destroy();

  return 0;
}
