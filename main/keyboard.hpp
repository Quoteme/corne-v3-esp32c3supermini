
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "keyboard_button.h"
#include <vector>

class Keyboard {
public:
  Keyboard(std::vector<int> out, std::vector<int> in)
      : out_(std::move(out)), in_(std::move(in)) {

    keyboard_btn_config_t cfg = {
        .output_gpios = out_.data(),
        .input_gpios = in_.data(),
        .output_gpio_num = static_cast<uint8_t>(out_.size()),
        .input_gpio_num = static_cast<uint8_t>(in_.size()),
        .active_level = 1,
        .debounce_ticks = 2,
        .ticks_interval = 500,
        .enable_power_save = false,
    };

    if (keyboard_button_create(&cfg, &kbd_) != ESP_OK)
      ESP_LOGE(TAG, "Failed to create keyboard");

    keyboard_btn_cb_config_t cb_cfg = {.event = KBD_EVENT_PRESSED,
                                       .callback = &Keyboard::on_key,
                                       .user_data = this};
    keyboard_button_register_cb(kbd_, cb_cfg, NULL);

    xTaskCreate(&Keyboard::task_entry, "kbd_task", 4096, this, 5, &task_);
  }

  ~Keyboard() {
    if (kbd_)
      keyboard_button_delete(kbd_);
    if (task_)
      vTaskDelete(task_);
  }

private:
  static constexpr const char *TAG = "Keyboard";
  std::vector<int> out_, in_;
  keyboard_btn_handle_t kbd_ = nullptr;
  TaskHandle_t task_ = nullptr;

  static void on_key(keyboard_btn_handle_t, keyboard_btn_report_t r,
                     void *arg) {
    auto *self = static_cast<Keyboard *>(arg);
    if (eTaskGetState(self->task_) == eSuspended)
      vTaskResume(self->task_);
    ESP_LOGI(TAG, "Keys pressed: %lu", r.key_pressed_num);
  }

  static void task_entry(void *arg) {
    while (true) {
      vTaskDelay(pdMS_TO_TICKS(10));
      vTaskSuspend(nullptr);
    }
  }
};
