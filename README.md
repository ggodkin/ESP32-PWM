# ESP32 PWM Generator (High Precision)
### Adjustable Frequency & Duty Cycle with Joystick UI

A standalone, industrial-grade PWM (Pulse Width Modulation) generator designed for precise electronic testing. This firmware is optimized for **ESP32 Core 3.3.5** and features a zero-dependency I2C driver for easy flashing via **FlashESP.com**.



---

## 🚀 Key Features
* **Dynamic Hardware Detection:** Automatically sets the frequency ceiling based on the ESP32 chip variant (Classic, S3, or C3).
* **10-Bit Precision:** Provides 1024 discrete steps of duty cycle control.
* **Digit-Wise Editing:** Use the joystick to navigate individual digits for rapid frequency and duty cycle tuning.
* **Zero-Dependency Code:** Custom bit-banged I2C driver avoids common library version conflicts.
* **Safety Clamps:** Hardcoded limits prevent hardware over-stress:
    * **Min Frequency:** 20 kHz
    * **Duty Cycle Range:** 1/1023 to 400/1023

---

## 🛠 Hardware Connections

| Component      | ESP32 Pin | Function               |
| :------------- | :-------- | :--------------------- |
| **PWM Output** | GPIO 13   | Signal Output          |
| **LCD SDA** | GPIO 21   | I2C Data (Needs 5V)    |
| **LCD SCL** | GPIO 22   | I2C Clock              |
| **Joy Up** | GPIO 5    | Increase Value / Up    |
| **Joy Down** | GPIO 18   | Decrease Value / Down  |
| **Joy Left** | GPIO 19   | Move Cursor Left       |
| **Joy Right** | GPIO 23   | Move Cursor Right      |
| **Joy Mid** | GPIO 25   | Enter / Save / Select  |

> **Note:** Connect LCD VCC to the **VIN** pin (5V) and GND to GND. If text is not visible, adjust the contrast potentiometer on the back of the LCD backpack.

---

## 🎮 Navigation & Operation

### Home Screen (Live View)
Displays real-time output. 
* **Row 1:** Frequency in Hz.
* **Row 2:** Duty cycle as a fraction (X / 1023).

### Menu System
1.  Click **Joystick Middle** to enter the menu.
2.  Use **Up/Down** to highlight a parameter.
3.  Click **Middle** to enter **Edit Mode**.

### Editing Values (Digit Control)
* **Left/Right:** Moves the underline cursor beneath a specific digit.
* **Up/Down:** Increments or decrements the selected digit.
* **Middle:** Saves the value and returns to the menu.



---

## 📋 Technical Specs
* **Clock Source:** 80 MHz APB Clock.
* **Frequency Ceiling:** Calculated as $f_{max} = \frac{f_{clock}}{2^{resolution}}$. For 10-bit, the limit is **78.125 kHz**.
* **Input Logic:** Active-Low (Pull-up resistors enabled internally).
* **Serial Debug:** 115200 baud.

---

## 🔧 Installation
1.  Connect your ESP32 to your computer.
2.  Navigate to [FlashESP.com](https://flashesp.com).
3.  Paste the source code into the editor.
4.  Select your ESP32 port and click **Flash**.

---

© 2026 - Developed for precision prototyping.
