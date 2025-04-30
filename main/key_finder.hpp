#pragma once

#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include <cstdint> // Required for uint8_t
#include <vector>

class KeyFinder {
public:
  // Constructor: Takes a vector of GPIO pins to test
  KeyFinder(const std::vector<gpio_num_t> &gpios);

  // Destructor (optional, could handle GPIO cleanup if needed)
  // ~KeyFinder();

  // Initializes the GPIO pins for testing
  bool initialize_gpios();

  // Performs one full scan cycle, detects changes, and logs them
  void scan_and_log();

  // Main loop to continuously scan and log
  void run_scan_loop(uint32_t delay_ms = 20);

private:
  const char *TAG = "KeyFinder";
  std::vector<gpio_num_t> gpios_to_test_;
  size_t num_gpios_;
  // State tracking: press_state_[driven_idx][read_idx] = 1 if pressed
  std::vector<std::vector<uint8_t>> press_state_;

  // Helper to configure a single GPIO temporarily as output low
  void set_pin_output_low(gpio_num_t gpio);

  // Helper to restore a single GPIO to input pullup
  void restore_pin_input_pullup(gpio_num_t gpio);
};
