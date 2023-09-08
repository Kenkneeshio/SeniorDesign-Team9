// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 5 // THE DIGITAL PIN THAT THE TEMPERATURE SENSORS ARE CONNECTED TO
#define CURRENT_PIN A0
#define VOLTAGE_PIN A1

int I2C_ADDRESS = 8; // From sheet

int lastRequestedEvent = -1;

// i2C Request codes. Each of these values are messages to the slave to request certain information.
// These codes must be sent first along Wire.write() before initiating a Wire.requestFrom() function call.
const int CURRENT = 0;
const int VOLTAGE = 1;
const int TEMPERATURE_0 = 2;
const int TEMPERATURE_1 = 3;
const int TEMPERATURE_2 = 4;

const float LOGIC_VOLTAGE = 5.08;
const float VOLTAGE_DIVIDER_R2 = 510000;
const float VOLTAGE_DIVIDER_R1 = 12000000;
const float SHUNT_RESISTOR = 0.01028;
const int LOAD_RESISTOR = 5000;

const uint8_t temperatureProbe0_LONG[8] = {0x28, 0x75, 0x56, 0x81, 0xE3, 0xD5, 0x3C, 0xAB};
const uint8_t temperatureProbe1_LONG[8] = {0x28, 0x0D, 0x7E, 0x81, 0xE3, 0x69, 0x3C, 0x1B};
const uint8_t temperatureProbe2_SHORT[8] = {0x28, 0xFE, 0x8D, 0x81, 0xE3, 0x73, 0x3C, 0xD3};

int numberOfDevices; // Number of temperature devices found
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

float presentCurrent = 0.0;
float presentVoltage = 0.0;
byte *presentTemperature0;
byte *presentTemperature1;
byte *presentTemperature2;

float presentTemperature0f;
float presentTemperature1f;
float presentTemperature2f;

float value;

/*
 * ADC2Current(float analogValue)
 * Description: Converts a given analog value from an ADC to the correct voltage based
 * the equation from the INA139 datasheet , where iS = (Vout * 1k) / (rs * rl).
 * Param: analogValue - ADC value
 */

float ADC2Current(float analogValue)
{
  float value;
  value = ((analogValue + 1) / 1024 * LOGIC_VOLTAGE); // first convert the analog value based on the logic level voltage
  // then use the equation from the INA139 datasheet , where iS = (Vout * 1k) / (rs * rl)
  value = (value * (1000) / ((LOAD_RESISTOR) * (SHUNT_RESISTOR)));
  return value;
}

/*
 * ADC2Voltage(float analogValue)
 * Description: Converts a given analog value from an ADC to the correct voltage based
 * on the voltage divider resistor values, and output voltage from the ADC to recover the input.
 * Param: analogValue - ADC value
 */

float ADC2Voltage(float analogValue)
{
  float value;
  value = ((analogValue + 1) / 1024 * LOGIC_VOLTAGE); // first convert the analog value based on the logic level voltage
  // then get back the input voltage from the voltage divider equation: Vin = Vout * (R2+R1)/R2
  value = (value * (VOLTAGE_DIVIDER_R2 + VOLTAGE_DIVIDER_R1) / (VOLTAGE_DIVIDER_R2));
  return value;
}

float requestCurrent()
{
  Serial.println("Current requested.");
  presentCurrent = ADC2Current(analogRead(CURRENT_PIN));
  return presentCurrent;
}

float requestVoltage()
{
  Serial.println("Voltage requested.");
  presentVoltage = ADC2Voltage(analogRead(VOLTAGE_PIN));
  return presentVoltage;
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while (1 < Wire.available())
  {                       // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);      // print the character
  }
  lastRequestedEvent = Wire.read(); // receive byte as an integer
  switch (lastRequestedEvent)
  {
  case CURRENT:
    Serial.println("Current"); // print the integer
    break;
  case VOLTAGE:
    Serial.println("Voltage"); // print the integer
    break;
  case TEMPERATURE_0:
    Serial.println("Temperature 0"); // print the integer
    break;
  case TEMPERATURE_1:
    Serial.println("Temperature 1"); // print the integer
    break;
  case TEMPERATURE_2:
    Serial.println("Temperature 2"); // print the integer
    break;
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  // Wire.write("hello "); // respond with message of 6 bytes
  switch (lastRequestedEvent)
  {
  case CURRENT:
    Wire.write("Current"); // print the integer
    break;
  case VOLTAGE:
    Wire.write("Voltage"); // print the integer
    break;
  case TEMPERATURE_0:
   presentTemperature0 = (byte *)&presentTemperature0f;
//  Serial.print("data bytes:");
//          Serial.print(presentTemperature0[0]);
//          Serial.print(" ");
//          Serial.print(presentTemperature0[1]);
          //Serial.print(" ");
          //Serial.print(presentTemperature0[2]);
          //Serial.print(" ");
          //Serial.println(presentTemperature0[3]);
    Wire.write(presentTemperature0[0]);
    Wire.write(presentTemperature0[1]);
    Wire.write(presentTemperature0[2]);
    Wire.write(presentTemperature0[3]);

    break;
  case TEMPERATURE_1:
  presentTemperature1 = (byte *)&presentTemperature1f;
//  Serial.print("data bytes:");
//          Serial.print(presentTemperature1[0]);
//          Serial.print(" ");
//          Serial.print(presentTemperature1[1]);
//          Serial.print(" ");
//          Serial.print(presentTemperature1[2]);
//          Serial.print(" ");
//          Serial.println(presentTemperature1[3]);
    Wire.write(presentTemperature1[0]);
    Wire.write(presentTemperature1[1]);
    Wire.write(presentTemperature1[2]);
    Wire.write(presentTemperature1[3]);
    break;
  case TEMPERATURE_2:
  presentTemperature2 = (byte *)&presentTemperature2f;
//  Serial.print("data bytes:");
//          Serial.print(presentTemperature2[0]);
//          Serial.print(" ");
//          Serial.print(presentTemperature2[1]);
//          Serial.print(" ");
//          Serial.print(presentTemperature2[2]);
//          Serial.print(" ");
//          Serial.println(presentTemperature2[3]);
    Wire.write(presentTemperature2[0]);
    Wire.write(presentTemperature2[1]);
    Wire.write(presentTemperature2[2]);
    Wire.write(presentTemperature2[3]);
    break;
  }
}

void setup()
{
  // Set up Dallas Temperature Sensors
  sensors.begin(); // start up the dallas temperature library
  numberOfDevices = sensors.getDeviceCount();

  // Set up i2C Communication
  Wire.begin(I2C_ADDRESS);      // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register a receive on event
  Wire.onRequest(requestEvent); // register a request on event

  // Set up Serial Communication
  Serial.begin(9600); // start serial for output
}

void loop()
{
  static uint32_t millis_ctr = 0;
  //Serial.println("Requesting Temps");

  if (millis() > millis_ctr + 5000)
  {
    for (uint8_t i = 0; i < numberOfDevices; i++)
    {
      // Search the wire for address

      if (sensors.getAddress(tempDeviceAddress, i))
      {

        if (memcmp(temperatureProbe0_LONG, tempDeviceAddress, 8) == 0)
        {
          // device 0
          sensors.requestTemperatures();
          presentTemperature0f = sensors.getTempC(tempDeviceAddress);
          presentTemperature0 = (byte *)&presentTemperature0f;
          Serial.print("Device ");
          Serial.print(i);
          Serial.print(" - Temp C: ");
          Serial.println(presentTemperature0f);
          // Serial.print("data bytes:");
          // Serial.print(presentTemperature0[0]);
          // Serial.print(" ");
          // Serial.print(presentTemperature0[1]);
          // Serial.print(" ");
          // Serial.print(presentTemperature0[2]);
          // Serial.print(" ");
          // Serial.println(presentTemperature0[3]);
        }
        else if (memcmp(temperatureProbe1_LONG, tempDeviceAddress, 8) == 0)
        {
          // device 1
          sensors.requestTemperatures();
          presentTemperature1f = sensors.getTempC(tempDeviceAddress);
          presentTemperature1 = (byte *)&presentTemperature1f;
          Serial.print("Device ");
          Serial.print(i);
          Serial.print(" - Temp C: ");
          Serial.println(presentTemperature1f);
          // Serial.print("data bytes:");
          // Serial.print(presentTemperature1[0]);
          // Serial.print(" ");
          // Serial.print(presentTemperature1[1]);
          // Serial.print(" ");
          // Serial.print(presentTemperature1[2]);
          // Serial.print(" ");
          // Serial.println(presentTemperature1[3]);
        }
        else if (memcmp(temperatureProbe2_SHORT, tempDeviceAddress, 8) == 0)
        {
          // device 2
          sensors.requestTemperatures();
          presentTemperature2f = sensors.getTempC(tempDeviceAddress);
          presentTemperature2 = (byte *)&presentTemperature2f;
          Serial.print("Device ");
          Serial.print(i);
          Serial.print(" - Temp C: ");
          Serial.println(presentTemperature2f);
          //           Serial.print("data bytes:");
          // Serial.print(presentTemperature2[0]);
          // Serial.print(" ");
          // Serial.print(presentTemperature2[1]);
          // Serial.print(" ");
          // Serial.print(presentTemperature2[2]);
          // Serial.print(" ");
          // Serial.println(presentTemperature2[3]);
        }
        else
        {
          Serial.println("No mattches :(");
        }
      }
    }
    // delay(5000);

    millis_ctr = millis();
  }
}
