// memory.c - EEPROM SIMULATION (No hardware access)

#include "memory.h"
#include "hardware_sim.h"
#include <stdio.h>
#include <string.h>

static DaliParams_t simulated_eeprom;
static uint8_t eeprom_initialized = 0;

void MEMORY_Init(void)
{
    printf("SIM: Memory initialized\n");
    if(!eeprom_initialized) {
        memset(&simulated_eeprom, 0xFF, sizeof(DaliParams_t));
        eeprom_initialized = 1;
    }
}

uint8_t MEMORY_Save(DaliParams_t *params)
{
    memcpy(&simulated_eeprom, params, sizeof(DaliParams_t));
    printf("SIM: Settings saved - Addr:%d, Brightness:%d\n", 
           params->short_address, params->last_brightness);
    return 1;
}

uint8_t MEMORY_Load(DaliParams_t *params)
{
    if(simulated_eeprom.magic == EEPROM_MAGIC) {
        memcpy(params, &simulated_eeprom, sizeof(DaliParams_t));
        printf("SIM: Settings loaded from memory\n");
        return 1;
    }
    
    params->magic = EEPROM_MAGIC;
    params->short_address = 1;
    params->last_brightness = 0;
    params->min_level = 0;
    params->max_level = 254;
    params->power_on_level = 254;
    params->fade_time = 0;
    memset(params->reserved, 0, 8);
    
    memcpy(&simulated_eeprom, params, sizeof(DaliParams_t));
    printf("SIM: Using default settings\n");
    return 0;
}

uint8_t MEMORY_IsValid(void)
{
    return (simulated_eeprom.magic == EEPROM_MAGIC);
}

void MEMORY_ResetToDefaults(DaliParams_t *params)
{
    params->magic = EEPROM_MAGIC;
    params->short_address = 1;
    params->last_brightness = 0;
    params->min_level = 0;
    params->max_level = 254;
    params->power_on_level = 254;
    params->fade_time = 0;
    printf("SIM: Reset to defaults\n");
}