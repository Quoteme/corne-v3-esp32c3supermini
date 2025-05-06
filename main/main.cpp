#include "esp_log.h"
#include "hal/gpio_types.h"
#include <vector>

extern "C" void app_main(void) {
  // Code below this line in app_main will not be reached
  ESP_LOGI("main", "Scan loop finished (should not happen)");
}
