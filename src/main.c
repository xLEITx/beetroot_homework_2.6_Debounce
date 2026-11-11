#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BUTTON_PIN GPIO_NUM_16

typedef enum {
    BUTTON_RELEASED,          
    BUTTON_DEBOUNCE_PRESS,    
    BUTTON_PRESSED,           
    BUTTON_DEBOUNCE_RELEASE 
} button_state_t;

static void button_polling_task(void* arg) {
    button_state_t state = BUTTON_RELEASED;
    uint16_t button_presses = 0;
    int debounce_ticks = 0;
    
    const int debounce_threshold = 3; 

    while (1) {
        int current_level = gpio_get_level(BUTTON_PIN);

        switch (state) {
            case BUTTON_RELEASED:
                if (current_level == 0) {
                    state = BUTTON_DEBOUNCE_PRESS;
                    debounce_ticks = 0;
                }
                break;

            case BUTTON_DEBOUNCE_PRESS:
                if (current_level == 0) {
                    debounce_ticks++;
                    if (debounce_ticks >= debounce_threshold) {
                        state = BUTTON_PRESSED;
                        button_presses++;
                        printf("Presses count: %d\n", button_presses);
                    }
                } else {
                    state = BUTTON_RELEASED;
                }
                break;

            case BUTTON_PRESSED:
                if (current_level == 1) {
                    state = BUTTON_DEBOUNCE_RELEASE;
                    debounce_ticks = 0;
                }
                break;

            case BUTTON_DEBOUNCE_RELEASE:
                if (current_level == 1) {
                    debounce_ticks++;
                    if (debounce_ticks >= debounce_threshold) {
                        state = BUTTON_RELEASED;
                    }
                } else {
                    state = BUTTON_PRESSED;
                }
                break;
        }

       
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void) {
    
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,         
        .mode = GPIO_MODE_INPUT,                
        .pin_bit_mask = (1ULL << BUTTON_PIN),   
        .pull_down_en = GPIO_PULLDOWN_DISABLE,  
        .pull_up_en = GPIO_PULLUP_ENABLE       
    };
    gpio_config(&io_conf);

    
    xTaskCreate(button_polling_task, "button_polling_task", 2048, NULL, 10, NULL);

    printf("Polling state machine initialized. No interrupts used.\n");
}