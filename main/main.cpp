#include "NimBLEDevice.h"
#include "NimBLEHIDDevice.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "keyboard.hpp"
#include "keyboard_button.h"
#include <vector>

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
} serverCallbacks;
;

extern "C" void app_main(void) {
  // Code below this line in app_main will not be reached
  static std::vector<int> output_gpios = {0, 1, 2, 3, 4, 5, 6};
  static std::vector<int> input_gpios = {10, 11, 12, 13};
  static Keyboard keyboard(output_gpios, input_gpios);
  // Code below this line in app_main will not be reached
  ESP_LOGI("main", "Starting esp32-corne-v3");
  NimBLEDevice::init("ESP32-CORNE-V3");
  NimBLEDevice::setSecurityAuth(
      /*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/
      BLE_SM_PAIR_AUTHREQ_SC);
  NimBLEDevice::setSecurityRespKey(0000);

  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(&serverCallbacks);
  NimBLEService *pService = pServer->createService("test-service");
  NimBLECharacteristic *pCharacteristic =
      pService->createCharacteristic("1234");

  // --- Advertising ---

  NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID("test-service");
  pAdvertising->setName("Luca-Keyboard");
  pAdvertising->setAdvertisingCompleteCallback(
      [](auto adv) { ESP_LOGI("BLE", "Advertising complete"); });
  pAdvertising->start();

  // --- HID ---
  // NimBLEHIDDevice hid = NimBLEHIDDevice(pServer);
}
