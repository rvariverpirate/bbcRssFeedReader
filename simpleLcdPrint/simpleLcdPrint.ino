#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>


const int rs = 5, en = 4, d4 = 16, d5 = 14, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // Setup the LCD's number of columns and rows:
  lcd.begin(16,2);

  // Print a message to the LCD
  lcd.print("Hello World!");
  
}

void loop() {
  // set cursor to column 0, line 1
  lcd.setCursor(0,1);

  // print number of seconds since reset
  lcd.print(millis()/1000);

}
