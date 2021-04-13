#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "i2c-lcd1602.h"
#include "lcd.h"
#include "smbus.h"
#include <string.h>

/*
Toegevoegde code voor proftaak individueel van Samir Rademakers staat
in de lcd component in lcd.c vanaf line 106 en in lcd.h vanaf line 11.
**/

//Method to test all added methods in lcd.c
void test_extension(void *pvParameter)
{
    setup_lcd();
    // write_random_numbers();
    // show_all_planets();
    // create_goodbye_screen();
    // create_welcome_screen();
    // start_time_counter();
    // set_timer(0,1,2);
    // show_order_of_operations();
    // show_table_of_value(8);
    // show_alphabet();
    vTaskDelete(NULL);
}

void app_main()
{
    xTaskCreate(&test_extension, "test_extension", 4096, NULL, 5, NULL);
}
