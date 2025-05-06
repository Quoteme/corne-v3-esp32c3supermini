#include "esp_log.h"
#include "keyboard_button.h"
#include <vector>

static TaskHandle_t light_progress_task_handle = NULL;

static void keyboard_cb(keyboard_btn_handle_t kbd_handle,
                        keyboard_btn_report_t kbd_report, void *user_data) {
  if (eTaskGetState(light_progress_task_handle) == eSuspended) {
    vTaskResume(light_progress_task_handle);
  }
  /*!< Lighting with key pressed */
  ESP_LOGI("main", "Keys pressed: %lu", kbd_report.key_pressed_num);
  // if (kbd_report.key_change_num > 0) {
  //   ESP_LOGI("main", "Key pressed: %lu", kbd_report.key_pressed_num);
  // }
}

static void light_progress_task(void *pvParameters) {
  while (1) {
    ESP_LOGI("main", "light_progress_task running...");
    vTaskDelay(10 / portTICK_PERIOD_MS);
    vTaskSuspend(NULL);
  }
}

extern "C" void app_main(void) {
  // Code below this line in app_main will not be reached
  ESP_LOGI("main", "Scan loop finished (should not happen)");
  static std::vector<int> output_gpios = {0, 1, 2, 3, 4, 5, 6};
  static std::vector<int> input_gpios = {10, 11, 12, 13};
  keyboard_btn_config_t cfg = {
      .output_gpios = output_gpios.data(),
      .input_gpios = input_gpios.data(),
      .output_gpio_num = output_gpios.size(),
      .input_gpio_num = input_gpios.size(),
      .active_level = 1,
      .debounce_ticks = 2,
      .ticks_interval = 500,      // us
      .enable_power_save = false, // enable power save
  };
  keyboard_btn_handle_t kbd_handle = NULL;
  esp_err_t err = keyboard_button_create(&cfg, &kbd_handle);
  if (err != ESP_OK) {
    ESP_LOGE("main", "keyboard_button_create failed: %s", esp_err_to_name(err));
    return;
  }

  keyboard_btn_cb_config_t cb_cfg = {
      .event = KBD_EVENT_PRESSED,
      .callback = keyboard_cb,
  };
  keyboard_button_register_cb(kbd_handle, cb_cfg, NULL);
  xTaskCreate(light_progress_task, "light_progress_task", 4096, NULL, 5,
              &light_progress_task_handle);
}
