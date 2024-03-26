#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "components/i2c-lcd.h"
#include "nvs_flash.h"
#include "nvs.h"

//global variable declarations
static const char *TAG = "i2c-adapter";
int alarm_status = 0; //0 is off and 1 is on

//function prototypes
void display_alarm_status();

//i2c initialization
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_NUM_0;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}


void display_alarm_status()
{
    lcd_clear();
    lcd_put_cur(0, 0);
    if(alarm_status == 1)
    {
        lcd_send_string("SYSTEM: ARMED");
        alarm_status = 0;
    }
    else
    {
        lcd_send_string("SYSTEM: DISARMED");
        alarm_status = 1;
    }
}


void app_main(void)
{
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");
    lcd_init();

    lcd_write_char('s');

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    while(1)
    {
        display_alarm_status();
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }


}
