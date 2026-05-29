# DALI Light Controller - Simulation Firmware

[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/platform-Keil-orange.svg)](https://www.keil.com/)
[![Simulation](https://img.shields.io/badge/simulation-Keil_Simulator-brightgreen.svg)](https://www.keil.com/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

## 📖 Overview

This project implements a **complete DALI (Digital Addressable Lighting Interface) compliant LED dimmer** that runs entirely in **software simulation** using Keil µVision. **No hardware required!**

You can test, debug, and validate your DALI protocol implementation completely on your computer before moving to actual hardware.

## ✨ Features

| Feature | Status | Description |
|---------|--------|-------------|
| DALI Protocol (IEC 62386) | ✅ | Complete command set implementation |
| Manchester Encoding | ✅ | Physical layer encoding/decoding |
| PWM Dimming Simulation | ✅ | 0-254 brightness levels |
| EEPROM Emulation | ✅ | Settings persist in simulation |
| UART Debug Output | ✅ | Real-time printf messages |
| Watch Window Integration | ✅ | Monitor variables live |
| Logic Analyzer Support | ✅ | View Manchester waveforms |

## 📋 Supported DALI Commands

| Command | Hex | Binary | Description |
|---------|-----|--------|-------------|
| OFF | 0x00 | 00000000 | Turn light off |
| UP | 0x01 | 00000001 | Increase brightness by 1 |
| DOWN | 0x02 | 00000010 | Decrease brightness by 1 |
| RECALL MAX | 0x05 | 00000101 | Set to 100% brightness |
| RECALL MIN | 0x06 | 00000110 | Set to 0% brightness |

## 🖥️ System Requirements

| Component | Requirement |
|-----------|-------------|
| **IDE** | Keil µVision v5 or later (any edition) |
| **Device** | Any ARM Cortex-M0 (simulation mode) |
| **RAM** | 4KB minimum |
| **Flash** | 32KB minimum |
| **OS** | Windows 7/8/10/11 |

> ⚠️ **No hardware needed!** This is a pure software simulation.

## 🚀 How to Run the Simulation

### Step 1: Open the Project
1. Launch Keil µVision
2. Open `dali_lks32mc.uvprojx`
3. Ensure device is set to any Cortex-M0 (simulation)

### Step 2: Configure for Simulation
1. Press **Alt+F7** (Target Options)
2. Go to **Debug** tab
3. Select **Use Simulator**
4. Set **CPU DLL**: `SARMCM3.DLL`
5. Set **Parameter**: `-pCM0`
6. Click **OK**

### Step 3: Build the Project

### Step 4: Start Debugging

### Step 5: Open Debug Windows

| Window | How to Open | Purpose |
|--------|-------------|---------|
| **UART #1** | View → Serial Windows → UART #1 | See printf output |
| **Watch 1** | View → Watch Windows → Watch 1 | Monitor variables |
| **Logic Analyzer** | View → Analysis Windows → Logic Analyzer | View waveforms |

### Step 6: Add Variables to Watch Window

Right-click in Watch 1 and add:

| Variable | Type | Description |
|----------|------|-------------|
| `DaliDevice.brightness` | uint8_t | Current brightness (0-254) |
| `DaliDevice.target_brightness` | uint8_t | Target during fade |
| `DaliDevice.power_on` | uint8_t | Power state (0/1) |
| `recive_data` | uint8_t | DALI data received flag |

### Step 7: Run the Simulation

The auto-test sequence will automatically send test commands.

## 📊 Expected Output

### UART #1 Window (Debug Output)
DALI Light Dimmer - SIMULATION MODE
========================================

System Ready! DALI Address: 1
Initial Brightness: 0/254

TEST 1: RECALL MAX
SIM: Processing command 0x05
SIM: LED Brightness = 254/254 (100%)
Current Brightness: 254/254

TEST 2: UP (10 times)
SIM: Brightness UP to 255
SIM: Brightness UP to 256
...
Current Brightness: 255/254

TEST 3: DOWN (5 times)
SIM: Brightness DOWN to 254
...
Current Brightness: 250/254

TEST 4: OFF
SIM: Brightness = 0/254 (0%)

text

### Watch Window Values

| Step | brightness | power_on |
|------|------------|----------|
| Start | 0 | 0 |
| After RECALL MAX | 254 | 1 |
| After UP x10 | 255 | 1 |
| After DOWN x5 | 250 | 1 |
| After OFF | 0 | 0 |

### Logic Analyzer Waveforms

Add `PORTA.0` to see Manchester-encoded DALI signals:
DALI RX (PORTA.0)
HIGH ───┐ ┌─┐ ┌─┐
│ │ │ │ │
LOW ───┘────┘ └────┘ └──────────

text

## 📁 File Structure
DALI-Firmware/
├── src/
│ ├── main.c - Main program loop
│ ├── dali_machine.c - Manchester encoding/decoding
│ ├── dali_machine.h
│ ├── dali_protocol.c - DALI command handling
│ ├── dali_protocol.h
│ ├── led_dimmer.c - PWM dimming simulation
│ ├── led_dimmer.h
│ ├── memory.c - EEPROM emulation
│ ├── memory.h
│ └── hardware.h - Pin and register definitions
├── Keil/
│ └── dali_lks32mc.uvprojx - Keil project file
├── README.md - This file
├── .gitignore - Git ignore rules
└── LICENSE - MIT License



## 🔧 Code Structure Explanation

| Module | Responsibility |
|--------|---------------|
| **dali_machine.c** | Converts bits to/from Manchester encoding; Simulates GPIO for DALI RX/TX |
| **dali_protocol.c** | Parses DALI commands (OFF, UP, DOWN, RECALL MAX); Manages device addressing |
| **led_dimmer.c** | Simulates PWM dimming; Converts brightness values (0-254 → 0-1000) |
| **memory.c** | Simulates EEPROM for saving address and brightness settings |
| **main.c** | Coordinates everything; Runs auto-test sequence |
This project is licensed under the MIT License - see the LICENSE file for details.
AUTHOR-NISHTHA JAIN
GitHub:@nishthajain50 


