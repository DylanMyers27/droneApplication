#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>



SoftwareSerial mySerial(9, 3); // GPS RX and TX
SoftwareSerial ecSerial(11,10); //EC circuit RX and TX
Adafruit_GPS GPS(&mySerial); //init GPS Module

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define GPSECHO  true
boolean usingInterrupt = false;
void useInterrupt(boolean);

String inputstring = "";                              //a string to hold incoming data from the PC
String sensorstring = "";                             //a string to hold the data from the Atlas Scientific product
boolean input_string_complete = false;                //have we received all the data from the PC
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product

boolean response = false;
float ecData[15];
int counter = 0;
void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  pinMode(6, OUTPUT); //intake pump
  pinMode(5, OUTPUT); // reservoir pump
  
  Serial.begin(115200);
  GPS.begin(9600);
  ecSerial.begin(9600);
  inputstring.reserve(10);                            //set aside some bytes for receiving data from the PC
  sensorstring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product
  
  //grabs NMEA sentences with relavent data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); 
  useInterrupt(true);

  delay(1000);


  // manual reset
  digitalWrite(RFM95_RST, LOW); 
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //logging information if connection fails
  while (!rf95.init()) {
    Serial.println("Connection can not be established");
    while (1);
  }
  Serial.println("Connection to testing site successful!");


  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  //setting the frequency to 915.0 MHz
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  rf95.setTxPower(23, false);
}
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}


void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
}


uint32_t timer = millis();

void loop()
{
  
  if(response == true){
  getGPS();    // get GPS 
  pumping();  // turn on pumps
  delay(1000);
  double nitrate = getSample(); // get sample data from EC
  delay(1000); // Wait 1 second between transmits, could also 'sleep' here!
//  Serial.println("Transmitting data..."); // Send a message to rf95_server
  
  float latitude = GPS.latitudeDegrees;
  char data[46];
  if(latitude >20.0){
    dtostrf(latitude, 3, 4, data); // placing latitude data in packet
  }
  float longitude = GPS.longitudeDegrees;
  data[7] = ',';
  
  dtostrf(longitude, 3, 4, data+8); // placing longitude data in packet
  
  data[16] = ',';
 dtostrf(nitrate,3,4, data + 17); // placing nitrate data in packet
  Serial.print("Sending: "); Serial.println(data);

  data[46] = 0;

  delay(10);
  rf95.send(data, 40);
  Serial.println("Waiting for packet to complete..."); 
  delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
   if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      response = true;
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
   Serial.println("No reply, is there a listener around?");
  }
   
  }
  response = false;

}

void getGPS(){
    // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 1000) { 
    if(GPS.fix){
    timer = millis(); // reset the timer
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.print(GPS.year, DEC); Serial.print(" ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.print(GPS.milliseconds);
    Serial.print(",");
    Serial.print(GPS.latitudeDegrees, 4);
    Serial.print(","); 
    Serial.print(GPS.longitudeDegrees, 4);
    Serial.println("");
    }     
  }
}

double getSample(){
  EC();
  delay(1000);  
  double sum = 0;
  int i = 0;
  while(ecData[i] != 0){
   sum = sum + ecData[i];
   i++;
  // Serial.println(sum);
  }
  double average = sum/i;
  Serial.print("The average is: ");
  Serial.println(average);
  clearECreadings();
  return average;    
}

void turnOnIntake(int time){
  digitalWrite(6, HIGH);
  Serial.print("Intake Pump On\n");
  delay(time);
}

void turnOffIntake (){
  digitalWrite(6, LOW);
  Serial.print("Intake Pump Off\n");
  
}

void turnOnSolution(int time){
  digitalWrite(5, HIGH);
  Serial.print("Solution Pump On\n");
  delay(time);
  
}

void turnOffSolution(){
  digitalWrite(5, LOW);
  Serial.print("Solution Pump Off\n");

}


void EC(){
  float time = millis();
  const float stopTime = time + 5000;

  
  while(time < stopTime){
  if (input_string_complete) {                        //if a string from the PC has been received in its entirety
    ecSerial.print(inputstring);                      //send that string to the Atlas Scientific product
    ecSerial.print('\r');                             //add a <CR> to the end of the string
    inputstring = "";                                 //clear the string
    input_string_complete = false;                    //reset the flag used to tell if we have received a completed string from the PC
  }

  if (ecSerial.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char inchar = (char)ecSerial.read();              //get the char we just received
    sensorstring += inchar;                           //add the char to the var called sensorstring
    if (inchar == '\r') {                             //if the incoming character is a <CR>
      sensor_string_complete = true;                  //set the flag
    }
  }


  if (sensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
    if (isdigit(sensorstring[0]) == false) {          //if the first character in the string is a digit
      Serial.println(sensorstring);                   //send that string to the PC's serial monitor
    }
    else                                              //if the first character in the string is NOT a digit
    {
     print_EC_data();                                //then call this function 
    }
    sensorstring = "";                                //clear the string
    sensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
  } 
  time = millis();
 }

}
 void print_EC_data(void) {                            //this function will pars the string  

  char sensorstring_array[30];                        //we make a char array
  char *EC;                                           //char pointer used in string parsing
 
  float f_ec;                                         //used to hold a floating point number that is the EC
  
  sensorstring.toCharArray(sensorstring_array, 30);   //convert the string to a char array 
  EC = strtok(sensorstring_array, ",");               //let's pars the array at each comma
 

  Serial.print("EC:");                                //we now print each value we parsed separately
  Serial.println(EC);                                 //this is the EC value

                                //this just makes the output easier to read
  
 f_ec= atof(EC);  //uncomment this line to convert the char to a float
 if(f_ec < 2000){
   ecData[counter] = f_ec;
  }else{
    counter--;
    }
 
 counter++;
}

void clearECreadings(){
  for(int i = 0; i < 15; i++){
    ecData[i] = 0;
  }
  counter = 0;
}

void pumping(){
    turnOnSolution(5000);
    turnOffSolution();
    turnOnIntake(5000);
    turnOffIntake();
    delay(100); 

  }
