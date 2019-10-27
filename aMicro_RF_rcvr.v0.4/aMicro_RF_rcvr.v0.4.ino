/*********************************************************

**********************************************************
nRF24l01 reciever sketch
**********************************************************/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Joystick.h>

Joystick_ Joystick1(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  8, 0,                   // Button Count, Hat Switch Count
  true, true, false,      // X Axis , Y Axis, Z Axis
  false, false, false,    // Rotate X, Rotate Y, Rotate Z
  false, false,           // Rudder, Throttle
  false, false, false);   // Accelerator, Brake, Steering
  
RF24 radio24(19,18); //              Pins for nRF24l01 for CE ad CSN
const byte rxAddr[][6] = {"0GAME","1GAME","2GAME"};
uint8_t rxFrom;

struct nRFdataRcv{
  uint16_t hexVal;
  char cVal[32];
}nRFdata;

void setup() {
 // while (!Serial);        // needed to keep leonardo/micro from starting too fast!
  //delay(5000);
  Serial.begin(115200);

  if (!radio24.begin()) {
    Serial.println("nRF24l01 not found, check wiring?");
    while (1);
  }
  
  // Initialize Joystick Library
  Joystick1.begin();
  Joystick1.setXAxisRange(-1, 1);
  Joystick1.setYAxisRange(-1, 1);
  
  radio24.setRetries(1, 5);  /* How many times the modem will retry to the send data in case of not receiving by another modem. The first argument sets how often modem will 
                        retry. It is a multiple of 250 microseconds. 15 * 250 = 3750. So, if the recipient does not receive data, modem will try to send them every 3.75 
                        milliseconds. Second argument is the number of attempts. So in our example, modem will try to send 5 times before it will stop and finds that the 
                        receiver is out of range, or is turned off.*/
  radio24.setChannel(75); // best in US 70-80 -- !! check the nRF24 infomration for what to use in your setup !! 
  radio24.openReadingPipe(0,rxAddr[0]);
  radio24.openReadingPipe(1,rxAddr[1]);
  radio24.openReadingPipe(2,rxAddr[2]);
  radio24.setPALevel(RF24_PA_LOW);
  radio24.enableDynamicPayloads();
  radio24.startListening();

  Serial.println("starting...");
  radio24.printDetails();
}

void loop() {
  Serial.println("..");
  
  if(radio24.available()){
    while (radio24.available(&rxFrom)) { // While there is data ready
      uint8_t RFlen = radio24.getDynamicPayloadSize();
      Serial.println("\nRX from: "+String(rxFrom)+"  packet length:"+String(RFlen));
      radio24.read( &nRFdata, RFlen); // Get the payload
    }
    
    Serial.println("\n\t0x"+String(nRFdata.hexVal,HEX)+"\t\t= "+String(nRFdata.hexVal));
    Serial.println("\tb:"+String(nRFdata.hexVal,BIN));
    Serial.print("\tc:");
    for (int i=0; i < 32; i++){
      if (!nRFdata.cVal[i]){
        Serial.print('.');
        //return;
      }
      Serial.print(nRFdata.cVal[i]);
    }
    
    for ( uint8_t i=0; i < 12; i++) {
      bool isTouched = false;
      if (nRFdata.cVal[i]=='+') {
        isTouched = true; 
      } else {
      isTouched = false; }
  
      bool isReleased = false;
      if (nRFdata.cVal[i]=='-') {
        isReleased = true; 
      } else {
        isReleased = false;
      }
      
      if (isTouched) {
  //      Serial.println("Pressing "+i);
        switch (i) {
          case 0: // UP
            Joystick1.setYAxis(-1);
          case 1: // DOWN
            Joystick1.setYAxis(1);
          case 2: // LEFT
            Joystick1.setXAxis(-1);
          case 3: // RIGHT
            Joystick1.setXAxis(1);
          default: // Buttons
            Joystick1.setButton(i-4, 1);
        }  
      }
      if (isReleased/*!(nRFdata.hexVal(i)) && (touchState(i))*/) {
  //      Serial.println("Clearing "+i);
        switch (i) {
          case 0: // UP
            if (nRFdata.cVal[i+1]=='+'){break;}
            Joystick1.setYAxis(0);
          case 1: // DOWN
            if (nRFdata.cVal[i-1]=='+'){break;}
            Joystick1.setYAxis(0);
          case 2: // LEFT
            if (nRFdata.cVal[i+1]=='+'){break;}
            Joystick1.setXAxis(0);
          case 3: // RIGHT
            if (nRFdata.cVal[i-1]=='+'){break;}
            Joystick1.setXAxis(0);
          default: // Buttons
            Joystick1.setButton(i-4, 0);
        } 
      }
    }
  }

  // put a delay so it isn't so overwhelming
  //delay(5);
  Serial.println(",,");
}

