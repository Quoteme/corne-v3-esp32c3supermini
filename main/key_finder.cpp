#include "key_finder.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "esp_timer.h" // For ets_delay_us if needed, often okay without

KeyFinder::KeyFinder(const std::vector<gpio_num_t> &gpios)
    : gpios_to_test_(gpios), num_gpios_(gpios.size()) {
  // Resize the state matrix based on the number of GPIOs
  press_state_.resize(num_gpios_, std::vector<uint8_t>(num_gpios_, 0));
  ESP_LOGI(TAG, "KeyFinder created for %d GPIOs.", num_gpios_);
  for (size_t i = 0; i < num_gpios_; ++i) {
    printf(" %d", gpios_to_test_[i]);
  }
  printf("\n");
}

bool KeyFinder::initialize_gpios() {
  ESP_LOGI(TAG, "Initializing GPIOs...");
  uint64_t pin_mask = 0;
  for (const auto &gpio : gpios_to_test_) {
    if (gpio < GPIO_NUM_MAX) {
      pin_mask |= (1ULL << gpio);
    } else {
      ESP_LOGE(TAG, "Invalid GPIO number configured: %d", gpio);
      return false; // Indicate failure
    }
  }

  gpio_config_t io_conf = {.pin_bit_mask = pin_mask,
                           .mode = GPIO_MODE_INPUT, // Start all as INPUT
                           .pull_up_en = GPIO_PULLUP_ENABLE, // Enable PULLUP
                           .pull_down_en = GPIO_PULLDOWN_DISABLE,
                           .intr_type = GPIO_INTR_DISABLE};

  esp_err_t err = gpio_config(&io_conf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "GPIO config error: %s", esp_err_to_name(err));
    return false; // Indicate failure
  }

  ESP_LOGI(TAG, "GPIOs initialized successfully with mask: %llx", pin_mask);
  ESP_LOGI(TAG, "Ready to scan. Press keys one by one.");
  ESP_LOGI(TAG, "Output format: Driven GPIO -> Read GPIO");
  return true; // Indicate success
}

void KeyFinder::set_pin_output_low(gpio_num_t gpio) {
  gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
  gpio_set_level(gpio, 0);
}

void KeyFinder::restore_pin_input_pullup(gpio_num_t gpio) {
  // Set HIGH first before changing direction to avoid potential brief shorts
  // if another pin is driving LOW during the transition.
  // gpio_set_level(gpio, 1); // Often not strictly necessary but safer
  gpio_set_direction(gpio, GPIO_MODE_INPUT);
  // Re-enable pull-up just in case direction change affected it
  gpio_set_pull_mode(gpio, GPIO_PULLUP_ONLY);
}

void KeyFinder::scan_and_log() {
  for (size_t driven_idx = 0; driven_idx < num_gpios_; ++driven_idx) {
    gpio_num_t driven_gpio = gpios_to_test_[driven_idx];

    // 1. Set the driven pin to OUTPUT LOW
    set_pin_output_low(driven_gpio);

    // Optional delay - uncomment if needed
    // ets_delay_us(5);

    // 2. Read all other pins
    for (size_t read_idx = 0; read_idx < num_gpios_; ++read_idx) {
      if (driven_idx == read_idx)
        continue; // Don't read the pin we're driving

      gpio_num_t read_gpio = gpios_to_test_[read_idx];
      int current_level = gpio_get_level(read_gpio);

      // 3. Check state changes and log
      if (current_level == 0 && press_state_[driven_idx][read_idx] == 0) {
        // driven_gpio:    5,4,3,2
        // read_gpio:      10,11,12,13 (brauche wahrschienlich noch 14,15!)
        // momentan frei: 1, 6,7,8,9,
        ESP_LOGI(TAG, "Key PRESSED: %d -> %d", driven_gpio, read_gpio);
        press_state_[driven_idx][read_idx] = 1; // Mark as pressed
      } else if (current_level == 1 &&
                 press_state_[driven_idx][read_idx] == 1) {
        // Use Warning level for release to differentiate easily in logs
        ESP_LOGW(TAG, "Key RELEASED: %d -> %d", driven_gpio, read_gpio);
        press_state_[driven_idx][read_idx] = 0; // Mark as released
      }
    }

    // 4. Restore the driven pin to INPUT PULLUP
    restore_pin_input_pullup(driven_gpio);

  } // End driven_idx loop
}

void KeyFinder::run_scan_loop(uint32_t delay_ms) {
  if (!initialize_gpios()) {
    ESP_LOGE(TAG, "Failed to initialize GPIOs. Halting scan loop.");
    return; // Or handle error differently
  }

  while (1) {
    scan_and_log();
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
  }
}
