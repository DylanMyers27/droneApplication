// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX

#include <SPI.h>
#include <RH_RF95.h>


#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 13

char serialData;
boolean isButton = false;

void setup()
{
  pinMode(10, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  while (!Serial) {
    delay(1);
  }
  delay(100);

//  Serial.println("Feather LoRa RX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
//  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
//    Serial.println("setFrequency failed");
    while (1);
  }
//  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void loop()
{
  isButton = digitalRead(10);
  if(Serial.available() > 0){
    serialData = Serial.read();
    Serial.print(serialData);
    }
  
 

    if(isButton == HIGH){
      uint8_t data[] = "Turn on pumps!";
//      Serial.println((char*)data);
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
//      Serial.println("Sending data");
      }
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) // this does receiving
    {
      digitalWrite(LED, HIGH);
//      RH_RF95::printBuffer("Received: ", buf, len);
//      Serial.print("Got: ");
      Serial.println((char*)buf); //25.7896,-80.2343,1300.00
//       Serial.print("RSSI: ");
//      Serial.println(rf95.lastRssi(), DEC);
      digitalWrite(LED, LOW);
    }
    
  
}

boolean buttonPressed(){
  delay(2000);
  boolean buttonState = digitalRead(10);
  Serial.println(buttonState);
  if (buttonState == HIGH){
    Serial.println("Setting button on");
      isButton = true;
    }
  return isButton;
}
