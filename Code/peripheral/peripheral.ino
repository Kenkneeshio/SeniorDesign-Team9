//////////////////////////////////////////////////////////
// SDSU Senior Design Team 9 - Robot Power System
// peripheral.ino
// Desc:
//////////////////////////////////////////////////////////

#include <Wire.h> // used for i2c communication
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 5 // THE DIGITAL PIN THAT THE TEMPERATURE SENSORS ARE CONNECTED TO
#define CURRENT_PIN A0
#define VOLTAGE_PIN A1

int led = 13; // LED is assigned to pin 13

int I2C_ADDRESS = 8; // Example i2C address

int lastRequestedEvent = -1; // This is the last requested event stored. This could be any one of the codes below:
//////////////////////////////////////////////////////////
// i2C Request codes. Each of these values are messages to the device to request certain information.
// These codes must be sent first along Wire.write() before initiating a Wire.requestFrom() function call.
//////////////////////////////////////////////////////////
const int CURRENT_0 = 0;
const int CURRENT_1 = 1;
const int VOLTAGE_0 = 2;
const int VOLTAGE_1 = 3;
const int TEMPERATURE_0 = 4;
const int TEMPERATURE_1 = 5;
const int TEMPERATURE_2 = 6;
const int TEMPERATURE_3 = 7;
//////////////////////////////////////////////////////////

// const float LOGIC_VOLTAGE = 5.08;
const float LOGIC_VOLTAGE = 4.4;
const float VOLTAGE_DIVIDER_R2 = 504000;
const float VOLTAGE_DIVIDER_R1 = 1223000;
const float SHUNT_RESISTOR = 0.01028;
const int LOAD_RESISTOR = 5000;

//////////////////////////////////////////////////////////
// DallasTemperature Sensor Serial Numbers
// Do not change the short serial numbers, as you will need to desolder them from the board.
const uint8_t temperatureProbe0_LONG[8] = {0x28, 0x75, 0x56, 0x81, 0xE3, 0xD5, 0x3C, 0xAB};
const uint8_t temperatureProbe1_LONG[8] = {0x28, 0x0D, 0x7E, 0x81, 0xE3, 0x69, 0x3C, 0x1B};
const uint8_t temperatureProbe2_SHORT[8] = {0x28, 0xFE, 0x8D, 0x81, 0xE3, 0x73, 0x3C, 0xD3};
const uint8_t temperatureProbe3_SHORT[8] = {0x28, 0x9C, 0x56, 0x81, 0xE3, 0xD9, 0x3C, 0x59};
//////////////////////////////////////////////////////////

int numberOfDevices; // Number of temperature devices found
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

//////////////////////////////////////////////////////////
// Present Values
// These floats represent the present current, voltage, or temperature value
// from the ADC.
float presentCurrent0 = 0.0;
float presentCurrent1 = 0.0;
float presentVoltage0 = 0.0;
float presentVoltage1 = 0.0;
float presentTemperature0f;
float presentTemperature1f;
float presentTemperature2f;
float presentTemperature3f;
//////////////////////////////////////////////////////////

bool debug = false;

float value;

//////////////////////////////////////////////////////////
/*
 * ADC2Current(float analogValue)
 * Description: Converts a given analog value from an ADC to the correct voltage based
 * the equation from the INA139 datasheet , where iS = (Vout * 1k) / (rs * rl).
 * Param: analogValue - ADC value
 */
//////////////////////////////////////////////////////////

float ADC2Current(float analogValue)
{
  float value;
  value = ((analogValue + 1) / 1024 * LOGIC_VOLTAGE); // first convert the analog value based on the logic level voltage
  // then use the equation from the INA139 datasheet , where iS = (Vout * 1k) / (rs * rl)
  value = (value * (1000) / ((LOAD_RESISTOR) * (SHUNT_RESISTOR)));
  return value;
}

//////////////////////////////////////////////////////////
/*
 * ADC2Voltage(float analogValue)
 * Description: Converts a given analog value from an ADC to the correct voltage based
 * on the voltage divider resistor values, and output voltage from the ADC to recover the input.
 * Param: analogValue - ADC value
 */
//////////////////////////////////////////////////////////

float ADC2Voltage(float analogValue)
{
  float value;
  value = analogValue * (LOGIC_VOLTAGE / 1023.0); // first convert the analog value based on the logic level voltage
  // then get back the input voltage from the voltage divider equation: Vin = Vout * (R2+R1)/R2
  value = (value * (VOLTAGE_DIVIDER_R2 + VOLTAGE_DIVIDER_R1) / (VOLTAGE_DIVIDER_R2));
  return value;
}

// function that executes whenever data is received from controller
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while (1 < Wire.available())
  {                       // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);      // print the character
  }
  lastRequestedEvent = Wire.read(); // receive byte as an integer

  if (debug) // if debug is enabled, print out the last requested event type to serial monitor
  {
    switch (lastRequestedEvent)
    {
    case CURRENT_0:
      Serial.println("Current 0");
      break;
    case CURRENT_1:
      Serial.println("Current 1");
      break;
    case VOLTAGE_0:
      Serial.println("Voltage 0");
      break;
    case VOLTAGE_1:
      Serial.println("Voltage 1");
      break;
    case TEMPERATURE_0:
      Serial.println("Temperature 0");
      break;
    case TEMPERATURE_1:
      Serial.println("Temperature 1");
      break;
    case TEMPERATURE_2:
      Serial.println("Temperature 2");
      break;
    case TEMPERATURE_3:
      Serial.println("Temperature 3");
      break;
    }
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  // Since all of the data we are reading is in floats, we have to send
  // the values in a total of four bytes, one at a time. On the controller end,
  // it will have to reconstruct the float from the bytes sent.

  byte *data;
  switch (lastRequestedEvent)
  {
  case CURRENT_0:
    data = (byte *)&presentCurrent0;
    // Wire.write("Current 0"); // print the integer
    //  Wire.write(data[0]);
    //  Wire.write(data[1]);
    //  Wire.write(data[2]);
    //  Wire.write(data[3]);
    break;
  case CURRENT_1:
    // Wire.write("Current 1"); // print the integer
    data = (byte *)&presentCurrent1;
    // Wire.write("Current 0"); // print the integer
    //  Wire.write(data[0]);
    //  Wire.write(data[1]);
    //  Wire.write(data[2]);
    //  Wire.write(data[3]);
    break;
  case VOLTAGE_0:
    // Wire.write("Voltage 0"); // print the integer
    data = (byte *)&presentVoltage0;
    // Wire.write(data[0]);
    // Wire.write(data[1]);
    // Wire.write(data[2]);
    // Wire.write(data[3]);
    break;
  case VOLTAGE_1:
    data = (byte *)&presentVoltage1;
    // Wire.write(data[0]);
    // Wire.write(data[1]);
    // Wire.write(data[2]);
    // Wire.write(data[3]);
    break;
  case TEMPERATURE_0:
    data = (byte *)&presentTemperature0f;
    // Serial.print("data bytes:");
    // Serial.print(data[0]);
    // Serial.print(" ");
    // Serial.print(data[1]);
    // Serial.print(" ");
    // Serial.print(data[2]);
    // Serial.print(" ");
    // Serial.println(data[3]);
    // Wire.write(data[0]);
    // Wire.write(data[1]);
    // Wire.write(data[2]);
    // Wire.write(data[3]);

    break;
  case TEMPERATURE_1:
    data = (byte *)&presentTemperature1f;
    // Wire.write(data[0]);
    // Wire.write(data[1]);
    // Wire.write(data[2]);
    // Wire.write(data[3]);
    break;
  case TEMPERATURE_2:
    data = (byte *)&presentTemperature2f;
    // Wire.write(data[0]);
    // Wire.write(data[1]);
    // Wire.write(data[2]);
    // Wire.write(data[3]);
    break;

  case TEMPERATURE_3:
    data = (byte *)&presentTemperature3f;
    // Wire.write(data[0]);
    // Wire.write(data[1]);
    // Wire.write(data[2]);
    // Wire.write(data[3]);
    break;
  }
  if (debug)
  {
    Serial.print("data bytes:");
    Serial.print(data[0]);
    Serial.print(" ");
    Serial.print(data[1]);
    Serial.print(" ");
    Serial.print(data[2]);
    Serial.print(" ");
    Serial.println(data[3]);
  }

  Wire.write(data[0]);
  Wire.write(data[1]);
  Wire.write(data[2]);
  Wire.write(data[3]);
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

  pinMode(led, OUTPUT);
}

void loop()
{
  static uint32_t millis_ctr = 0;
  // Serial.println("Requesting Temps");

  if (millis() > millis_ctr + 5000)
  {
    presentCurrent0 = ADC2Current(analogRead(CURRENT_PIN));
    presentVoltage0 = ADC2Voltage(analogRead(VOLTAGE_PIN));

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
        else if (memcmp(temperatureProbe3_SHORT, tempDeviceAddress, 8) == 0)
        {
          // device 2
          sensors.requestTemperatures();
          presentTemperature3f = sensors.getTempC(tempDeviceAddress);
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
