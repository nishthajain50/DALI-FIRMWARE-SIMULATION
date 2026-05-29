// led_dimmer.c - LED PWM CONTROL (Simulation Version)

#include "led_dimmer_sim.h"
#include "hardware_sim.h"
#include <stdio.h>

static uint8_t current_brightness = 0;

void LED_Init(void)
{
    printf("SIM: LED Dimmer initialized\n");
}


void LED_SetBrightness(uint8_t brightness)
{
    if(brightness > 254) brightness = 254;
    current_brightness = brightness;
    printf("SIM: LED Brightness = %d/254 (%d%%)\n", 
           brightness, (brightness * 100) / 254);
}

void LED_Update(void)
{
    static uint16_t pwm_counter = 0;
    pwm_counter++;
    if(pwm_counter >= 1000) {
        pwm_counter = 0;
    }
}

uint8_t LED_GetBrightness(void)
{
    return current_brightness;
}