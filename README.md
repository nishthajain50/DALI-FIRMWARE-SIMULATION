# DALI Light Controller Firmware

## Overview
This firmware implements a DALI (Digital Addressable Lighting Interface) compliant LED dimmer for the LKS32MC033H6P8B microcontroller.

## Features
- ✅ DALI protocol support (IEC 62386 compliant)
- ✅ Manchester encoding/decoding
- ✅ PWM dimming (0-100% brightness)
- ✅ EEPROM emulation for settings persistence
- ✅ Bus-powered from DALI

## Hardware Requirements
| Component | Part Number | Purpose |
|-----------|-------------|---------|
| MCU | LKS32MC033H6P8B | Main processor |
| Optocoupler | 4N35 / LTV-817 | DALI isolation |
| Voltage Regulator | 78L05 | 16V → 5V |
| Debugger | J-Link | Programming |



## File Structure
├── main.c - Main program loop
├── dali_machine.c - Manchester encoding/decoding
├── dali_machine.h
├── dali_protocol.c - DALI command processing
├── dali_protocol.h
├── led_dimmer.c - PWM dimming control
├── led_dimmer.h
├── memory.c - EEPROM emulation
├── memory.h
├── hardware.h - Pin and register definitions
└── dali_lks32mc.uvprojx - Keil project file


## How to Build
1. Install Keil MDK v5 or later
2. Install LKS32MC03x device pack
3. Open `dali_lks32mc.uvprojx` in Keil
4. Press **F7** to build
5. Press **F8** to flash (with J-Link connected)

## Supported DALI Commands
| Command | Hex Value | Description |
|---------|-----------|-------------|
| OFF | 0x00 | Turn light off |
| UP | 0x01 | Increase brightness by 1 |
| DOWN | 0x02 | Decrease brightness by 1 |
| RECALL MAX | 0x05 | Set to 100% brightness |

## Testing in Simulation
1. Press **Ctrl+F5** to enter debug mode
2. Open **View → Serial Windows → UART #1**
3. Open **View → Watch Windows → Watch 1**
4. Add `DaliDevice.brightness` to watch
5. Press **F5** to run

## License
MIT License 

## Author
Nishtha jain
