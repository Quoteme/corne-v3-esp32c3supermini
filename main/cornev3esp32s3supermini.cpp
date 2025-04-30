#include "esp_log.h"
#include "hal/gpio_types.h"
#include "key_finder.hpp" // Include our new class header
#include <vector>

// *** TODO: IMPORTANT! Define the ESP32 GPIO pins you connected here ***
const std::vector<gpio_num_t> connected_gpios = {
    GPIO_NUM_1,  GPIO_NUM_2,  GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5,
    GPIO_NUM_6,  GPIO_NUM_7,  GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10,
    GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_13
    // Add or remove pins according to your actual wiring
};

extern "C" void app_main(void) {
  ESP_LOGI("main", "Starting C++ Key Finder Application");

  // Create an instance of the KeyFinder class
  KeyFinder keyFinder(connected_gpios);

  // Run the continuous scan loop
  // This function contains the infinite loop and won't return.
  keyFinder.run_scan_loop();

  // Code below this line in app_main will not be reached
  ESP_LOGI("main", "Scan loop finished (should not happen)");
}
