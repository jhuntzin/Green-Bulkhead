
#include "ThingSpeak.h"
#include <SPI.h>
#include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetClient client;
// On Arduino:  0 - 1023 maps to 0 - 5 volts

unsigned long myChannelNumber = 537825;
const char * myWriteAPIKey = "";

//Flowmeter Stuff
byte statusLed    = 13;

byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 7.1 pulses per second per (Calibrated for our purposes)
// litre/minute of flow.
float calibrationFactor = 7.1;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;


void setup()
{
  
  Serial.begin(9600);
  Ethernet.begin(mac);
  ThingSpeak.begin(client);

  //Flowrate Stuff
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

/**
 * Main program loop
 */

void loop()
{ 
  //Flowrate Stuff
  if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    //Serial.print("Flow rate: ");
    //Serial.print(int(flowRate));  // Print the integer part of the variable
    //Serial.print(".");             // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac = (flowRate - int(flowRate)) * 10;
    //Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    //Serial.print("L/min");
    // Print the number of litres flowed in this second
    //Serial.print("  Current Liquid Flowing: ");             // Output separator
    //Serial.print(flowMilliLitres);
    //Serial.print("mL/Sec");

    // Print the cumulative total of litres flowed since starting
    //Serial.print("  Output Liquid Quantity: ");             // Output separator
    //Serial.print(totalMilliLitres);
    //Serial.println("mL"); 

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }

  
  //Sesnor Inputs
  int sensorValue1 = analogRead(A0);
  int sensorValue2 = analogRead(A1);
  float sensorValue3 = totalMilliLitres/1000; //convert to Liters
  
  
  Serial.println(sensorValue1);
  Serial.println(sensorValue2);
  Serial.println(totalMilliLitres);
  Serial.println(sensorValue3);
 
  // Write to ThingSpeak. 
  ThingSpeak.setField(1, sensorValue1);
  ThingSpeak.setField(2, sensorValue2);
  ThingSpeak.setField(3, sensorValue3);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  delay(15000); // ThingSpeak will only accept updates every 15 seconds.
}

void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++; 
}
