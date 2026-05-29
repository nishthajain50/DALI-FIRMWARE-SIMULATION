// main.c
// High-frequency SysTick software PWM

#include "led_dimmer.h"
#include "lks32mc03x.h"

// ============================================================
// GLOBAL TICK COUNTER
// ============================================================

volatile uint32_t msTicks = 0;

// ============================================================
// SYSTICK INTERRUPT
// ============================================================

void SysTick_Handler(void)
{
    msTicks++;

    // Continuous PWM generation
    LED_Update();
}

// ============================================================
// DELAY FUNCTION
// ============================================================

void delay_ms(uint32_t ms)
{
    uint32_t current = msTicks;

    while((msTicks - current) < ms)
    {
    }
}

// ============================================================
// MAIN
// ============================================================

int main(void)
{
    // Initialize PWM GPIO
    LED_Init();

    // ========================================================
    // SysTick = 50kHz interrupt
    //
    // 48MHz / 960 = 50kHz
    // PWM frequency:
    // 50kHz / 50 steps = 1kHz PWM
    // ========================================================

    SysTick_Config(960);

    // Brightness variables
    uint8_t brightness = 0;
    uint8_t increasing = 1;

    // ========================================================
    // MAIN LOOP
    // ========================================================

    while(1)
    {
        // Set brightness
        LED_SetBrightness(brightness);

        // Fade speed
        delay_ms(40);

        // Ramp UP
        if(increasing)
        {
            brightness++;

            if(brightness >= 254)
            {
                increasing = 0;
            }
        }
        // Ramp DOWN
        else
        {
            brightness--;

            if(brightness == 0)
            {
                increasing = 1;
            }
        }
    }
}







/*#include "led_dimmer.h"
#include "lks32mc03x.h"

// ============================================================
// SIMPLE DELAY
// ============================================================

static void delay_ms(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; i++)
    {
        for(volatile uint32_t j = 0; j < 3000; j++)
        {
            __NOP();
        }
    }
}

// ============================================================
// SYSTICK INTERRUPT
// Runs continuously
// ============================================================

void SysTick_Handler(void)
{
    LED_Update();
}

// ============================================================
// MAIN
// ============================================================

int main(void)
{
    // Initialize PWM GPIO
    LED_Init();

    // --------------------------------------------------------
    // Configure SysTick
    // 48MHz / 10000 = 4800
    // Gives 10kHz interrupt
    // --------------------------------------------------------

    SysTick_Config(4800);

    // Brightness variables
    uint8_t brightness = 0;
    uint8_t increasing = 1;

    // ========================================================
    // MAIN LOOP
    // ========================================================

    while(1)
    {
        // Set brightness
        LED_SetBrightness(brightness);

        // Fade speed
        delay_ms(20);

        // Brightness ramp UP
        if(increasing)
        {
            brightness++;

            if(brightness >= 254)
            {
                increasing = 0;
            }
        }
        // Brightness ramp DOWN
        else
        {
            brightness--;

            if(brightness == 0)
            {
                increasing = 1;
            }
        }
    }
}
*/







// =============================================================
// main.c - DALI Light Dimmer (REAL HARDWARE VERSION)
// Target MCU : LKS32MC033H6PB8
// System Clock: 48 MHz
// PWM Timer   : 10 kHz tick (100us period) via TIM0 interrupt
// PWM Pin     : PB8 (P1_8)
// DALI TX/RX  : configured in dali_protocol.c
// =============================================================
/*
#include "dali_machine.h"
#include "dali_protocol.h"
#include "led_dimmer.h"
#include "memory.h"
#include "hardware.h"

// ------------------------------------------------------------
// SYSTEM CLOCK: 48 MHz
// TIMER RELOAD : 48,000,000 / 10,000 = 4800 - 1 = 4799
// This gives 10kHz interrupt = 100us tick for software PWM
// ------------------------------------------------------------
#define SYS_CLK_HZ        48000000UL
#define PWM_TICK_HZ        10000UL
#define TIMER_RELOAD      ((SYS_CLK_HZ / PWM_TICK_HZ) - 1)   // 4799

// LKS32MC033 register definitions
// (include these if not already in hardware.h)
#ifndef RCC_APBENR
#define RCC_APBENR        (*((volatile uint32_t *)0x40021014))
#endif
#ifndef RCC_TIM0_EN
#define RCC_TIM0_EN       (1 << 0)
#endif

// TIM0 registers (basic timer on LKS32MC033)
#ifndef TIM0_BASE
#define TIM0_BASE         0x40012C00
#endif
#define TIM0_CR1          (*((volatile uint32_t *)(TIM0_BASE + 0x00)))
#define TIM0_DIER         (*((volatile uint32_t *)(TIM0_BASE + 0x0C)))
#define TIM0_SR           (*((volatile uint32_t *)(TIM0_BASE + 0x10)))
#define TIM0_PSC          (*((volatile uint32_t *)(TIM0_BASE + 0x28)))
#define TIM0_ARR          (*((volatile uint32_t *)(TIM0_BASE + 0x2C)))

#define TIM_CR1_CEN       (1 << 0)   // Counter enable
#define TIM_DIER_UIE      (1 << 0)   // Update interrupt enable
#define TIM_SR_UIF        (1 << 0)   // Update interrupt flag

// NVIC (Cortex-M0 compatible)
#define NVIC_ISER         (*((volatile uint32_t *)0xE000E100))
#define TIM0_IRQn         13          // Check LKS32MC033 datasheet for correct IRQ number

// ------------------------------------------------------------
// GLOBAL STATE
// ------------------------------------------------------------
DaliParams_t SystemParams;

// Flag set by TIM0 ISR, consumed in main loop
static volatile uint8_t pwm_tick_flag = 0;

// ------------------------------------------------------------
// TIMER 0 INIT — 10 kHz interrupt for software PWM
// ------------------------------------------------------------
static void Timer0_Init(void)
{
    // Enable TIM0 peripheral clock
    RCC_APBENR |= RCC_TIM0_EN;

    // Prescaler = 0 (no prescale, runs at 48 MHz)
    TIM0_PSC = 0;

    // Auto-reload = 4799 ? overflow at 10 kHz
    TIM0_ARR = TIMER_RELOAD;

    // Enable update interrupt
    TIM0_DIER |= TIM_DIER_UIE;

    // Enable TIM0 IRQ in NVIC
    NVIC_ISER |= (1 << TIM0_IRQn);

    // Start the counter
    TIM0_CR1 |= TIM_CR1_CEN;
}

// ------------------------------------------------------------
// TIM0 INTERRUPT HANDLER — fires every 100 us
// Keep this SHORT — only set flag, do work in main loop
// ------------------------------------------------------------
void TIM0_IRQHandler(void)
{
    // Clear the update interrupt flag
    TIM0_SR &= ~TIM_SR_UIF;

    // Update PWM output immediately (time-critical)
    LED_Update();

    // Set flag for main loop tasks
    pwm_tick_flag = 1;
}

// ------------------------------------------------------------
// SIMPLE DELAY using NOP loops
// At 48 MHz: ~1000 NOPs ˜ 20us, so 50000 ˜ 1ms
// ------------------------------------------------------------

static void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < 2800; j++) {
            __asm("nop");
        }
    }
}

// ------------------------------------------------------------
// STATUS LED BLINK (uses same PB8 pin before PWM starts,
// or use a separate status LED pin if available)
// ------------------------------------------------------------
static void StatusLED_Blink(uint8_t times)
{
    for (uint8_t i = 0; i < times; i++) {
        LED_SetBrightness(254);   // Full ON
        delay_ms(200);
        LED_SetBrightness(0);     // OFF
        delay_ms(200);
    }
}

// ------------------------------------------------------------
// TEST SEQUENCE — runs once on startup to verify PWM + DALI
// Visible on Logic Analyzer as changing duty cycle
// ------------------------------------------------------------
static void Run_Test_Sequence(void)
{
    // --- TEST 1: Recall MAX brightness ---
    // Logic Analyzer: should see ~100% duty cycle on PB8
    DALI_ProcessCommand(1, DALI_CMD_RECALL_MAX);
    LED_SetBrightness(DALI_GetBrightness());
    delay_ms(1000);

    // --- TEST 2: Step UP brightness 10 times ---
    // Logic Analyzer: duty cycle increasing in steps
    for (int i = 0; i < 10; i++) {
        DALI_ProcessCommand(1, DALI_CMD_UP);
        LED_SetBrightness(DALI_GetBrightness());
        delay_ms(300);
    }
    delay_ms(1000);

    // --- TEST 3: Step DOWN brightness 5 times ---
    // Logic Analyzer: duty cycle decreasing in steps
    for (int i = 0; i < 5; i++) {
        DALI_ProcessCommand(1, DALI_CMD_DOWN);
        LED_SetBrightness(DALI_GetBrightness());
        delay_ms(300);
    }
    delay_ms(1000);

    // --- TEST 4: OFF ---
    // Logic Analyzer: flat LOW on PB8
    DALI_ProcessCommand(1, DALI_CMD_OFF);
    LED_SetBrightness(0);
    delay_ms(1500);

    // --- TEST 5: Recall MIN brightness ---
    // Logic Analyzer: very small duty cycle (near 0%)
    DALI_ProcessCommand(1, DALI_CMD_RECALL_MIN);
    LED_SetBrightness(DALI_GetBrightness());
    delay_ms(1500);

    // --- TEST 6: Broadcast RECALL MAX ---
    // Logic Analyzer: back to ~100% duty cycle
    DALI_ProcessCommand(255, DALI_CMD_RECALL_MAX);
    LED_SetBrightness(DALI_GetBrightness());
    delay_ms(2000);

    // --- TEST 7: Ramp from 0 to 254 smoothly ---
    // Logic Analyzer: smooth duty cycle sweep, good for checking linearity
    for (uint8_t b = 0; b <= 254; b++) {
        LED_SetBrightness(b);
        delay_ms(10);   // 10ms per step = ~2.5 second full sweep
    }
    delay_ms(500);

    // --- TEST 8: Set to 50% and hold ---
    // Logic Analyzer: stable 50% duty cycle square wave
    // THIS IS THE BEST STATE TO CHECK IN LOGIC 2
    LED_SetBrightness(127);
    DALI_SetBrightness(127);
    delay_ms(3000);
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main(void)
{
    // NOTE: SystemInit() is called automatically by the startup file
    // (startup_lks32mc03x.s) BEFORE main() — do NOT call it again here.
    // The 48 MHz clock is already active when we reach main().

    // ----------------------------------------------------------
    // PERIPHERAL INIT
    // ----------------------------------------------------------
    MEMORY_Init();
    LED_Init();          // Configures PB8 as output, starts LOW
    Timer0_Init();       // Starts 10kHz interrupt ? LED_Update()

    // ----------------------------------------------------------
    // 3. LOAD SAVED PARAMETERS FROM FLASH
    // ----------------------------------------------------------
    if (MEMORY_Load(&SystemParams)) {
        // Parameters loaded — blink once to confirm
        StatusLED_Blink(1);
    } else {
        // No saved params — use defaults, blink 3 times
        SystemParams.short_address   = 1;
        SystemParams.last_brightness = 127;
        StatusLED_Blink(3);
    }

    // ----------------------------------------------------------
    // 4. DALI PROTOCOL INIT
    // ----------------------------------------------------------
    DALI_Machine_Init();
    DALI_Protocol_Init(SystemParams.short_address);

    // ----------------------------------------------------------
    // 5. RESTORE LAST BRIGHTNESS
    // ----------------------------------------------------------
    LED_SetBrightness(SystemParams.last_brightness);
    DALI_SetBrightness(SystemParams.last_brightness);

    // Ready blink
    StatusLED_Blink(2);

    // ----------------------------------------------------------
    // 6. TEST SEQUENCE (startup verification)
    //    Connect Logic Analyzer to PB8 now and press CAPTURE
    //    You will see duty cycle changing through all levels
    // ----------------------------------------------------------
    Run_Test_Sequence();

    // After test, restore saved brightness
    LED_SetBrightness(SystemParams.last_brightness);
    DALI_SetBrightness(SystemParams.last_brightness);

    // ----------------------------------------------------------
    // 7. MAIN LOOP — wait for and process DALI commands
    // ----------------------------------------------------------
    while (1)
    {
        // Check if a DALI frame has arrived on the bus
        if (DALI_IsDataAvailable())
        {
            uint8_t address = DALI_ReceiveByte(10);

            if (address != 0xFF)
            {
                uint8_t command = DALI_ReceiveByte(10);

                if (command != 0xFF)
                {
                    // Process the DALI command
                    DALI_ProcessCommand(address, command);

                    // Update PWM to match new brightness
                    LED_SetBrightness(DALI_GetBrightness());

                    // Save to flash if brightness changed
                    if (DALI_GetBrightness() != SystemParams.last_brightness)
                    {
                        SystemParams.last_brightness = DALI_GetBrightness();
                        MEMORY_Save(&SystemParams);
                    }
                }
            }
        }

        // pwm_tick_flag can be used for periodic tasks
        // (LED_Update is already called inside TIM0_IRQHandler)
        if (pwm_tick_flag)
        {
            pwm_tick_flag = 0;
            // Add any 100us periodic tasks here if needed
            // e.g. button debounce, sensor polling, etc.
        }
    }
}

*/



