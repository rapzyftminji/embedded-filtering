# STM32 Embedded Filtering & Visualization

This project implements a real-time data acquisition and filtering system using an STM32 microcontroller and a Python-based visualization tool. It demonstrates how to implement complex digital signal processing (DSP) algorithms on a low-memory microcontroller while maintaining high performance.

## Project Structure

- **STM32 Source/**: Contains the firmware for the STM32L031K6Tx microcontroller.
  - `Core/Src/main.c`: Main application logic.
  - `Core/Src/filter.c`: Implementation of Butterworth filters (LPF, HPF, BPF, BSF) using optimized math approximations.
- **readSTM.py**: A Python script to read serial data from the STM32, plot the real-time signal, and display the Frequency Spectrum (FFT).
- **signal_generator.ino**: Arduino sketch for generating test signals.

## Features

- **Multi-Type Embedded Filtering**: Supports Low Pass (LPF), High Pass (HPF), Band Pass (BPF), and Band Stop (BSF) Butterworth filters.
- **Efficient Low-Memory Implementation**: Optimized for MCUs with limited resources, using approximation algorithms to avoid heavy standard library dependencies.
- **Real-Time Visualization**: Live plotting of ADC data using Matplotlib.
- **Frequency Analysis**: Real-time FFT display to analyze signal frequency components.

## Technical Implementation

The system is designed for deterministic real-time performance:

- **Sampling Protocol**: Uses a hardware timer interrupt to trigger sampling at exactly **1 kHz**, ensuring precise signal reconstruction.
- **ADC with DMA**: The Analog-to-Digital Converter (ADC) operates in **DMA Circular Mode**. This allows data to be transferred directly to memory without CPU intervention, freeing up the processor for filtering calculations.
- **UART Communication**: Data transmission is handled via **UART with Interrupts**, preventing blocking delays during serial communication.
- **Robustness**: Includes comprehensive **Error Handlers** within the HAL (Hardware Abstraction Layer) to manage peripheral failures gracefully.

## Hardware Specifications

The project is built on the **NUCLEO-L031K6** development board, featuring the ultra-low-power **STM32L031K6T6** microcontroller.

### Microcontroller (STM32L031K6T6)
- **Core**: ARM® Cortex®-M0+ 32-bit
- **Clock Frequency**: Up to 32 MHz
- **Flash Memory**: 32 KB
- **RAM**: 8 KB
- **EEPROM**: 1 KB
- **Supply Voltage**: 1.65 V to 3.6 V
- **I/O**: 25 GPIOs
- **Peripherals**: I²C, SPI, UART, USART, and 10-channel ADC

### Board Features (NUCLEO-L031K6)
- **Form Factor**: 32-pin package, Arduino™ Nano V3 compatible.
- **Debugger**: On-board ST-LINK/V2-1 debugger/programmer with Virtual COM port.
- **Power**: Flexible options (USB VBUS, external source).
- **User Interface**: 1 User LED, 1 Reset push-button.
- **Clock**: 24 MHz crystal oscillator.
- **Ecosystem**: Arm® Mbed Enabled™ compliant.

## Getting Started

### Prerequisites

- **Hardware**: STM32L031K6Tx board (or compatible), USB-UART bridge (if not built-in).
- **Software**: 
  - STM32CubeIDE (for firmware).
  - Python 3.x with `pyserial`, `matplotlib`, and `numpy`.

### Running the Python Interface

1. Install dependencies:
   ```bash
   pip install pyserial matplotlib numpy
   ```
2. Connect the STM32 board to your computer.
3. Run the script:
   ```bash
   python readSTM.py
   ```
   *Note: You may need to adjust `SERIAL_PORT` in `readSTM.py` to match your system (e.g., `COM3` on Windows or `/dev/ttyUSB0` on Linux).*

## Contributors

- @200dollarrbill
