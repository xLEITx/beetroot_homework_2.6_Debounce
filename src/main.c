#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BUTTON_PIN GPIO_NUM_16

static volatile uint16_t buttonPresses = 0;
static volatile bool isButtonPressed = false;

static void IRAM_ATTR button_isr_handler(void* arg){
    buttonPresses++;
    isButtonPressed = true;
}

void app_main(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL);

    while (1)
    {
        if (isButtonPressed)
        {
            printf("Number of presses: %d\n", buttonPresses);
            isButtonPressed = false;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
        
    }
    

}