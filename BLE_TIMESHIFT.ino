//--------------------------------------------------------------------------------------------------------------------------
// Created by Kaan Raif 4/24/217
// 
// Purpose: Create BLE app that reads in time of phone and modifies it 
// Displays messages and time through LCD Screen
// 
// Revisions:None
//
// Wiring:
//  LCD (HD44780 16X2 Display Module)
//      RS<=>8 E<=>7 D4<=>6 D4<=>5 D5<=>4 D6<=>3 D7<=>2
//      VSS<=>GND VDD<=>5V RW<=>GND A<=>5V K<=>GND
//   
//  Bluetooth Module (nRF8001 v1.1)
//      SCK<=>13 MISO<=>12 MOSI<=>11 REQ<=>10 RST<=>9 RDY<=>2 VIN<=>5V
//
//---------------------------------------------------------------------------------------------------------------------------
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 7, 6, 5, 4, 3);

#include <SPI.h>
#include "Adafruit_BLE_UART.h"

// Connect CLK/MISO/MOSI to hardware SPI
// e.g. On UNO & compatible: CLK = 13, MISO = 12, MOSI = 11
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2     // This should be an interrupt pin, on Uno thats #2 or #3
#define ADAFRUITBLE_RST 9


Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

 
void setup(void)
{ 
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("Hello!");

  // initize serial 
  Serial.begin(9600);
  while(!Serial); // Leonardo/Micro should wait for serial init
  Serial.println(F("Adafruit Bluefruit Low Energy nRF8001 Enter/Recieve Message"));

  BTLEserial.setDeviceName("TimeShift"); // Name Bluetooth device, 7 characters max
  BTLEserial.begin();
  
}


  
aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;

void loop()

{

    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print(millis() / 1000);    
    

    // Tell the nRF8001 to do whatever it should be working on.
    BTLEserial.pollACI();

    // Ask what is our current status
    aci_evt_opcode_t status = BTLEserial.getState();
    
    // If the status changed....
  if (status != laststatus) 
  {
    
      // print it out!
    if (status == ACI_EVT_DEVICE_STARTED)
    
    {
      
        Serial.println(F("* Advertising started"));
        
    }
    
    if (status == ACI_EVT_CONNECTED) 
    {
      
        Serial.println(F("* Connected!"));
        
    }
    
    if (status == ACI_EVT_DISCONNECTED)
    {
      
        Serial.println(F("* Disconnected or advertising timed out"));
        
    }
    
      // OK set the last status change to this one
    laststatus = status;
  }

  if (status == ACI_EVT_CONNECTED) 
  {
    
      // Lets see if there's any data for us!
    if (BTLEserial.available()) 
    {
      
      Serial.print("* "); Serial.print(BTLEserial.available()); Serial.println(F(" bytes available from BTLE"));
    
    }
    
      // OK while we still have something to read, get a character and print it out
    while (BTLEserial.available())
    {

      // Read from bluetooth and print on serial
      char c = BTLEserial.read();
      Serial.print(c);
      
    }

    // Next up, see if we have any data to get from the Serial console

    if (Serial.available()) 
    {
      
      // Read from Serial
      Serial.setTimeout(100); // 100 millisecond timeout
      String s = Serial.readString();
      lcd.clear();
      lcd.print(s);

      // We need to convert the line to bytes, no more than 20 at this time
      uint8_t sendbuffer[20];
      s.getBytes(sendbuffer, 20);
      char sendbuffersize = min(20, s.length());

      Serial.print(F("\n* Sending -> \"")); Serial.print((char *)sendbuffer); Serial.println("\"");

      // write the data
      BTLEserial.write(sendbuffer, sendbuffersize);
      
    }
  }
 }



