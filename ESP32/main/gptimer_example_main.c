#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define START_PORT_PIN GPIO_NUM_2
#define MAIN_CLOCK_PIN GPIO_NUM_4
#define LED_PIN GPIO_NUM_5

int count = 0;
int start = 1;

int reset = 0;
int reset_count = 18;

int measurement_count = 111;
int measurement = 0;
int led = 0;

bool border = false;
bool high_frequency = true;

void cpl_bit(gpio_num_t pin) {
    gpio_set_level(pin, !gpio_get_level(pin));
}

static bool IRAM_ATTR timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data) {
    if ((count == 1 || count == 5) && border == true) {
        cpl_bit(START_PORT_PIN);
        border = !border;
    }
    if ((count == 4 || count == 8) && (border == false)) {
        cpl_bit(START_PORT_PIN);
        border = !border;
        if (count == 8 && start == 0) {
            cpl_bit(MAIN_CLOCK_PIN);
        }
        if (start != 1) {
            count = 0;
        }
        if (reset == 1) {
            reset_count--;
        }
        if (reset_count == 0 && reset == 1) {
            reset = 0;
            reset_count = 18;
            measurement = 1;
        }
        if (measurement_count != 0 && measurement == 1) {
            led = 1;
            measurement_count--;
        } else if (measurement_count == 0 && measurement == 1) {
            measurement = 0;
            measurement_count = 111;
            if (high_frequency) {
                start = 1;
            } else {
                start = 0;
            }
            led = 0;
        }
        if (led == 1) {
            gpio_set_level(LED_PIN, 1);
        } else {
            gpio_set_level(LED_PIN, 0);
        }
    }
    if (count == 6 && start == 1) {
        cpl_bit(MAIN_CLOCK_PIN);
        reset = 1;
        start = 0;
    }
    count++;
    if (high_frequency) {
        gptimer_set_raw_count(timer, 0);
    }
    return false;
}

void app_main(void) {
    // Initialize GPIOs
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << START_PORT_PIN) | (1ULL << MAIN_CLOCK_PIN) | (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Initialize Timer
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000 // 1 MHz
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_on_alarm_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 5, // 200 kHz, 5 us per tick
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    while (true) {
        // Main loop can handle other tasks, if needed
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
