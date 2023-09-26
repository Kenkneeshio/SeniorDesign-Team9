

#include <Wire.h> // used for i2c communication

//////////////////////////////////////////////////////////
// i2C ASSIGNMENTS
int I2C_ADDRESS = 8; // From sheet

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
// Present Values
// These floats represent the present current, voltage, or temperature value
// from the ADC.
float presentCurrent0 = 0.0;
float presentCurrent1 = 0.0;
float presentVoltage0 = 0.0;
float presentVoltage1 = 0.0;
float presentTemperature0 = 0.0;
float presentTemperature1 = 0.0;
float presentTemperature2 = 0.0;
float presentTemperature3 = 0.0;
//////////////////////////////////////////////////////////

byte transmissionData = 0; // variable for sending data across the i2c bus
byte data[4]; // stores the temporary byte information coming from the i2c peripheral float values 
int lastRequestedEvent = -1;

union floatByte // two different data types in one area of memory. 
      {
        byte byteVal[4];
        float floatVal;
      } ;

//////////////////////////////////////////////////////////
// Debug
// Enable this for logging of all messages to the Serial monitor. 
bool debug = true;

void setup()
{
  Wire.begin(); // join i2c bus (address optional for host)
  Serial.begin(9600); // for this demo, keep serial monitor on regardless
}


void loop()
{
  lastRequestedEvent = (int)transmissionData; // explain what this is doing
  Serial.print("Requesting: ");
  Serial.println(transmissionData);
  Wire.beginTransmission(I2C_ADDRESS); // transmit to our intended device along the bus
  Wire.write(transmissionData);        // send the currently requested sensing value
  Wire.endTransmission();              // now stop transmitting

  switch (lastRequestedEvent)
  {
    case CURRENT_0:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from peripheral device #8
    while (Wire.available())
    { // peripheral may send less than requested
      
      int i = 0;
      while (i < 4)
      {                     // peripheral may send less than requested
        data[i] = Wire.read(); // receive a byte as character
        i++;
      }

      if (debug) // if debug enabled, the hex of the code about to be sent will print 
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


      union floatByte current;
      current.byteVal[0] = data[0];
      current.byteVal[1] = data[1];
      current.byteVal[2] = data[2];
      current.byteVal[3] = data[3];
      if (current.floatVal > 0)
      {
        presentCurrent0 = current.floatVal;
        Serial.print("Current 0: ");
        Serial.println(presentCurrent0);
      }
      else
      {
        Serial.println("Invalid Data received: ");
        Serial.print(data[0]);
        Serial.print(" ");
        Serial.print(data[1]);
        Serial.print(" ");
        Serial.print(data[2]);
        Serial.print(" ");
        Serial.println(data[3]);
      }
    }
    break;
  case CURRENT_1:
  case VOLTAGE_1:
    Wire.requestFrom(I2C_ADDRESS, 7); // request 6 bytes from peripheral device #8    
    while (Wire.available())
    {                       // peripheral may send less than requested
      char c = Wire.read(); // receive a byte as character
      Serial.print(c);      // print the character
    }
    break;

    case VOLTAGE_0:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from peripheral device #8
    while (Wire.available())
    { // peripheral may send less than requested
      int i = 0;
      while (i < 4)
      {                     // peripheral may send less than requested
        data[i] = Wire.read(); // receive a byte as character
        i++;
      }

      if (debug) // if debug enabled, the hex of the code about to be sent will print 
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


      union floatByte voltage;
      voltage.byteVal[0] = data[0];
      voltage.byteVal[1] = data[1];
      voltage.byteVal[2] = data[2];
      voltage.byteVal[3] = data[3];
      if (voltage.floatVal > 0)
      {
        presentVoltage0 = voltage.floatVal;
        Serial.print("Voltage 0: ");
        Serial.println(presentVoltage0);
      }
      else
      {
        Serial.println("Invalid Data received: ");
        Serial.print(data[0]);
        Serial.print(" ");
        Serial.print(data[1]);
        Serial.print(" ");
        Serial.print(data[2]);
        Serial.print(" ");
        Serial.println(data[3]);
      }
    }
    break;
  case TEMPERATURE_0:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from peripheral device #8
    while (Wire.available())
    { // peripheral may send less than requested
      int i = 0;
      while (i < 4)
      {                     // peripheral may send less than requested
        data[i] = Wire.read(); // receive a byte as character
        i++;
      }

      if (debug) // if debug enabled, the hex of the code about to be sent will print 
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

      union floatByte temperature;
      temperature.byteVal[0] = data[0];
      temperature.byteVal[1] = data[1];
      temperature.byteVal[2] = data[2];
      temperature.byteVal[3] = data[3];
      if (temperature.floatVal > 0)
      {
        presentTemperature0 = temperature.floatVal;
        Serial.print("Temperature 0: ");
        Serial.println(presentTemperature0);
      }
      else
      {
        Serial.println("Invalid Data received: ");
        Serial.print(data[0]);
        Serial.print(" ");
        Serial.print(data[1]);
        Serial.print(" ");
        Serial.print(data[2]);
        Serial.print(" ");
        Serial.println(data[3]);
      }
    }
    break;
  case TEMPERATURE_1:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from peripheral device #8
    while (Wire.available())
    { // peripheral may send less than requested
      int i = 0;
      while (i < 4)
      {                     // peripheral may send less than requested
        data[i] = Wire.read(); // receive a byte as character
        i++;
      }

      if (debug) // if debug enabled, the hex of the code about to be sent will print 
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

      union floatByte temperature;
      temperature.byteVal[0] = data[0];
      temperature.byteVal[1] = data[1];
      temperature.byteVal[2] = data[2];
      temperature.byteVal[3] = data[3];
      if (temperature.floatVal > 0)
      {
        presentTemperature1 = temperature.floatVal;
        Serial.print("Temperature 1: ");
        Serial.println(presentTemperature1);
      }
      else
      {
        Serial.println("Invalid Data received: ");
        Serial.print(data[0]);
        Serial.print(" ");
        Serial.print(data[1]);
        Serial.print(" ");
        Serial.print(data[2]);
        Serial.print(" ");
        Serial.println(data[3]);
      }
    }
    break;
  case TEMPERATURE_2:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from peripheral device #8
    while (Wire.available())
    { // peripheral may send less than requested
      int i = 0;
      while (i < 4)
      {                     // peripheral may send less than requested
        data[i] = Wire.read(); // receive a byte as character
        i++;
      }

      if (debug) // if debug enabled, the hex of the code about to be sent will print 
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

      union floatByte temperature;
      temperature.byteVal[0] = data[0];
      temperature.byteVal[1] = data[1];
      temperature.byteVal[2] = data[2];
      temperature.byteVal[3] = data[3];
      if (temperature.floatVal > 0)
      {
        presentTemperature2 = temperature.floatVal;
        Serial.print("Temperature 2: ");
        Serial.println(presentTemperature2);
      }
      else
      {
        Serial.println("Invalid Data received: ");
        Serial.print(data[0]);
        Serial.print(" ");
        Serial.print(data[1]);
        Serial.print(" ");
        Serial.print(data[2]);
        Serial.print(" ");
        Serial.println(data[3]);
      }
    }
    break;
   case TEMPERATURE_3:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from peripheral device #8
    while (Wire.available())
    { // peripheral may send less than requested
      int i = 0;
      while (i < 4)
      {                     // peripheral may send less than requested
        data[i] = Wire.read(); // receive a byte as character
        i++;
      }

      if (debug) // if debug enabled, the hex of the code about to be sent will print 
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

      union floatByte temperature;
      temperature.byteVal[0] = data[0];
      temperature.byteVal[1] = data[1];
      temperature.byteVal[2] = data[2];
      temperature.byteVal[3] = data[3];
      if (temperature.floatVal > 0)
      {
        presentTemperature3 = temperature.floatVal;
        Serial.print("Temperature 3: ");
        Serial.println(presentTemperature3);
      }
      else
      {
        Serial.println("Invalid Data received: ");
        Serial.print(data[0]);
        Serial.print(" ");
        Serial.print(data[1]);
        Serial.print(" ");
        Serial.print(data[2]);
        Serial.print(" ");
        Serial.println(data[3]);
      }
    }
    break;
  }


  Serial.println(" ");
  transmissionData++;
  if (transmissionData > 7)
    transmissionData = 0;

  float millis_ctr2 = millis();
        while(millis() < millis_ctr2 + 2000)
        {
            // wait for 200 ms and do nothing, getting around delay()
        }
  //delay(2000);
}
