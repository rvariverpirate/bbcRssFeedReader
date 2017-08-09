#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <LiquidCrystal.h>

// Setup ESP pins for LCD display
const int rs = 5, en = 4, d4 = 16, d5 = 14, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Home WiFi
const char WIFI_SSID[] = "YourWiFISSID";
const char WIFI_PSK[] = "YourPassword";
const String startSequence = "<description><![CDATA[";// Start read
const String endSequence = "]]></description>";// End read
 
// Remote site information
const char http_site[] = "feeds.bbci.co.uk";
const int http_port = 80;
 
// Pin definitions
const int LED_PIN = 2;

// Set up variables for checing start/stop transmit (same length)
String isStartSequence = "<deskription><![CDATA[";// Start read
String isEndSequence = "]]></deskription>";// End read
char allDescriptions[50][250];// Store  all descriptions here 
String description = "";// Store description here
char descriptionArray [200];// then here
String descriptionOut = "";// Send out final string here
char displayed = ' ';//
bool dataReceived = false;
char carriageReturn = '\r';// Carriage Return ASCII
int line = 0;
unsigned int scrolls;


// Global variables
WiFiClient client;
 
void setup() {
  // Setup the LCD's number of columns and rows:
  lcd.begin(16,2);
    
  // Set up serial console to read web page
  Serial.begin(9600);
  Serial.print("RSS Feed Reader ");
  // Print a message to the LCD
  lcd.print("RSS Feed Reader ");
  
  // Set up LED for debugging
  pinMode(LED_PIN, OUTPUT);
  
  // Connect to WiFi
  connectWiFi();
  
  // Attempt to connect to website
  if ( !getPage() ) {
    Serial.println("GET request failed");
    lcd.print("GET request failed");
  }
}
 
void loop() {
  //allDescriptions = "";// clear this
  // If there are incoming bytes, print them
  if ( client.available() ) {
    char c = client.read();
    if(isStartSequence != startSequence)// Keep looking
    { 
        isStartSequence.remove(0,1);// Remove first char of string
        isStartSequence += c;/// append to end
     }
     else // Start sequence was found start recording
     {
        if(isEndSequence != endSequence)
        {
             isEndSequence.remove(0,1);// Remove first char
             isEndSequence += c;// add to end
             description += c;// Add data to description
        }
        else // Found end sequence after start sequence
        {     
            
            if(line < 16){// Get first 15 stories
                // Replace end sequence with blank space
                description.replace(endSequence, String(' '));// I think replace is wrong!
                int itter = 0;
                for(itter = 0; itter < description.length(); itter++){
                    if(description.length() < 245){
                      allDescriptions[line][itter] = description.charAt(itter);
                    }
                  }
      
                  
                  description = "";// clear description so it doesnt pile up
                  line ++;// Move to next story/line
      
                  
                  // Continue search
                  isEndSequence.remove(0,1);
                  isEndSequence += c;// Shift left
                  isStartSequence.remove(0,1);
                  isStartSequence += c;// Shift left

              }
        }
      }

      
  }
  
  // If the server has disconnected, stop the client and WiFi
  if ( !client.connected() ) {
    
    // Close socket and wait for disconnect from WiFi
    client.stop();
    if ( WiFi.status() != WL_DISCONNECTED ) {
      WiFi.disconnect();
    }
    
    // Turn off LED
    digitalWrite(LED_PIN, LOW);
    scrolls = 0;
    while(scrolls < 5)// print current data 5 times before updating
    {
        lcd.clear();
        // Send Data
        unsigned int num = 0;
        for(num = 0; num < 16; num++)
        {
             delay(10);// Wait a moment
             descriptionOut = allDescriptions[num];
             lcd.setCursor(16,0);

             scrollToLeftLargeString(0, descriptionOut, 200);
             
             /*for(int thisChar = 0; thisChar < descriptionOut.length(); thisChar++){
                Serial.print(descriptionOut[thisChar]);
                lcd.print(descriptionOut[thisChar]);
                lcd.scrollDisplayLeft();
                if(thisChar %16 == 0){
                    lcd.setCursor(16,0);
                  }
                delay(200);
              }*/
         }
  
         scrolls ++;
         delay(1000);
    
     }
   
    // Connect to WiFi
    connectWiFi();
  
    // Attempt to connect to website
    if ( !getPage() ) {
      Serial.println("GET request failed");
      lcd.print("GET request failed");
    }
    
  }
}// Loop ends here
 
// Attempt to connect to WiFi
void connectWiFi() {
  
  byte led_status = 0;
  
  // Set WiFi mode to station (client)
  WiFi.mode(WIFI_STA);
  
  // Initiate connection with SSID and PSK
  WiFi.begin(WIFI_SSID, WIFI_PSK);
  
  // Blink LED while we wait for WiFi connection
  while ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(LED_PIN, led_status);
    led_status ^= 0x01;
    delay(100);
    Serial.println("Trying to connect");
    lcd.println("Trying to connect");// maybe remove this later
  }
  
  // Turn LED on when we are connected
  digitalWrite(LED_PIN, HIGH);
}
 
// Perform an HTTP GET request to a remote page
bool getPage() {
  
  // Attempt to make a connection to the remote server
  if ( !client.connect(http_site, http_port) ) {
    return false;
  }
  
  // Make an HTTP GET request
  client.println("GET /news/video_and_audio/world/rss.xml HTTP/1.1");
  client.print("Host: ");
  client.println(http_site);
  client.println("Connection: close");
  client.println();
  
  return true;
}


void scrollToLeftLargeString(int line, String str, int delayTime){
  String tempString = "                 ";
  lcd.clear();
  for(int i = 0; i < str.length() + 32; i++)
  {
    if(i < str.length()){
      tempString += str[i];// add current char to temporary string
    }
    else
    {
      tempString += " ";// let the last  16 chars scroll across the screen
    }
    if(i > 16)// string is loaded
    {
      tempString.remove(0,1);// remove the first character in the temp string
      lcd.println(tempString);
      Serial.println(tempString);
      delay(delayTime);
      lcd.clear();
    }
  }
}
