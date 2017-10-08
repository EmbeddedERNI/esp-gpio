/**
 *  @brief     Example to manage GPIOs on Espressif ESP32 modules. 
 *  
 *  @file      main.c
 *  @author    Hernan Bartoletti - hernan.bartoletti@gmail.com
 *  @copyright MIT License 
 */

#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "driver/gpio.h"

const gpio_config_t g_gpio_config[] = {
    { GPIO_SEL_23, GPIO_MODE_OUTPUT, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE, GPIO_INTR_DISABLE }
,   { 0 }
};

void app_main(void)
{
    int i;

    for(i=0; g_gpio_config[i].pin_bit_mask; ++i)
    {
        if(ESP_OK!=gpio_config(&g_gpio_config[i]))
        {
            
        }
    }

    for (i=0;;i=!i) 
    {
        gpio_set_level(GPIO_NUM_23, i);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

