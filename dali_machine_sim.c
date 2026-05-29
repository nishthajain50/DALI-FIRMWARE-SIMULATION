// dali_machine.c - SIMULATION VERSION
// This version simulates Manchester encoding without real GPIO

#include "dali_machine_sim.h"
#include "hardware_sim.h"
#include <stdio.h>

// ============================================================
// SIMULATION VARIABLES
// ============================================================
static uint8_t simulated_rx_buffer[100];
static uint8_t simulated_rx_index = 0;
static uint8_t simulated_rx_length = 0;
static uint8_t simulation_tx_byte = 0;

// ============================================================
// SIMULATION DELAY (reduced for faster simulation)
// ============================================================
static void delay_us(uint32_t us)
{
    // Much shorter delay for simulation
    for(uint32_t i = 0; i < us / 100; i++) {
        __asm("nop");
    }
}

static void __attribute__((unused)) delay_ms(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; i++) {
        delay_us(100);
    }
}

// ============================================================
// SIMULATION GPIO FUNCTIONS (prints instead of actual pin)
// ============================================================
static void GPIO_Init_DALI(void)
{
    printf("SIM: DALI pins initialized\n");
}

static void GPIO_Write_TX(uint8_t value)
{
    // Print TX activity for debugging
    if(value) {
        printf("SIM: TX HIGH\n");
    } else {
        printf("SIM: TX LOW\n");
    }
}

static uint8_t GPIO_Read_RX(void)
{
    // Return simulated RX data
    if(simulated_rx_index < simulated_rx_length) {
        return simulated_rx_buffer[simulated_rx_index] ? 1 : 0;
    }
    return 1;  // Idle HIGH
}

// ============================================================
// SIMULATION DATA INJECTION
// ============================================================
// Use this function to simulate incoming DALI commands
void SIM_InjectDALICommand(uint8_t address, uint8_t command)
{
    // Convert address and command to Manchester bits
    uint8_t frame[18];  // Start + 8 address bits + 8 command bits + stop
    int idx = 0;
    
    // Start bit (1)
    frame[idx++] = 1;
    
    // Address bits (MSB first)
    for(int i = 7; i >= 0; i--) {
        frame[idx++] = (address >> i) & 0x01;
    }
    
    // Command bits (MSB first)
    for(int i = 7; i >= 0; i--) {
        frame[idx++] = (command >> i) & 0x01;
    }
    
    // Stop bits (idle HIGH)
    frame[idx++] = 1;
    frame[idx++] = 1;
    
    simulated_rx_length = idx;
    for(int i = 0; i < idx; i++) {
        simulated_rx_buffer[i] = frame[i];
    }
    simulated_rx_index = 0;
    
    printf("\n=== SIM: Injected DALI Command ===\n");
    printf("SIM: Address: 0x%02X (%d)\n", address, address);
    printf("SIM: Command: 0x%02X\n", command);
    printf("================================\n");
}

// ============================================================
// PUBLIC MANCHESTER FUNCTIONS (same as real version)
// ============================================================

void DALI_Machine_Init(void)
{
    GPIO_Init_DALI();
    // Initialize simulation buffer
    simulated_rx_length = 0;
    simulated_rx_index = 0;
    printf("SIM: DALI Machine initialized (Simulation Mode)\n");
}

void DALI_SendBit(uint8_t bit)
{
    if(bit == 1) {
        GPIO_Write_TX(0); delay_us(HALF_BIT_US);
        GPIO_Write_TX(1); delay_us(HALF_BIT_US);
    } else {
        GPIO_Write_TX(1); delay_us(HALF_BIT_US);
        GPIO_Write_TX(0); delay_us(HALF_BIT_US);
    }
}

void DALI_SendByte(uint8_t data)
{
    printf("SIM: Sending byte: 0x%02X\n", data);
    DALI_SendBit(1);  // Start bit
    for(int i = 7; i >= 0; i--) {
        DALI_SendBit((data >> i) & 0x01);
    }
    GPIO_Write_TX(1);
    delay_us(FULL_BIT_US * 2);
    simulation_tx_byte = data;
    printf("SIM: Byte sent\n");
}

uint8_t DALI_ReceiveBit(void)
{
    uint8_t first, second;
    
    first = GPIO_Read_RX();
    delay_us(HALF_BIT_US);
    second = GPIO_Read_RX();
    delay_us(HALF_BIT_US);
    
    if(simulated_rx_index < simulated_rx_length) {
        simulated_rx_index++;
    }
    
    if(first == 0 && second == 1) return 1;
    if(first == 1 && second == 0) return 0;
    return 2;
}

uint8_t DALI_ReceiveByte(uint32_t timeout_ms)
{
    uint8_t data = 0;
    uint32_t timeout = 0;
    
    // In simulation, immediately return if we have injected data
    if(simulated_rx_length > 0) {
        // Wait for falling edge
        while(GPIO_Read_RX() == 1) {
            timeout++;
            if(timeout > timeout_ms * 10) break;
            delay_us(1);
        }
        
        if(GPIO_Read_RX() == 1 && simulated_rx_length == 0) {
            return 0xFF;
        }
        
        // Verify start bit
        delay_us(HALF_BIT_US);
        if(GPIO_Read_RX() == 0 && simulated_rx_length == 0) return 0xFF;
        delay_us(HALF_BIT_US);
        
        // Receive 8 data bits
        for(int i = 7; i >= 0; i--) {
            uint8_t bit = DALI_ReceiveBit();
            if(bit == 2) return 0xFF;
            data |= (bit << i);
        }
        
        delay_us(FULL_BIT_US * 2);
        
        // Clear simulated buffer
        simulated_rx_length = 0;
        
        return data;
    }
    
    return 0xFF;  // No data
}

uint8_t DALI_IsDataAvailable(void)
{
    return (simulated_rx_length > 0) ? 1 : 0;
}