#include <Arduino.h>
#include <Wire.h>

/* * PROJECT: ESP32 PWM Generator (High Precision)
 * CORE VERSION: 3.3.5 (FlashESP.com / Arduino IDE)
 */

#define PWM_PIN 13       
#define LCD_ADDR 0x3F    
#define PWM_RES 10       

// Joystick Pins
const int PIN_UP = 5, PIN_DOWN = 18, PIN_LEFT = 19, PIN_RIGHT = 23, PIN_MID = 25;

// --- Chip Limits ---
#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)
  #define MAX_FREQ 78000  
  const char* chipName = "ESP32-S3/C3";
#else
  #define MAX_FREQ 78125  
  const char* chipName = "ESP32 Classic";
#endif

#define MIN_FREQ 20000
#define MIN_DUTY 1       
#define MAX_DUTY 400     

// --- Variables ---
uint32_t frequency = MIN_FREQ; 
uint32_t dutyRaw = MIN_DUTY;       
bool inMenu = false;
int menuIndex = 0; 
int editMode = 0;  
int digitPos = 0;  

// --- LCD Bit-Bang Driver ---
void lcdPulse(uint8_t data) {
  Wire.beginTransmission(LCD_ADDR);
  Wire.write(data | 0x04); Wire.endTransmission();
  delayMicroseconds(2);
  Wire.beginTransmission(LCD_ADDR);
  Wire.write(data & ~0x04); Wire.endTransmission();
  delayMicroseconds(50);
}

void lcdSend(uint8_t val, uint8_t mode) {
  uint8_t high = (val & 0xF0) | mode | 0x08; 
  uint8_t low = ((val << 4) & 0xF0) | mode | 0x08;
  Wire.beginTransmission(LCD_ADDR); Wire.write(high); Wire.endTransmission();
  lcdPulse(high);
  Wire.beginTransmission(LCD_ADDR); Wire.write(low); Wire.endTransmission();
  lcdPulse(low);
}

void lcdCommand(uint8_t cmd) { lcdSend(cmd, 0); }
void lcdWrite(uint8_t data) { lcdSend(data, 1); }

void lcdInit() {
  Wire.begin(21, 22);
  delay(100);
  for(int i=0; i<3; i++) { lcdCommand(0x03); delay(5); }
  lcdCommand(0x02); lcdCommand(0x28); lcdCommand(0x0C); lcdCommand(0x06); lcdCommand(0x01);
  delay(5);
}

void lcdSetCursor(int col, int row) { lcdCommand((row == 0 ? 0x80 : 0xC0) + col); }
void lcdPrint(const char* str) { while (*str) lcdWrite(*str++); }

// --- FIXED PWM UPDATE LOGIC ---
void updatePWM() {
  // Use ledcWriteTone to force a frequency change on the pin's timer
  ledcWriteTone(PWM_PIN, frequency);
  
  // Re-apply duty cycle because ledcWriteTone resets it to 50%
  ledcWrite(PWM_PIN, dutyRaw);
  
  Serial.printf("Hardware Updated -> Freq: %u Hz, Duty: %u\n", frequency, dutyRaw);
}

uint32_t modifyValue(uint32_t val, int pos, int dir, uint32_t minV, uint32_t maxV) {
  uint32_t multiplier = 1;
  for(int i=0; i<pos; i++) multiplier *= 10;
  int64_t nextVal = (int64_t)val + ((int64_t)dir * multiplier);
  if (nextVal < (int64_t)minV) return minV;
  if (nextVal > (int64_t)maxV) return maxV;
  return (uint32_t)nextVal;
}

void refreshDisplay() {
  lcdCommand(0x01); 
  delay(2);
  char buf[21];

  if (!inMenu) {
    lcdCommand(0x0C); 
    lcdSetCursor(0, 0);
    sprintf(buf, "F: %5u Hz", frequency); lcdPrint(buf);
    lcdSetCursor(0, 1);
    sprintf(buf, "D: %4u/1023", dutyRaw); lcdPrint(buf);
  } else {
    lcdSetCursor(0, 0);
    if (editMode != 0) {
      lcdPrint(editMode == 1 ? "EDIT FREQUENCY" : "EDIT DUTY");
      lcdSetCursor(0, 1);
      if (editMode == 1) {
        sprintf(buf, "F: %5u Hz", frequency); lcdPrint(buf);
        lcdSetCursor(7 - digitPos, 1); 
      } else {
        sprintf(buf, "D: %4u/1023", dutyRaw); lcdPrint(buf);
        lcdSetCursor(6 - digitPos, 1);
      }
      lcdCommand(0x0E); 
    } else {
      lcdCommand(0x0C); 
      lcdPrint("- "); lcdPrint(chipName); lcdPrint(" -");
      lcdSetCursor(0, 1);
      const char* labels[] = {"< Exit", "Edit Freq", "Edit Duty", "About"};
      lcdPrint("> "); lcdPrint(labels[menuIndex]);
    }
  }
}

void setup() {
  Serial.begin(115200);
  lcdInit();
  pinMode(PIN_UP, INPUT_PULLUP); pinMode(PIN_DOWN, INPUT_PULLUP);
  pinMode(PIN_LEFT, INPUT_PULLUP); pinMode(PIN_RIGHT, INPUT_PULLUP);
  pinMode(PIN_MID, INPUT_PULLUP);

  // Initial attach
  ledcAttach(PWM_PIN, frequency, PWM_RES);
  updatePWM();
  refreshDisplay();
}

void loop() {
  static uint32_t lastInput = 0;
  if (millis() - lastInput < 200) return;

  bool changed = false;

  if (digitalRead(PIN_MID) == LOW) {
    if (!inMenu) inMenu = true;
    else {
      if (editMode == 0) {
        if (menuIndex == 0) inMenu = false;
        else if (menuIndex == 1) { editMode = 1; digitPos = 0; }
        else if (menuIndex == 2) { editMode = 2; digitPos = 0; }
        else if (menuIndex == 3) { lcdCommand(0x01); lcdPrint("PWM Gen v1.7"); delay(1000); }
      } else editMode = 0;
    }
    changed = true; lastInput = millis();
  }

  if (digitalRead(PIN_UP) == LOW || digitalRead(PIN_DOWN) == LOW) {
    int dir = (digitalRead(PIN_UP) == LOW) ? 1 : -1;
    if (inMenu) {
      if (editMode == 1) { frequency = modifyValue(frequency, digitPos, dir, MIN_FREQ, MAX_FREQ); updatePWM(); }
      else if (editMode == 2) { dutyRaw = modifyValue(dutyRaw, digitPos, dir, MIN_DUTY, MAX_DUTY); updatePWM(); }
      else menuIndex = (menuIndex + (dir * -1) + 4) % 4;
      changed = true; lastInput = millis();
    }
  }

  if (digitalRead(PIN_LEFT) == LOW || digitalRead(PIN_RIGHT) == LOW) {
    if (editMode != 0) {
      int dir = (digitalRead(PIN_LEFT) == LOW) ? 1 : -1;
      int maxDigits = (editMode == 1) ? 4 : 3;
      digitPos = constrain(digitPos + dir, 0, maxDigits);
      changed = true; lastInput = millis();
    }
  }

  if (changed) refreshDisplay();
}