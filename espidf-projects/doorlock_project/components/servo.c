#include "servo.h"

void servo_init() {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_SERVO_CONTROL_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

void servo_set_angle(int angle) {
    uint32_t duty = SG90_MIN_PULSEWIDTH + (angle * (SG90_MAX_PULSEWIDTH - SG90_MIN_PULSEWIDTH) / SG90_MAX_DEGREE);
    gpio_set_level(GPIO_SERVO_CONTROL_PIN, 1);
    esp_rom_delay_us(duty);
    gpio_set_level(GPIO_SERVO_CONTROL_PIN, 0);
    vTaskDelay(20 / portTICK_PERIOD_MS); // Wait for 20ms (50Hz) for servo to settle
}
