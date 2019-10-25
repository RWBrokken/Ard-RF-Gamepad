#include <avr/power.h>
#include <CapacitiveSensor.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

CapacitiveSensor cs_7_8 = CapacitiveSensor(7,8);
CapacitiveSensor cs_7_6 = CapacitiveSensor(7,6);
CapacitiveSensor cs_7_5 = CapacitiveSensor(7,5);

RF24 radio24(17, 16);     // Pins for nRF24l01 for CE ad CSN
const byte rxAddr[][6] = {"0GAME","1GAME","2GAME"};

String nRFstr = "************ABC123";
              //"ABCDEFGHIJKLMNOPQRSTUVWXYZ789012";

struct dataStruct{
  uint16_t hexVal;
  char cVal[32];
}nRFdata;

// the setup function runs once when you press reset or power the board
void setup() {
  ///while(!Serial);
  Serial.begin(19200);
////  clock_prescale_set(clock_div_1); // set internal clockt0 20Mhz
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(9, OUTPUT);
  // push button in, grounding
 /// pinMode(2, INPUT_PULLUP);

//  if (!radio24.begin()) {
//    while (1){
//      Serial.println(F("nRF24l01 not found, check wiring!"));
//    };
//  }

}

// the loop function runs over and over again forever
void loop() {
  long tTouch8 = cs_7_8.capacitiveSensor(30);
  Serial.println(int(tTouch8));
  long tTouch6 = cs_7_6.capacitiveSensor(30);
  Serial.println(int(tTouch8));
  long tTouch5 = cs_7_5.capacitiveSensor(30);
  Serial.println(int(tTouch8));

  
  if (tTouch8 > 35) { 
    digitalWrite(9, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println(int(1));
    delay(100);                       // wait for a second
    digitalWrite(9, LOW);    // turn the LED off by making the voltage LOW
      Serial.println(int(0));
    delay(100);                       // wait for a second
  } else if (tTouch6 > 35) { 
    digitalWrite(9, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println(int(1));
    delay(100);                       // wait for a second
    digitalWrite(9, LOW);    // turn the LED off by making the voltage LOW
      Serial.println(int(0));
    delay(100);                       // wait for a second
  } else if (tTouch5 > 35) { 
    digitalWrite(9, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println(int(1));
    delay(100);                       // wait for a second
    digitalWrite(9, LOW);    // turn the LED off by making the voltage LOW
      Serial.println(int(0));
    delay(100);                       // wait for a second
  } else {
    digitalWrite(9, LOW);
  }
  //delay(500);
}
