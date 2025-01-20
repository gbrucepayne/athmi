#include <Arduino.h>
#ifdef LED_MULTICOLOR
#include <Adafruit_NeoPixel.h>
#endif

// LED configuration
#ifndef LED_PIN
#define LED_PIN 8
#endif
#ifndef LED_COUNT
#define LED_COUNT 1
#endif

// Modem configuration
#ifndef MODEM_RX_PIN
#define MODEM_RX_PIN 19
#define MODEM_TX_PIN 18
#endif
#define MODEM_BAUD_RATE 115200

#ifdef LED_MULTICOLOR
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
static uint8_t colorIndex = 0;
const uint8_t brightness = 25;
#else
static bool ledState = false;
#endif
const uint16_t blinkInterval = 1000; // LED blink in ms

HardwareSerial modemSerial(1); // avoid UART0 used for loading via USB

void toggleLed();

// Formats special characters for readability on debug
String formatSpecialChars(const String &input);

void setup() {
  // Initialize USB serial for user interaction
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for Serial to be ready
  }
  delay(1000);
  Serial.println("ESP32 AT Command Interface");

  // Initialize UART for modem communication
  modemSerial.begin(MODEM_BAUD_RATE, SERIAL_8N1);
  Serial.println("Modem serial initialized");

  // Initialize LED
#ifdef LED_MULTICOLOR
  strip.begin();
  strip.show();
#else
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
#endif
}

void loop() {
  static unsigned long lastBlinkTime = 0;
  static uint8_t colorIndex = 0;

  if (millis() - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = millis();
    toggleLed();
  }

  // Check for user input to send to modem
  if (Serial.available()) {
    String userInput = Serial.readStringUntil('\n');
    userInput.trim();

    if (userInput.length() > 0) {
      Serial.print("Sending to modem: ");
      Serial.println(formatSpecialChars(userInput + '\r'));
      delay(500);
      modemSerial.printf("%s\r", userInput);
    }
  }

  // Check for URC or modem response to print to console
  if (modemSerial.available()) {
    String modemResponse = modemSerial.readStringUntil('\n');
    if (modemResponse.length() > 0) {
      Serial.print("Modem response: ");
      Serial.println(formatSpecialChars(modemResponse + '\n'));
    }
  }
}

void toggleLed() {
#ifdef LED_MULTICOLOR
  colorIndex = (colorIndex + 1) % 3;
  uint32_t color;
  if (colorIndex == 0) {
    color = strip.Color(brightness, 0, 0);
  } else if (colorIndex == 1) {
    color = strip.Color(0, brightness, 0);
  } else {
    color = strip.Color(0, 0, brightness);
  }
  strip.setPixelColor(0, color);
  strip.show();
#else
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
#endif
}

String formatSpecialChars(const String &input) {
  String formatted = "";
  for (size_t i = 0; i < input.length(); i++) {
    char c = input[i];
    switch (c) {
      case '\r':
        formatted += "<cr>";
        break;
      case '\n':
        formatted += "<lf>";
        break;
      case '\b':
        formatted += "<bs>";
        break;
      case '\t':
        formatted += "<tab>";
        break;
      default:
        formatted += c;
    }
  }
  return formatted;
}
