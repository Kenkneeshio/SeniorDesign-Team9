// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

int I2C_ADDRESS = 8;      // From sheet

// i2C Request codes. Each of these values are messages to the slave to request certain information.
// These codes must be sent first along Wire.write() before initiating a Wire.requestFrom() function call. 
const int CURRENT = 0;
const int VOLTAGE = 1;
const int TEMPERATURE_0 = 2;         
const int TEMPERATURE_1 = 3;         
const int TEMPERATURE_2 = 4;       

int lastRequestedEvent = -1;


void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600); 
}

byte x  = 0;
void loop() {
    lastRequestedEvent = (int)x;
    Serial.print("Requesting: ");
    Serial.println(x);
  Wire.beginTransmission(I2C_ADDRESS); // transmit to device #8
  Wire.write(x);              // sends one byte
  Wire.endTransmission();    // stop transmitting

    switch (lastRequestedEvent)
    {
        case CURRENT:
        Wire.requestFrom(I2C_ADDRESS, 7);    // request 6 bytes from slave device #8
        break;
        case VOLTAGE:
        Wire.requestFrom(I2C_ADDRESS, 7);    // request 6 bytes from slave device #8
        break;
        case TEMPERATURE_0:
        Wire.requestFrom(I2C_ADDRESS, 7);    // request 6 bytes from slave device #8
        break;
        case TEMPERATURE_1:
        Wire.requestFrom(I2C_ADDRESS, 7);    // request 6 bytes from slave device #8
        break;
        case TEMPERATURE_2:
        Wire.requestFrom(I2C_ADDRESS, 7);    // request 6 bytes from slave device #8
        break;
    }

    while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }

  Serial.println(" "); 
  x++;
  if(x > 4)
    x = 0;
  delay(2000);
}
