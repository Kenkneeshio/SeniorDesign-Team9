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

int I2C_ADDRESS = 8; // From sheet

// i2C Request codes. Each of these values are messages to the slave to request certain information.
// These codes must be sent first along Wire.write() before initiating a Wire.requestFrom() function call.
const int CURRENT = 0;
const int VOLTAGE = 1;
const int TEMPERATURE_0 = 2;
const int TEMPERATURE_1 = 3;
const int TEMPERATURE_2 = 4;

int lastRequestedEvent = -1;

float presentCurrent = 0.0;
float presentVoltage = 0.0;
float presentTemperature0 = 0.0;
float presentTemperature1 = 0.0;
float presentTemperature2 = 0.0;

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);
}
byte a[4];
byte b[4];
byte c[4];

byte x = 0;
void loop()
{
  lastRequestedEvent = (int)x;
  Serial.print("Requesting: ");
  Serial.println(x);
  Wire.beginTransmission(I2C_ADDRESS); // transmit to device #8
  Wire.write(x);                       // sends one byte
  Wire.endTransmission();              // stop transmitting

  switch (lastRequestedEvent)
  {
  case CURRENT:
    Wire.requestFrom(I2C_ADDRESS, 7); // request 6 bytes from slave device #8    
    while (Wire.available())
    {                       // slave may send less than requested
      char c = Wire.read(); // receive a byte as character
      Serial.print(c);      // print the character
    }
    break;
  case VOLTAGE:
    Wire.requestFrom(I2C_ADDRESS, 7); // request 6 bytes from slave device #8
    while (Wire.available())
    {                       // slave may send less than requested
      char c = Wire.read(); // receive a byte as character
      Serial.print(c);      // print the character
    }
    break;
  case TEMPERATURE_0:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from slave device #8
    while (Wire.available())
    { // peripheral may send less than requested
      // byte c[4] = Wire.read(); // receive a byte as character
      int i = 0;
      while (i < 4)
      {                     // peripheral may send less than requested
        a[i] = Wire.read(); // receive a byte as character
        i++;
        // Serial.println(i);         // print the character
      }

      union Temp
      {
        byte temp_byte[4];
        float temp_float;
      } Temp_union;
      Temp_union.temp_byte[0] = a[0];
      Temp_union.temp_byte[1] = a[1];
      Temp_union.temp_byte[2] = a[2];
      Temp_union.temp_byte[3] = a[3];
      if (Temp_union.temp_float > 0)
      {
        presentTemperature0 = Temp_union.temp_float;
        Serial.print("Temperature 0: ");
        Serial.println(presentTemperature0);
      }
    }
    break;
  case TEMPERATURE_1:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from slave device #8
    while (Wire.available())
    { // peripheral may send less than requested
      // byte c[4] = Wire.read(); // receive a byte as character
      int i = 0;
      while (i < 4)
      {                     // peripheral may send less than requested
        b[i] = Wire.read(); // receive a byte as character
        i++;
        // Serial.println(i);         // print the character
      }

      union Temp
      {
        byte temp_byte[4];
        float temp_float;
      } Temp_union;
      Temp_union.temp_byte[0] = b[0];
      Temp_union.temp_byte[1] = b[1];
      Temp_union.temp_byte[2] = b[2];
      Temp_union.temp_byte[3] = b[3];
      if (Temp_union.temp_float > 0)
      {
        presentTemperature1 = Temp_union.temp_float;
        Serial.print("Temperature 1: ");
        Serial.println(presentTemperature1);
      }
    }
    break;
  case TEMPERATURE_2:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from slave device #8
    while (Wire.available())
    { // peripheral may send less than requested
      // byte c[4] = Wire.read(); // receive a byte as character
      int i = 0;
      while (i < 4)
      {                     // peripheral may send less than requested
        c[i] = Wire.read(); // receive a byte as character
        i++;
        // Serial.println(i);         // print the character
      }

      union Temp
      {
        byte temp_byte[4];
        float temp_float;
      } Temp_union;
      Temp_union.temp_byte[0] = c[0];
      Temp_union.temp_byte[1] = c[1];
      Temp_union.temp_byte[2] = c[2];
      Temp_union.temp_byte[3] = c[3];
      if (Temp_union.temp_float > 0)
      {
        presentTemperature2 = Temp_union.temp_float;
        Serial.print("Temperature 2: ");
        Serial.println(presentTemperature2);
      }
    }
    break;
  }


  Serial.println(" ");
  x++;
  if (x > 4)
    x = 0;
  delay(2000);
}
