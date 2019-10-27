/*********************************************************
Adafruit MPR121 12-channel Capacitive touch sensor
 nRF24l01 gamecontroller sketch
**********************************************************/

#include <SPI.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <nRF24L01.h>
#include <RF24.h>
#include <Joystick.h>

Joystick_ Joystick1(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  8, 0,                   // Button Count, Hat Switch Count
  true, true, false,      // X Axis , Y Axis, Z Axis
  false, false, false,    // Rotate X, Rotate Y, Rotate Z
  false, false,           // Rudder, Throttle
  false, false, false);   // Accelerator, Brake, Steering

RF24 radio24(19, 18);     // Pins for nRF24l01 for CE ad CSN
const byte rxAddr[][6] = {"0GAME","1GAME"};

Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

String nRFstr = "************ABC123";
              //"ABCDEFGHIJKLMNOPQRSTUVWXYZ789012";

struct dataStruct{
  uint16_t hexVal;
  char cVal[32];
}nRFdata;

void setup() {
  pinMode(4,OUTPUT);  digitalWrite(4, HIGH);      // Power for MPR121
  //while (!Serial){};        // needed to keep leonardo/micro from starting too fast!
  //delay(500);
  Serial.begin(115200);
     
  // Default address is 0x5A, if tied to 3.3V its 0x5B, if tied to SDA its 0x5C and if SCL then 0x5D
  // More information is found on the manufacturers website including examples
  Serial.println(F("\n\tStarting ...\nLooking for Cap.sensor, if test locks up check the wiring.."));
  if (!cap.begin(0x5A)) {
    Serial.println(F("MPR121 not found!"));
    while (1);
  }

  Serial.println(F("Looking for nRF24.."));
  if (!radio24.begin()) {
    Serial.println(F("nRF24l01 not found, check wiring!"));
    while (1);
  }
  
  Serial.println("Initializing...");
  // Initialize Joystick Library
  Joystick1.begin();
  Joystick1.setXAxisRange(-1, 1);
  Joystick1.setYAxisRange(-1, 1);
  
  radio24.setRetries(1, 5);  /* how many times the modem will retry to the send data in case of not receiving by another modem. The first argument sets how often modem will 
                        retry. It is a multiple of 250 microseconds. 15 * 250 = 3750. So, if the recipient does not receive data, modem will try to send them every 3.75 
                        milliseconds. Second argument is the number of attempts. So in our example, modem will try to send 15 times before it will stop and finds that the 
                        receiver is out of range, or is turned off.*/
  radio24.setChannel(75); // best in US 70-80 -- !! check the nRF24 infomration for what to use in your setup !! 
  radio24.openWritingPipe(rxAddr[0]);
  radio24.setPALevel(RF24_PA_LOW);
  radio24.enableDynamicPayloads();
  radio24.stopListening();  /*  switch the modem to data transmission mode. */
  radio24.printDetails();
}

  
void loop() {
  Serial.println("   . . .");
  // Get the currently touched pads
  currtouched = cap.touched();

  for (uint8_t i=0; i<12; i++) {
    // if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" touched");
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" released");
    }
  }
  
  nRFdata.hexVal = currtouched;

  for (int i=0; i < 32; i++) {
    nRFdata.cVal[i] = nRFstr.charAt(i);
  }
  
  checkTouching();
  checkCleared();
  
  Serial.println("\t0x"+(String(nRFdata.hexVal,HEX)));
/*
  Serial.print("Filt: ");
  for (uint8_t i=0; i<12; i++) {
  Serial.print(cap.filteredData(i)); Serial.print("\t");
  }
  Serial.println();
  Serial.print("Base: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.baselineData(i)); Serial.print("\t");
  }
  Serial.println();
*/

  Serial.print("Sending to nRF24...");
  Serial.println("\tb:"+String(nRFdata.hexVal,BIN));
  if (!radio24.write(&nRFdata, sizeof(nRFdata))){
    Serial.print("\tError sending data to nRF24...");
  }
  /*  First argument is an indication of the variable that stores the data to send. That’s why we used  “&” before the variable name, 
  so we can make an indicator from this variable. The second argument is the number of bytes that the radio will take from a variable to be sent.  */
  Serial.println("\tDone");
  Serial.print("\n\tc:");
  for (int i=0; i < 32; i++){
    Serial.print(nRFdata.cVal[i]);
  }
  Serial.println();
  Serial.println("\tPacket size: "+String(sizeof(nRFdata)));

  delay(25);
}

void checkTouching () {
  for ( uint8_t i=0; i < 12; i++) {
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.println("Clearing "+i);
      switch (i) {
        case 0: // UP
          nRFstr.setCharAt(i,'-');
          Joystick1.setYAxis(0);
          break;
        case 1: // DOWN
          nRFstr.setCharAt(i,'-');
          Joystick1.setYAxis(0);
          break;
        case 2: // LEFT
          nRFstr.setCharAt(i,'-');
          Joystick1.setXAxis(0);
          break;
        case 3: // RIGHT
          nRFstr.setCharAt(i,'-');
          Joystick1.setXAxis(0);
          break;
        default: // Buttons
          nRFstr.setCharAt(i,'-');
          Joystick1.setButton(i-4, 0);
          break;
      }
    }
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      switch (i) {
        case 0: // UP
          nRFstr.setCharAt(i,'+');
          Joystick1.setYAxis(-1);
          Serial.println("Pressing "+i);
          break;
        case 1: // DOWN
          nRFstr.setCharAt(i,'+');
          Joystick1.setYAxis(1);
          Serial.println("Pressing "+i);
          break;
        case 2: // LEFT
          nRFstr.setCharAt(i,'+');
          Joystick1.setXAxis(-1);
          Serial.println("Pressing "+i);
          break;
        case 3: // RIGHT
          nRFstr.setCharAt(i,'+'); // 42 = *
          Joystick1.setXAxis(1);
          Serial.println("Pressing "+i);
          break;
        default: // Buttons
          nRFstr.setCharAt(i,'+'); // 0x42 = B
          Joystick1.setButton(i-4, 1);
          Serial.println("Pressing "+i);
          break;
      }  
    }
  }
  lasttouched = currtouched;
}

// Under further development
void checkCleared(){
  radio24.startListening();
    if( radio24.available()){
      while (radio24.available()) {     // While there is data ready
        uint8_t RFlen = radio24.getDynamicPayloadSize();
        Serial.println("Received update");
        radio24.read( &nRFdata, RFlen); // Get the payload
      }
    }
  radio24.stopListening();
}
