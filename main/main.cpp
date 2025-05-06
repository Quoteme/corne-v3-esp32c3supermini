#include "NimBLEDevice.h"
#include "esp_log.h"
#include "keyboard.hpp"
#include "keyboard_button.h"
#include <vector>
#include "hal/gpio_types.h"
#include <vector>

extern "C" void app_main(void) {
  // Code below this line in app_main will not be reached
  ESP_LOGI("main", "Scan loop finished (should not happen)");
  static std::vector<int> output_gpios = {0, 1, 2, 3, 4, 5, 6};
  static std::vector<int> input_gpios = {10, 11, 12, 13};
  static Keyboard keyboard(output_gpios, input_gpios);
  // Code below this line in app_main will not be reached
  ESP_LOGI("main", "Scan loop finished (should not happen)");
  ESP_LOGI("main", "Starting esp32-corne-v3");
  NimBLEDevice::init("ESP32-CORNE-V3");
  NimBLEServer *pServer = NimBLEDevice::createServer();
  NimBLEService *pService = pServer->createService("test-service");
  NimBLECharacteristic *pCharacteristic =
      pService->createCharacteristic("1234");

  // --- Advertising ---

  NimBLEAdvertising *pAdvertising =
      NimBLEDevice::getAdvertising(); // create advertising instance
  pAdvertising->addServiceUUID(
      "test-service");                    // advertise the UUID of our service
  pAdvertising->setName("Luca-Keyboard"); // advertise the device name
  pAdvertising->start();                  // start advertising
}
