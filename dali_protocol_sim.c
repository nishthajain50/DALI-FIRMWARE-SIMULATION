// dali_protocol.c - DALI COMMAND HANDLING (NO HARDWARE)

#include "dali_protocol_sim.h"
#include "led_dimmer_sim.h"
#include "hardware_sim.h"
#include <stdio.h>

// Global device structure
DaliDevice_t DaliDevice;

// Simple delay (no hardware)
static void delay_ms(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; i++) {
        for(uint32_t j = 0; j < 1000; j++) {
            __asm("nop");
        }
    }
}

// Status LED simulation (no hardware)
static void StatusLED_On(void)
{
    printf("SIM: Status LED ON\n");
}

static void StatusLED_Off(void)
{
    printf("SIM: Status LED OFF\n");
}
void DALI_Protocol_Init(uint8_t address)
{
    DaliDevice.short_address = address;
    DaliDevice.brightness = 0;
    DaliDevice.target_brightness = 0;
    DaliDevice.power_on = 0;
    DaliDevice.min_level = 0;
    DaliDevice.max_level = 254;
    DaliDevice.fade_time = 0;
    printf("SIM: DALI Protocol initialized, Address: %d\n", address);
}
uint8_t DALI_IsMyAddress(uint8_t address)
{
    if(address == DaliDevice.short_address) return 1;
    if(address == BROADCAST_ADDRESS) return 1;
    return 0;
}
void DALI_ProcessCommand(uint8_t address, uint8_t command)
{ if(!DALI_IsMyAddress(address)) {
        printf("SIM: Command not for me (Addr:%d)\n", address);
        return;}
    StatusLED_On();
    printf("SIM: Processing command 0x%02X\n", command);
    switch(command) {
        case DALI_CMD_OFF:
            DaliDevice.target_brightness = 0;
            DaliDevice.power_on = 0;
            LED_SetBrightness(0);
            printf("SIM: Light OFF\n");
            break;
            case DALI_CMD_UP:
            if(DaliDevice.target_brightness < DaliDevice.max_level) {
                DaliDevice.target_brightness++;
                DaliDevice.power_on = 1;
                LED_SetBrightness(DaliDevice.target_brightness);
                printf("SIM: Brightness UP to %d\n", DaliDevice.target_brightness);
            }
            break;
            
        case DALI_CMD_DOWN:
            if(DaliDevice.target_brightness > DaliDevice.min_level) {
                DaliDevice.target_brightness--;
                if(DaliDevice.target_brightness == 0) DaliDevice.power_on = 0;
                LED_SetBrightness(DaliDevice.target_brightness);
                printf("SIM: Brightness DOWN to %d\n", DaliDevice.target_brightness);
            }
            break;
            
        case DALI_CMD_RECALL_MAX:
            DaliDevice.target_brightness = DaliDevice.max_level;
            DaliDevice.power_on = 1;
            LED_SetBrightness(DaliDevice.target_brightness);
            printf("SIM: Brightness MAX (%d)\n", DaliDevice.target_brightness);
            break;
            
        case DALI_CMD_RECALL_MIN:
            DaliDevice.target_brightness = DaliDevice.min_level;
            DaliDevice.power_on = (DaliDevice.min_level > 0);
            LED_SetBrightness(DaliDevice.target_brightness);
            printf("SIM: Brightness MIN (%d)\n", DaliDevice.target_brightness);
            break;
            
        default:
            printf("SIM: Unknown command 0x%02X\n", command);
            break;
    }
    
    DaliDevice.brightness = DaliDevice.target_brightness;
    delay_ms(50);
    StatusLED_Off();
}

void DALI_SetBrightness(uint8_t brightness)
{
    if(brightness > DaliDevice.max_level) brightness = DaliDevice.max_level;
    DaliDevice.target_brightness = brightness;
    DaliDevice.power_on = (brightness > 0);
    LED_SetBrightness(brightness);
    DaliDevice.brightness = brightness;
}

uint8_t DALI_GetBrightness(void)
{
    return DaliDevice.brightness;
}

void DALI_SaveState(void)
{
    printf("SIM: State saved\n");
}