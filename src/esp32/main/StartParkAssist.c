#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <esp_log.h>
#include <ssd1306.h>


// Chân LED (đa số board DevKitC có LED trên GPIO2).
#define LED_GPIO        GPIO_NUM_2

// Chu kỳ nháy: 2000 ms (2 giây) -> 1 giây sáng, 1 giây tắt
#define BLINK_PERIOD_MS 10000
#define LED_ON_MS       (BLINK_PERIOD_MS / 2)
#define LED_OFF_MS      (BLINK_PERIOD_MS / 2)

void app_main(void)
{
    // 1. Cấu hình chân GPIO thành output
    // gpio_config_t io_conf = {
    //     .pin_bit_mask = (1ULL << LED_GPIO),
    //     .mode = GPIO_MODE_OUTPUT,
    //     .pull_up_en = GPIO_PULLUP_DISABLE,
    //     .pull_down_en = GPIO_PULLDOWN_DISABLE,
    //     .intr_type = GPIO_INTR_DISABLE
    // };
    // gpio_config(&io_conf);

    // // 2. Đảm bảo trạng thái ban đầu (tắt)
    // gpio_set_level(LED_GPIO, 0);

    // // 3. Vòng lặp nháy LED
    // while (1) {
    //     // Bật
    //     gpio_set_level(LED_GPIO, 1);
    //     vTaskDelay(pdMS_TO_TICKS(LED_ON_MS));

    //     // Tắt
    //     gpio_set_level(LED_GPIO, 0);
    //     vTaskDelay(pdMS_TO_TICKS(LED_OFF_MS));
    // }
    init_ssd1306();

    while(1)
    {   
        clear_ssd1306();
        ssd1306_print_str(18, 0, "Hello World!", false);
        ssd1306_print_str(18, 17, "SSD1306 OLED", false);
        ssd1306_print_str(28, 27, "with ESP32", false);
        ssd1306_print_str(38, 37, "ESP-IDF", false);
        ssd1306_print_str(28, 47, "Embedded C", false);

        ssd1306_display();
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        
        clear_ssd1306();

        ssd1306_print_str(18, 0, "hANH!", false);
        ssd1306_print_str(18, 17, "SSH OLED", false);
        ssd1306_print_str(28, 27, "with ESP32", false);
        ssd1306_print_str(38, 37, "#$#$#$#@3", false);
        ssd1306_print_str(28, 47, "!!!!~~~~68", false);

        ssd1306_display();
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}