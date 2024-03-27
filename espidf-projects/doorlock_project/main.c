#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "components/keypad.h"
#include "components/i2c-lcd.h"
#include "components/servo.h"


//global variable declarations
static const char *TAG = "i2c-adapter";
int32_t alarm_status = 0; //0 is off and 1 is on. This is the default value if no data exists on the flash storage
char alarm_code[] = "1014";
nvs_handle_t alarm_state_handle;

//function prototypes
void setup_function();
void display_alarm_status(int32_t current_alarm_status);
void display_unlock_message();
void display_lock_message(); 
void display_wrong_message();
void check_code(char *submitted_code);
void unlock_door();
void lock_door();

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

void setup_function()
{
    nvs_flash_init();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    nvs_open("storage", NVS_READWRITE, &alarm_state_handle);
    nvs_get_i32(alarm_state_handle, "data", &alarm_status);
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");
    lcd_init();
    keypad_init();
    servo_init();
    display_alarm_status(alarm_status);
}

void display_alarm_status(int32_t current_alarm_state)
{
    lcd_clear();
    lcd_put_cur(0, 0);
    if(current_alarm_state == 1)
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

void display_unlock_message()
{
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("CODE ACCEPTED");
    lcd_put_cur(2, 0);
    lcd_send_string("DISABLING SYSTEM...");
}

void display_lock_message()
{
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("CODE ACCEPTED");
    lcd_put_cur(2, 0);
    lcd_send_string("ENABLING SYSTEM...");
}

void display_wrong_message()
{
    lcd_clear();
    lcd_send_string("INCORRECT CODE");
    lcd_put_cur(2, 0);
    lcd_send_string("NICE TRY HA HA HA");
}

void check_code(char *submitted_code)
{

    if (strcmp(submitted_code, alarm_code) == 0)
    {
        if (alarm_status == 1)
        {   
            display_unlock_message();
            alarm_status = 0;
            nvs_set_i32(alarm_state_handle, "data", alarm_status);
            nvs_commit(alarm_state_handle);
            //unlock_door();
            xTaskCreate(&unlock_door, "unlock_door", 4096, NULL, 10, NULL);
        }
        else if (alarm_status == 0)
        {
            display_lock_message();
            alarm_status = 1;
            nvs_set_i32(alarm_state_handle, "data", alarm_status);
            nvs_commit(alarm_state_handle);
            //lock_door();
            xTaskCreate(&lock_door, "lock_door", 4096, NULL, 10, NULL);
        }
    }
    else
    {   
        display_wrong_message();
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    display_alarm_status(alarm_status);
}

void unlock_door(void * pvParameter)
{
    for (int i = 0; i <= 180; i++) 
    {
        servo_set_angle(i);
    }
    vTaskDelete(NULL);
}

void lock_door(void * pvParameter)
{
    for (int i = 180; i >= 0; i--) 
    {
        servo_set_angle(i);
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    setup_function();
    char input_code[5] = "";
    while (1)
    {
        char key = keypad_scan();
       
        if (key == '*')
        {
            lcd_clear();
            display_alarm_status(alarm_status);
            input_code[0] = '\0';
            //esp_restart();
        }
        else if (key == 'A')
        {
            esp_restart();
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
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

