#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_seesaw.h"

// Screen Stuff
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//Capread min and max
#define CAPREADMAX 1020
#define CAPREADMIN 310

Adafruit_SSD1306 newDisplay;
Adafruit_seesaw newSoil;
int state = 0;
bool lightOn = false;
bool buttonWasPressed = false;
bool watering = false;

void setup() {
    pinMode(9, OUTPUT);
    digitalWrite(9, LOW);
    pinMode(8, OUTPUT);
    digitalWrite(8, LOW);
    pinMode(7, INPUT);
    newSoil = setupSoil();
    newDisplay = setupDisplay();
}

void loop() {
    float capread = newSoil.touchRead(0);
    uint16_t temp = newSoil.getTemp();

    switch (state) {
        // Display data
        case 0:
          printData(temp, capread, lightOn);
          if (capread < 500) {
            state = 1;
          }
          break;
        // Spew water
        case 1:

          if (watering == false) {
            digitalWrite(9, HIGH);
            watering = true;
            newDisplay.clearDisplay();
            newDisplay.setCursor(0,0);
            newDisplay.println();
            newDisplay.println();
            newDisplay.println("Watering the plant!");
            newDisplay.display();
          }

          if(newSoil.touchRead(0) > 900 && watering == true) {
            digitalWrite(9, LOW);
            newDisplay.clearDisplay();
            newDisplay.setCursor(0,0);
            newDisplay.println();
            newDisplay.println();
            newDisplay.println("Done watering.");
            newDisplay.display();
            delay(1000);
            watering = false;
            state = 0;
          }
          break;
    }

    if (digitalRead(7) == HIGH && buttonWasPressed == false) {
      if (lightOn) {
          digitalWrite(8, LOW);
          lightOn = false;
      } else {
          digitalWrite(8, HIGH);
          lightOn = true;
      }
      buttonWasPressed = true;
    }

    if (digitalRead(7) == LOW) {
      buttonWasPressed = false;
    }
}

void printData(float temp, uint16_t waterLevel, bool lighton) {
  
  float waterpercent = (float(waterLevel - CAPREADMIN) / float(CAPREADMAX - CAPREADMIN)) * 100.0;
  newDisplay.clearDisplay();
  newDisplay.setCursor(0,0);
  if (lighton) {
      newDisplay.println("Light is on!");
  } else {
      newDisplay.println("Light is off.");
  }
  
  newDisplay.println();
  newDisplay.print("Temperature: ");
  newDisplay.print(temp);
  newDisplay.println(" C");
  newDisplay.print("Water Level: ");
  newDisplay.print(waterpercent);
  newDisplay.println("%");
  newDisplay.display();

}

Adafruit_SSD1306 setupDisplay() {
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true); 

  return display;
}

Adafruit_seesaw setupSoil() {
  Adafruit_seesaw soil;

  if (!soil.begin(0x36)) {
    Serial.println("ERROR! Soil Sensor Not Found");
    while(1);
  } else {
    Serial.print("Soil Sensor Found");
  }

  return soil;
}