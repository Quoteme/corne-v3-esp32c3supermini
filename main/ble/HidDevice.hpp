#pragma once

#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

class HidDevice {
public:
  // Constructor
  explicit HidDevice(const std::string &address)
      : address(address), client(nullptr), connected(false) {}

  // Destructor
  ~HidDevice() {
    if (connected) {
      disconnect();
    }
  }

  // Connect to the HID device
  void connect() {
    if (connected) {
      throw std::runtime_error("Device is already connected.");
    }

    client =
        new BLEClient(address); // Replace with your BLE client initialization
    if (!client->connect()) {
      delete client;
      client = nullptr;
      throw std::runtime_error("Failed to connect to the device.");
    }

    connected = true;
  }

  // Disconnect from the HID device
  void disconnect() {
    if (!connected) {
      return;
    }

    client->disconnect(); // Replace with your BLE client's disconnect method
    delete client;
    client = nullptr;
    connected = false;
  }

  // Send a HID report to the device
  void sendReport(const std::vector<uint8_t> &report,
                  const std::string &characteristicUUID) {
    if (!connected) {
      throw std::runtime_error("Device is not connected.");
    }

    if (!client->writeCharacteristic(characteristicUUID, report)) {
      throw std::runtime_error("Failed to send the report.");
    }
  }

  // Read a HID report from the device
  std::vector<uint8_t> readReport(const std::string &characteristicUUID) {
    if (!connected) {
      throw std::runtime_error("Device is not connected.");
    }

    std::vector<uint8_t> report;
    if (!client->readCharacteristic(characteristicUUID, report)) {
      throw std::runtime_error("Failed to read the report.");
    }

    return report;
  }

  // Check if the device is connected
  bool isConnected() const { return connected; }

private:
  std::string address;
  BLEClient *client; // Replace with your BLE client class
  bool connected;
};
