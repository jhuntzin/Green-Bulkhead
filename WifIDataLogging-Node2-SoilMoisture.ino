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



 Adapted by John Huntzinger August 16, 2018
 University of Maryland, College Park
 ENST Undergraduate
*/

#include <ESP8266WiFi.h>
//#include <dht.h>
// Wi-Fi Settings
const char* ssid = "GreenBulkhead"; // your wireless network name (SSID)
const char* password = "xxxxxxxxxxxxx"; // your Wi-Fi network password

WiFiClient client;

// ThingSpeak Settings
const int channelID = 537825;
String writeAPIKey = "XXXXXXXXXXXXXXXX"; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";
const int postingInterval = 15 * 1000; // post data every 15 seconds

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
    int inverseSensorVal = map(sensorValue, 0, 1024, 1024, 0);
    
    //Map 1-1024 Sensor Values to a 0-100 scale of wetness
    //int percentValue = map(sensorValue, 1024, 550, 0, 100);  //550 = full submersion **NEEDS TO BE CALIBRATED**

    Serial.print("Moisture Value: ");
    Serial.println(sensorValue);
    
    
    // Construct API request body
    
    //Field 3
    
    String body = "field3=";
           body += String(rssi);
           
    //Serial.print("RSSI: ");
    //Serial.println(rssi);

    //Field 4

           body += "&field4=";
           body += String(inverseSensorVal);
           
    Serial.println(body);
    Serial.println("");
    
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
