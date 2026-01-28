# Project Functional Specification: ESP32 PWM Generator

## 1. Overview
This project defines a standalone, high-precision PWM signal generator utilizing the ESP32 microcontroller. The system provides a user-friendly interface via an I2C LCD and a 5-way joystick, allowing for real-time adjustment of frequency and duty cycle with 10-bit resolution.

---

## 2. Functional Requirements

### 2.1 PWM Generation
* **Resolution:** Fixed at 10-bit ($0$ to $1023$ integer steps).
* **Frequency Range:** * **Minimum:** 20,000 Hz (20 kHz).
  * **Maximum:** Chip-dependent (calculated to maintain 10-bit stability). 
    * Classic ESP32: 78,125 Hz.
    * ESP32-S3/C3: 78,000 Hz.
* **Duty Cycle Limits:**
  * **Minimum:** 1/1023 (Initial default).
  * **Maximum:** 400/1023 (Adjustable via firmware constant).
* **Real-time Updates:** Any change made in "Edit Mode" must be applied to the output GPIO immediately.

### 2.2 User Interface (LCD)
* **Hardware:** 20x2 Character LCD via PCF8574 I2C Backpack.
* **Default View:**
  * Line 1: `F: [Value] Hz`
  * Line 2: `D: [Value]/1023`
* **Menu View:**
  * Scrollable list featuring: `Exit`, `Edit Freq`, `Edit Duty`, and `About`.
* **Visual Feedback:** An underline cursor (`_`) must appear under the active digit during Edit Mode.

### 2.3 Navigation Logic
* **Menu Entry:** Center-click (Mid) to enter/exit the settings menu.
* **Digit-Wise Editing:** * **Left/Right:** Shift the cursor to select a specific decimal place (1s, 10s, 100s, etc.).
  * **Up/Down:** Increment or decrement only the selected digit.
* **Boundary Safety:** The system must prevent values from rolling over or exceeding the defined Min/Max limits.

---

## 3. Hardware Architecture

### 3.1 Component List
| Component | Specification |
| :--- | :--- |
| **MCU** | ESP32 DevKit V1 (or S3/C3 variant) |
| **Display** | 20x2 LCD with I2C Backpack (Address 0x3F or 0x27) |
| **Input** | 5-way Navigation Joystick (Active-Low) |
| **Power** | 5V via USB or VIN pin |

### 3.2 Pin Mapping


* **I2C SDA:** GPIO 21
* **I2C SCL:** GPIO 22
* **PWM Out:** GPIO 13
* **Joystick:** GPIOs 5, 18, 19, 23, 25 (Up, Down, Left, Right, Mid)

---

## 4. Software Design

### 4.1 Driver Implementation
* **I2C Communication:** Uses a bit-banged implementation to ensure compatibility with **FlashESP.com** and avoid external library dependencies.
* **PWM Engine:** Utilizes the ESP32 `ledc` peripheral for hardware-timed signal generation.

### 4.2 State Machine
The software operates in three primary states:
1. **Live View:** Monitoring mode showing current output.
2. **Menu Navigation:** Selecting which parameter to modify.
3. **Digit Edit:** High-precision modification of specific numerical values.



---

## 5. Non-Functional Requirements
* **Zero-Install:** Must be compatible with browser-based flashing tools.
* **Stability:** Frequency must remain jitter-free at the 10-bit resolution ceiling.
* **Reliability:** Debounce logic (200ms) on all physical inputs to prevent double-triggering.

---

## 6. Future Enhancements
* EEPROM/NVS integration to save settings between power cycles.
* Output enable/disable toggle via dedicated "Set" button.
