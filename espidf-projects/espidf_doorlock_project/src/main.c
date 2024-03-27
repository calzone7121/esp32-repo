#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "components/i2c-lcd.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "components/keypad.h"

#include <string.h>

//global variable declarations
static const char *TAG = "i2c-adapter";
int alarm_status = 0; //0 is off and 1 is on
char alarm_code[] = "1014";

//function prototypes
void display_alarm_status();
void check_code(char *submitted_code);

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
    }
    else
    {
        lcd_send_string("SYSTEM: DISARMED");
    }
    lcd_put_cur(2, 0);
    lcd_send_string("CODE: ");
    lcd_put_cur(2, 6);
}

void check_code(char *submitted_code)
{
    lcd_clear();
    lcd_put_cur(0, 0);
    if (strcmp(submitted_code, alarm_code) == 0)
    {
        if (alarm_status == 1)
        {   
            lcd_send_string("CODE ACCEPTED");
            lcd_put_cur(2, 0);
            lcd_send_string("DISABLING SYSTEM...");
            alarm_status = 0;
        }
        else if (alarm_status == 0)
        {
            lcd_send_string("CODE ACCEPTED");
            lcd_put_cur(2, 0);
            lcd_send_string("ENABLING SYSTEM...");
            alarm_status = 1;
        }
    }
    else
    {
        lcd_send_string("INCORRECT CODE");
        lcd_put_cur(2, 0);
        lcd_send_string("NICE TRY HAHAHA");
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    display_alarm_status();
}

void app_main(void)
{
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");
    lcd_init();
    keypad_init();
    display_alarm_status();
    char input_code[5] = "";
    while (1)
    {
        char key = keypad_scan();
       
        if (key == '*')
        {
            lcd_clear();
            display_alarm_status();
            input_code[0] = '\0';
        }
        else if (key == '#')
        {
            check_code(input_code);
            input_code[0] = '\0';
        }
        else if (strlen(input_code) >= 4)
        {
            continue;
        }
        else if (key != '\0')
        {
            lcd_write_char(key);
            strncat(input_code, &key, 1);
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
