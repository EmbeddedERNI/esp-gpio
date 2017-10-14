/**
 *  @brief     Example to manage GPIOs on Espressif ESP32 modules. 
 *  
 *  @file      main.c
 *  @author    Hernan Bartoletti - hernan.bartoletti@gmail.com
 *  @copyright MIT License 
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

#define PIN_OUTPUT  GPIO_NUM_23
#define PIN_INPUT   GPIO_NUM_22

const gpio_config_t g_gpio_config[] = {
    { 1<<PIN_OUTPUT, GPIO_MODE_OUTPUT|GPIO_MODE_INPUT, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE, GPIO_INTR_DISABLE }
,   { 1<<PIN_INPUT,  GPIO_MODE_INPUT,                  GPIO_PULLUP_ENABLE,  GPIO_PULLDOWN_DISABLE, GPIO_INTR_ANYEDGE }
,   { 0 }
};

static SemaphoreHandle_t g_semaphore = NULL;

static void IRAM_ATTR my_isr_handler(void* arg)
{
    if(g_semaphore)
    {
        if(pdTRUE != xSemaphoreGiveFromISR(g_semaphore, NULL))
        {   // Handle the error
        }
    }
}

static void my_task(void* arg)
{
    static TickType_t wait_for = 500 / portTICK_PERIOD_MS;

    if (g_semaphore)
    {
        for(;;) 
        {
            if(pdTRUE == xSemaphoreTake(g_semaphore, wait_for)) 
            {
                wait_for = portMAX_DELAY;
                printf("%08X: Processing input event on pin #%u on tick count!\n", xTaskGetTickCount(), PIN_INPUT);
            }

            if(ESP_OK==gpio_set_level(PIN_OUTPUT, !gpio_get_level( portMAX_DELAY!=wait_for ? PIN_OUTPUT : PIN_INPUT )))
            {
                if(portMAX_DELAY!=wait_for)
                {
                    printf("Setting level to %i for pin #%u!\n", gpio_get_level(PIN_OUTPUT), PIN_OUTPUT); 
                }
            }
            else
            {
                printf("ERROR during gpio_set_level for pin #%u!\n", PIN_OUTPUT); 
            }
        }
    }
    else
    {
        printf("ERROR. The semaphore must be created first!\n"); 
    }
}


void app_main(void)
{
    int i;


    for(i=0; g_gpio_config[i].pin_bit_mask; ++i)
    {
        if(ESP_OK!=gpio_config(&g_gpio_config[i]))
        {
            printf("ERROR during gpio_config for 0x%016llX mask!\n", g_gpio_config[i].pin_bit_mask); 
        }
    }

    //create a semaphore to synchronize events between the ISR and the worker task
    g_semaphore = xSemaphoreCreateBinary();

    //start worker task
    xTaskCreate(my_task, "my_task", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(0);

    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(PIN_INPUT, my_isr_handler, (void*) PIN_INPUT);

    for(;;)
    {   // Wait forever
        vTaskDelay( portMAX_DELAY );
    }
}
 
