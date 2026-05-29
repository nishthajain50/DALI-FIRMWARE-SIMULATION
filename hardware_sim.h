// hardware.h - SIMULATION VERSION (No real hardware registers)

#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>
#include <stdio.h>

// ============================================================
// PIN DEFINITIONS (For simulation, these are just numbers)
// ============================================================
#define DALI_RX_PIN         0
#define DALI_TX_PIN         2
#define STATUS_LED_PIN      1
#define PWM_OUT_PIN         8

// ============================================================
// SIMULATED GPIO REGISTERS (Variables instead of hardware)
// ============================================================
extern uint32_t sim_GPIOA_ODR;
extern uint32_t sim_GPIOA_IDR;
extern uint32_t sim_GPIOA_MODER;
extern uint32_t sim_GPIOA_PUPDR;
extern uint32_t sim_GPIOA_OTYPER;
extern uint32_t sim_GPIOB_ODR;
extern uint32_t sim_RCC_AHBENR;

// Redirect register access to simulation variables
#define GPIOA_ODR   (sim_GPIOA_ODR)
#define GPIOA_IDR   (sim_GPIOA_IDR)
#define GPIOA_MODER (sim_GPIOA_MODER)
#define GPIOA_PUPDR (sim_GPIOA_PUPDR)
#define GPIOA_OTYPER(sim_GPIOA_OTYPER)
#define GPIOB_ODR   (sim_GPIOB_ODR)
#define RCC_AHBENR  (sim_RCC_AHBENR)

// ============================================================
// SIMULATED CLOCK ENABLE BITS
// ============================================================
#define RCC_GPIOA_EN    (1 << 17)
#define RCC_GPIOB_EN    (1 << 18)

// ============================================================
// DALI CONSTANTS (Same as before)
// ============================================================
#define HALF_BIT_US     416
#define FULL_BIT_US     833
#define LENGTH_MAX      8
#define DEBOUNCE_TIME   7
#define INTERVAL_MIN    8
#define INTERVAL_MAX    12

// DALI Commands
#define DALI_CMD_OFF        0x00
#define DALI_CMD_UP         0x01
#define DALI_CMD_DOWN       0x02
#define DALI_CMD_RECALL_MAX 0x05
#define DALI_CMD_RECALL_MIN 0x06

#define BROADCAST_ADDRESS   255

// ============================================================
// MANCHESTER ENCODING CONSTANTS
// ============================================================
#define LOGIC_0             0x02
#define LOGIC_1             0x01
#define FULL_LOWLEVEL       0x00
#define FULL_HIGHLEVEL      0x03
#define LOGIC_STOP          FULL_HIGHLEVEL
#define LOGIC_SPACING       FULL_LOWLEVEL
#define EDGETYPE_MARK       0x03
#define FIRLEVEL_MARK       0xff
#define SECLEVEL_MARK       0x00
#define FIRLEVEL_MARK16     0xffff
#define SECLEVEL_MARK16     0x0000

// ============================================================
// FLASH CONSTANTS (For simulation)
// ============================================================
#define EEPROM_START_ADDR   0x00007C00
#define EEPROM_MAGIC        0x5A3C

#endif