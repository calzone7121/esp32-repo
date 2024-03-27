#ifndef SERVO_H
#define SERVO_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_SERVO_CONTROL_PIN 15
#define SG90_MIN_PULSEWIDTH 500
#define SG90_MAX_PULSEWIDTH 2500
#define SG90_MAX_DEGREE 180

void servo_init();
void servo_set_angle(int angle);

#endif /* SG90_H */
