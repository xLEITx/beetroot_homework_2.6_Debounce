#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

#define BUTTON_PIN GPIO_NUM_16

static QueueHandle_t button_evt_queue = NULL;


static void IRAM_ATTR button_isr_handler(void* arg){
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(button_evt_queue, &gpio_num, NULL);
}

static void button_task(void* arg) {
    uint32_t io_num;
    uint16_t button_presses = 0;
    int64_t last_interrupt_time = 0;

    while (1) {
        if (xQueueReceive(button_evt_queue, &io_num, portMAX_DELAY)) {
            // Отримуємо поточний час у мілісекундах (заміна millis())
            int64_t current_time = esp_timer_get_time() / 1000;


            if (current_time - last_interrupt_time > 50) {
                button_presses++;
                last_interrupt_time = current_time;
                

                printf("Presses count: %d\n", button_presses);
            }
        }
    }
}

void app_main(void) {

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,          
        .mode = GPIO_MODE_INPUT,               
        .pin_bit_mask = (1ULL << BUTTON_PIN),   
        .pull_down_en = GPIO_PULLDOWN_DISABLE,  
        .pull_up_en = GPIO_PULLUP_ENABLE        
    };
    gpio_config(&io_conf);

 
    button_evt_queue = xQueueCreate(10, sizeof(uint32_t));


    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);

    
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, (void*) BUTTON_PIN);

    printf("System initialized. Waiting for button presses...\n");
}