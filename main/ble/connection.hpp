#include "NimBLEDevice.h"
#include "NimBLEHIDDevice.h"
#include "hid/page/generic_desktop.hpp"
#include "hid/rdf/descriptor.hpp"

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override {
    printf("Client address: %s\n", connInfo.getAddress().toString().c_str());
    pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 180);
  }

  void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo,
                    int reason) override {
    printf("Client disconnected - start advertising\n");
    NimBLEDevice::startAdvertising();
  }

  void onAuthenticationComplete(NimBLEConnInfo &connInfo) override {
    if (!connInfo.isEncrypted()) {
      NimBLEDevice::getServer()->disconnect(connInfo.getConnHandle());
      printf("Encrypt connection failed - disconnecting client\n");
      return;
    }
    printf("Secured connection to: %s\n",
           connInfo.getAddress().toString().c_str());
  }

} serverCallbacks;

using namespace hid::rdf;
using namespace hid::page;

class Connection {
public:
  Connection(std::string name = "Luca-Keyboard") { this->name = name; }

  void start() {
    // --- Init ---
    NimBLEDevice::init(name);
    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEServer *pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(&serverCallbacks);

    // --- HID ---
    NimBLEHIDDevice *pHID = new NimBLEHIDDevice(pServer);
    pHID->setManufacturer("Luca Leon Happel");
    // pHID->setReportMap()

    // --- Advertising ---
    NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID("test-service");
    pAdvertising->setName(name);
    pAdvertising->setAdvertisingCompleteCallback(
        [](auto adv) { ESP_LOGI("BLE", "Advertising complete"); });
    pAdvertising->start();
  }

private:
  std::string name;

  // clang-format off
  static constexpr auto hid_report = descriptor(
    usage_page<generic_desktop>(),
    usage(generic_desktop::KEYBOARD),
    collection::application(
      usage(generic_desktop::KEYBOARD)
      // collection::physical(
      //
      // )
    )
  );
  // clang-format on
};
