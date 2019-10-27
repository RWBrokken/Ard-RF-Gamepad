/* A variation of a bluetooth basic computer gamepad using the Adafruit Bluefruit board */
/* In part based off of the CapitiveSense Library Demo Sketch by Paul Badger 2008 */
/* Coded by Richard B. 2017, last modified Oct 2019 */

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "BluefruitConfig.h"
//#include "Keyboard.h"
//#include "Joystick.h"
#include <CapacitiveSensor.h>

// 10 megohm resistor between pins 12 & 21, pin 21 is sensor pin, add wire and foil for touch pad
CapacitiveSensor cs_12_21 = CapacitiveSensor(12,21);
CapacitiveSensor cs_12_20 = CapacitiveSensor(12,20);
CapacitiveSensor cs_12_19 = CapacitiveSensor(12,19);
CapacitiveSensor cs_12_18 = CapacitiveSensor(12,18);
CapacitiveSensor cs_12_2 = CapacitiveSensor(12,2);
CapacitiveSensor cs_12_3 = CapacitiveSensor(12,3);

// Setup the Gamepad buttons
//   buttons currently as connected - by pin number
int btnUp =    10;
int btnDown =  11;
int btnLeft =   5;
int btnRight =  6;
int btnA =     22; // A4 pin
int btnB =     23; // A5 pin
int buttonPins[6] = {btnUp, btnDown, btnLeft, btnRight, btnA, btnB};
int lastButtonState[6] = {0,0,0,0,0,0};
int numOfBtns = sizeof(buttonPins);
int ctrlAxisX, ctrlAxisY, ctrlBtnsPressed = 0;
bool touchS1, touchS2, touchS3, touchS4, touchS5, touchS6;
bool touchPrev1, touchPrev2, touchPrev3, touchPrev4, touchPrev5, touchPrev6;
String bleSigOut = "";

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

void setupBluefruitDevice() {
  ble.begin(true);
//  ble.echo(false);
  ble.sendCommandCheckOK(F( "AT+GAPDEVNAME=RWB Bluefruit Gamepad V0.5" ));
  /* Enable HID Service */
  Serial.println(F("Enable HID Service... "));
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    if (!ble.sendCommandCheckOK(F( "AT+GATTCLEAR" ))) {
      error(F("Could not clear any previous GATT services/characteristics"));
    }
    Serial.println(" -- ENABLING --");
    ble.sendCommandCheckOK(F( "AT+BLEHIDEN=1" ));
    if (!ble.sendCommandCheckOK(F( "AT+BLEHIDGAMEPADEN=1" ))) {
      error(F("Could not enable Gamepad"));
    } else {
      //ble.sendCommandCheckOK(F( "AT+BLEKEYBOARDEN=0"));
      ble.sendCommandCheckOK(F( "AT+BLEMOUSE=0"));
    }
  } else {error(F("Please check fireware and ensure is >= 0.7.7"));}
  /* Add or remove service requires a reset */
  //Serial.println(F("Performing a SW reset (service changes require a reset)"));
  if (! ble.reset() ) {
    error(F("Couldn't reset??"));
  }
}

void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/*
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  2, 0,                   // Button Count, Hat Switch Count
  true, true, false,      // X Axis , Y Axis, Z Axis
  false, false, false,    // Rotate X, Rotate Y, Rotate Z
  false, false,           // Rudder, Throttle
  false, false, false);   // Accelerator, Brake, Steering
*/

void setup() {
  startupLED();
  delay(500); 
  Serial.begin(115200);
  Serial.println("   ---   LOADING   ---");

  setupBluefruitDevice();

  pinMode(btnUp, INPUT_PULLUP);
  pinMode(btnDown, INPUT_PULLUP);
  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);
  pinMode(btnA, INPUT_PULLUP);
  pinMode(btnB, INPUT_PULLUP);

  pinMode(13,OUTPUT);

  // Capacitive touch
  //cs_12_21.set_CS_AutocaL_Millis(0xFFFFFFFF); // turn off autocalibrate on channel 1 - just as an example
}

void startupLED() {
//  while (!Serial) { // required for Flora & Micro
//  }
///  do{
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    //analogWrite(A0, .5);
    delay(750);
    digitalWrite(13, LOW);
    //analogWrite(A0, 0);
    delay(600);
    digitalWrite(13, HIGH);
    delay(250);
    digitalWrite(13, LOW);
    delay(400);
///  }
  Serial.println("Starting... ");
}

void loop() {
  ctrlAxisX, ctrlAxisY, ctrlBtnsPressed = 0;
  String bleHEXprep = "";
  Serial.println("Checking...");
  checkTouchButtons();
  checkButtonsPressed();
  Serial.println("Found ->> " + String(ctrlAxisX) + "," + String(ctrlAxisY) + "," + ctrlBtnsPressed);

// Prep and send the values to the host device over bluetooth
  bleHEXprep = String(ctrlBtnsPressed, HEX);
  if (bleHEXprep.length() < 2) {
    bleHEXprep = ("0x0" + bleHEXprep);
  } else {
    bleHEXprep = ("0x" + bleHEXprep);
  }
  bleSigOut = ("AT+BLEHIDGAMEPAD=" + String(ctrlAxisX) + "," + String(ctrlAxisY) + "," + bleHEXprep);
  ble.println(String (bleSigOut));

  delay(50); // debounce
}

void checkTouchButtons() {
  // capacitive touch
  long total1 = cs_12_21.capacitiveSensor(30);
  long total2 = cs_12_19.capacitiveSensor(30);
  long total3 = cs_12_20.capacitiveSensor(30);
  long total4 = cs_12_18.capacitiveSensor(30);
  long total5 = cs_12_2.capacitiveSensor(30);
  long total6 = cs_12_3.capacitiveSensor(30);
  Serial.println("\tT1: "+ String(total1)+"\tT2: "+ String(total2)+"\tT3: "+ String(total3)+"\tT4:  "+ String(total4)+"\tT5:  "+ String(total5)+"\tT6:  "+ String(total6));
  if ( total1 > 35 ) {
    touchS1=true;
  } else { touchS1=false; }
  if ( total2 > 70 ) {
    touchS2=true;
  } else { touchS2=false; }
  if ( total3 > 35 ) {
    touchS3=true;
  } else { touchS3=false; }
  if ( total4 > 35 ) {
    touchS4=true;
  } else { touchS4=false; }
  if ( total5 > 35 ) {
    touchS5=true;
  } else { touchS5=false; }
  if ( total6 > 35 ) {
    touchS6=true;
  } else { touchS6=false; }

// A visual output on the microcontroller to show that a touch was detected in this loop
  if (touchS1 || touchS2 || touchS3 || touchS4 || touchS5 || touchS6) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);      
  }
  // Output over serial to show what was detected
  Serial.print("   Boolean: "+String(touchS1)+"\t    "+String(touchS2)+"\t    "+String(touchS3)+"\t    "+String(touchS4)+"\t    "+String(touchS5)+"\t    "+String(touchS6)+"\t    "+"\n\n");
}

void checkButtonsPressed() {
  for (int index = 0; index < 6; index++) {
    int currentButtonState = !digitalRead(buttonPins[index]);
    if (currentButtonState != lastButtonState[index]) {
      switch (index) {
        case 0: // UP
          if (currentButtonState == 1) {
            ctrlAxisY = -1;
          } else { 
            ctrlAxisY = 0;
          }
          break;
        case 1: // DOWN
          if (currentButtonState == 1) {
            ctrlAxisY = 1;
          } else { 
            if (ctrlAxisY != -1) { 
              ctrlAxisY = 0;
            }
          }
          break;
        case 2: // LEFT
          if (currentButtonState == 1) {
            ctrlAxisX = -1;
          } else { 
            ctrlAxisX = 0;
          }
          break;
        case 3: // RIGHT
          if (currentButtonState == 1) {
            ctrlAxisX = 1;
          } else { 
            if (ctrlAxisX != -1) { 
              ctrlAxisX = 0;
            }
          }
          break;
        case 4: // Button 1
          if (currentButtonState == 1) {
            ctrlBtnsPressed += 1;
          } else {
            ctrlBtnsPressed -= 1; 
          }
          break;
        case 5: // Button 2
          if (currentButtonState == 1) {
            ctrlBtnsPressed += 2;
          } else {
            ctrlBtnsPressed -= 2; 
          }
          break;
        default:
          break;
      }
      lastButtonState[index] = currentButtonState;
    } 
  }

  if (touchS1 && !touchPrev1) {
    ctrlBtnsPressed += 4;
    touchPrev1 = true;
  }
  if (!touchS1 && touchPrev1) {
    ctrlBtnsPressed -= 4;
    touchPrev1 = false;
  }
  if (touchS2 && !touchPrev2) {
    ctrlBtnsPressed += 8;
    touchPrev2 = true;
  }
  if (!touchS2 && touchPrev2) {
    ctrlBtnsPressed -= 8;
    touchPrev2 = false;
  }
  if (touchS3 && !touchPrev3) {
    ctrlBtnsPressed += 16;
    touchPrev3 = true;
  }
  if (!touchS3 && touchPrev3) {
    ctrlBtnsPressed -= 16;
    touchPrev3 = false;
  }

  if (touchS4 && !touchPrev4) {
    ctrlBtnsPressed += 32;
    touchPrev4 = true;
  }
  if (!touchS4 && touchPrev4) {
    ctrlBtnsPressed -= 32;
    touchPrev4 = false;
  }
  if (touchS5 && !touchPrev5) {
    ctrlBtnsPressed += 64;
    touchPrev5 = true;
  }
  if (!touchS5 && touchPrev5) {
    ctrlBtnsPressed -= 64;
    touchPrev5 = false;
  }
    if (touchS6 && !touchPrev6) {
    ctrlBtnsPressed += 128;
    touchPrev6 = true;
  }
  if (!touchS6 && touchPrev6) {
    ctrlBtnsPressed -= 128;
    touchPrev6 = false;
  }
}
