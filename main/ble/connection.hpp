#include "NimBLEDevice.h"
#include "NimBLEHIDDevice.h"

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

class Connection {
public:
  std::string name;

  Connection(std::string name = "Luca-Keyboard") {
    NimBLEDevice::init(name);
    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEServer *pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(&serverCallbacks);
    NimBLEService *pService = pServer->createService("test-service");
    NimBLECharacteristic *pCharacteristic =
        pService->createCharacteristic("1234");

    // --- Advertising ---

    NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID("test-service");
    pAdvertising->setName(name);
    pAdvertising->setAdvertisingCompleteCallback(
        [](auto adv) { ESP_LOGI("BLE", "Advertising complete"); });
    pAdvertising->start();
  }

private:
};
