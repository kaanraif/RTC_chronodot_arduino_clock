//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Create: Kaan Raif on 4/11/2017
//
// Purpose: Set up a clock that reads in from RTC , then displays to LCD
// Need to create way that APP interfaces with the controller and modifies clock
//
// This code is modified version of code found on the ChronoDot by Macetech design and specs webpage
// 
// Modifications:
// Added a way to initialize LCD screen and display to LCD,when running loop to also output the clock (4/12/17)
// Needed to adjust for initial time shift from setting time, created carry bits and while loops for the times not in bounds (4/16/17)
//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Initialize Libraries
#include <Wire.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12,11,5,4,3,2); //set lcd pins used
int sec;
int m;
int h; 
int c; // carry bit for the minutes when minutes > 60 due to time shift from initial time setting
int day;


void setup()
{
  // Initialize time once *THIS IS THE INITIAL START TIME OF CLOCK, DEPENDS ON THE WHEN BATTERY FIRST PUT IN RTC*
  // *RTC RECORDS SIMILAR TO MILLIS() THEREFORE YOU WANT TO MAKE SURE THE BATTERY IS PLACED IN RTC AT SAME TIME AS INITIAL TIME*
  sec = 00; //initial sec
  m = 51; // initial min
  h = 17; //initial hour
  
  // initialize LCD
  //lcd.begin(16,2); //define # columns and rows in lcd screen
  //lcd.setCursor(0,0);

  Wire.begin();
  Serial.begin(9600); //bps for serial transimttion
  // clear EOSC bit
  // Sometimes necessary to ensure that the clock
  // keeps running on just battery power.
  Wire.beginTransmission(0x68); // address DS3231
  Wire.write(0x0E); // select register
  Wire.write(0b00011100); // write register bitmap, bit 7 is /EOSC
  Wire.endTransmission();
}
 
void loop()
{
  // send request to receive data starting at register 0
  Wire.beginTransmission(0x68); // 0x68 is DS3231 device address
  Wire.write((byte)0); // start at register 0
  Wire.endTransmission();
  Wire.requestFrom(0x68, 4); // request three bytes (seconds, minutes, hours)
 
  while(Wire.available())
  { 
    int seconds = Wire.read(); // get seconds
    int minutes = Wire.read(); // get minutes
    int hours = Wire.read();   // get hours
    int day = Wire.read(); // get day 
 
    seconds = sec + (((seconds & 0b11110000)>>4)*10 + (seconds & 0b00001111)); // convert BCD to decimal and add time to inital start time 
    minutes = m + (((minutes & 0b11110000)>>4)*10 + (minutes & 0b00001111)); // convert BCD to decimal and add to initial start time
       while (minutes > 60) // accounts for initial time shift
        {
          minutes = minutes - 60; // 
          c = 1; //create carry so that hour can be incremented
        }
    hours = c + h + (((hours & 0b00100000)>>5)*20 + ((hours & 0b00010000)>>4)*10 + (hours & 0b00001111)); // convert BCD to decimal (assume 24 hour mode) and add to initial start time
       while (hours > 24) // accounts for initial time shift when setting clock
        {
          hours = hours - 24; 
        }
    day = (((day & 0b11110000)>>4)*10 + (day & 0b00001111)); // convert BCD to decimal  
       // day values from 1-7
       // 1-7 => (Sun-Sat)
       
    Serial.print(hours);
    Serial.print(":"); 
    Serial.print(minutes); 
    Serial.print(":"); 
    Serial.println(seconds);
    
    
    //display to lcd screen
    //lcd.print("Time:");
    //lcd.print(hours);
    //lcd.print(":"); 
    //lcd.print(minutes); 
    //lcd.print(":"); 
    //lcd.print(seconds);
  }
 
  delay(1000);
}
