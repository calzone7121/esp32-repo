#include "keypad.h"

// Define GPIO pins for rows and columns
const gpio_num_t row_pins[ROWS] = {GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_12, GPIO_NUM_13};
const gpio_num_t col_pins[COLS] = {GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_18, GPIO_NUM_5};

// Define the keypad layout
const char keypad_layout[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Function to initialize keypad
void keypad_init() {
    // Initialize row pins as input with pull-up resistors
    for (int i = 0; i < ROWS; i++) {
        gpio_set_direction(row_pins[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(row_pins[i], GPIO_PULLUP_ONLY);
    }
    
    // Initialize column pins as output
    for (int i = 0; i < COLS; i++) {
        gpio_set_direction(col_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(col_pins[i], 1); // Set high by default
    }
}

// Function to scan keypad and return pressed key
char keypad_scan() {
    for (int i = 0; i < COLS; i++) {
        // Set column pin to low
        gpio_set_level(col_pins[i], 0);
        
        // Scan rows
        for (int j = 0; j < ROWS; j++) {
            if (gpio_get_level(row_pins[j]) == 0) {
                // Key pressed, return key
                return keypad_layout[j][i];
            }
        }
        
        // Set column pin back to high
        gpio_set_level(col_pins[i], 1);
    }
    
    return '\0'; // No key pressed
}
