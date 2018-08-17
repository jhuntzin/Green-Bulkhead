/*
 ESP8266 --> ThingSpeak Channel via MKR1000 Wi-Fi
 
 This sketch sends the Wi-Fi Signal Strength (RSSI) of an ESP8266 to a ThingSpeak
 channel using the ThingSpeak API (https://www.mathworks.com/help/thingspeak).
 
 Requirements:
 
   * ESP8266 Wi-Fi Device
   * Arduino 1.6.9+ IDE
   * Additional Boards URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json
   * Library: esp8266 by ESP8266 Community
 
 ThingSpeak Setup:
 
   * Sign Up for New User Account - https://thingspeak.com/users/sign_up
   * Create a new Channel by selecting Channels, My Channels, and then New Channel
   * Enable one field
   * Note the Channel ID and Write API Key
    
 Setup Wi-Fi:
  * Enter SSID
  * Enter Password
  
 Tutorial: http://nothans.com/measure-wi-fi-signal-levels-with-the-esp8266-and-thingspeak
   
 Created: Feb 1, 2017 by Hans Scharler (http://nothans.com)
*/

#include <ESP8266WiFi.h>
//#include <dht.h>
// Wi-Fi Settings
const char* ssid = "GamingHouse"; // your wireless network name (SSID)
const char* password = "patisgay69"; // your Wi-Fi network password

WiFiClient client;

// ThingSpeak Settings
const int channelID = 537825;
String writeAPIKey = "BJJUPXRKO9XADTV9"; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";
const int postingInterval = 5 * 1000; // post data every 6 seconds

//dht DHT;


int moisturePin = A0;

void setup() {
  Serial.begin(9600);
  
  pinMode (D0, INPUT); //NodeMCU pin mapping: Digital 0 = D0 etc
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  if (client.connect(server, 80)) {
    
    // Measure Signal Strength (RSSI) of Wi-Fi connection
    long rssi = WiFi.RSSI();

    //Measure Other Things
    int sensorValue = analogRead(moisturePin);
    
    //Map 1-1024 Sensor Values to a 0-100 scale of wetness
    //int mappedValue = map(sensorValue, 1024, 550, 0, 100);  //550 = full submersion
    
    Serial.println(sensorValue);
    //Serial.println(mappedLevel);
    
    // Construct API request body
    
    //Field 1
    
    String body = "&field1=";
           //body += mappedLevel;
           body += sensorValue;
           
    Serial.print("RSSI: ");
    Serial.println(rssi);

    //Field 2

           body += "&field2=";
           body += String(rssi);
           
    Serial.println(body);
    
    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: ESP8266 (nothans)/1.0");
    client.println("Connection: close");
    client.println("X-THINGSPEAKAPIKEY: " + writeAPIKey);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(body.length()));
    client.println("");
    client.print(body);
    
  }
  client.stop();

  // wait and then post again
  delay(postingInterval);
}
