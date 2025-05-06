#include "BleServer.hpp"
#include "HidDevice.hpp"
#include "NimBLEDevice.h"
#include "esp_log.h"

void BleServer::init() {
  NimBLEDevice::init("ESP32 BLE Keyboard+Mouse");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9); // max power
  NimBLEDevice::setSecurityAuth(false, false, false);

  auto *server = NimBLEDevice::createServer();
  auto *hidDevice = new HidDevice(server);
  hidDevice->init();

  startAdvertising();
}

void BleServer::startAdvertising() {
  auto *adv = NimBLEDevice::getAdvertising();
  adv->setAppearance(HID_GENERIC);
  adv->addServiceUUID("1812"); // HID
  adv->start();
}
