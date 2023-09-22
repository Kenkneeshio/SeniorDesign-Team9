

#include <Wire.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>

int I2C_ADDRESS = 8; // From sheet

// i2C Request codes. Each of these values are messages to the peripheral to request certain information.
// These codes must be sent first along Wire.write() before initiating a Wire.requestFrom() function call.
const int CURRENT_0 = 0;
const int CURRENT_1 = 1;
const int VOLTAGE_0 = 2;
const int VOLTAGE_1 = 3;
const int TEMPERATURE_0 = 4;
const int TEMPERATURE_1 = 5;
const int TEMPERATURE_2 = 6;
const int TEMPERATURE_3 = 7;

int lastRequestedEvent = -1;

float presentCurrent0 = 0.0;
float presentCurrent1 = 0.0;
float presentVoltage0 = 0.0;
float presentVoltage1 = 0.0;
float presentTemperature0 = 0.0;
float presentTemperature1 = 0.0;
float presentTemperature2 = 0.0;
float presentTemperature3 = 0.0;

byte transmissionData = 0; // variable for sending data across the i2c bus

void setup()
{
  Wire.begin(); // join i2c bus (address optional for host)
  Serial.begin(9600);
}
byte a[4]; // stores the temporary byte information coming from the i2c peripheral float values 
byte b[4];
byte c[4];


void loop()
{
  lastRequestedEvent = (int)transmissionData; // explain what this is doing
  Serial.print("Requesting: ");
  Serial.println(transmissionData);
  Wire.beginTransmission(I2C_ADDRESS); // transmit to device #8
  Wire.write(transmissionData);                       // sends one byte
  Wire.endTransmission();              // stop transmitting

  switch (lastRequestedEvent)
  {
    case CURRENT_0:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from peripheral device #8
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

      union Temp // search up more about this , used to combine the 4 bytes received into a full float variable
      {
        byte temp_byte[4];
        float temp_float;
      } Temp_union;
          // Serial.print("data bytes:");
          // Serial.print(a[0]);
          // Serial.print(" ");
          // Serial.print(a[1]);
          // Serial.print(" ");
          // Serial.print(a[2]);
          // Serial.print(" ");
          // Serial.println(a[3]);
      Temp_union.temp_byte[0] = a[0];
      Temp_union.temp_byte[1] = a[1];
      Temp_union.temp_byte[2] = a[2];
      Temp_union.temp_byte[3] = a[3];

      if (Temp_union.temp_float >= 0)
      {
        presentCurrent0 = Temp_union.temp_float;
        Serial.print("Current 0: ");
        Serial.println(presentCurrent0);
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
//          Serial.print("data bytes:");
//          Serial.print(a[0]);
//          Serial.print(" ");
//          Serial.print(a[1]);
//           Serial.print(" ");
//           Serial.print(a[2]);
//           Serial.print(" ");
//           Serial.println(a[3]);
      Temp_union.temp_byte[0] = a[0];
      Temp_union.temp_byte[1] = a[1];
      Temp_union.temp_byte[2] = a[2];
      Temp_union.temp_byte[3] = a[3];
      if (Temp_union.temp_float > 0)
      {
        presentVoltage0 = Temp_union.temp_float;
        Serial.print("Voltage 0: ");
        Serial.println(presentVoltage0);
      }
    }
    break;
  case TEMPERATURE_0:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from peripheral device #8
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
          // Serial.print("data bytes:");
          // Serial.print(a[0]);
          // Serial.print(" ");
          // Serial.print(a[1]);
          // Serial.print(" ");
          // Serial.print(a[2]);
          // Serial.print(" ");
          // Serial.println(a[3]);
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
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from peripheral device #8
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
      // Serial.print("data bytes:");
      //     Serial.print(b[0]);
      //     Serial.print(" ");
      //     Serial.print(b[1]);
      //     Serial.print(" ");
      //     Serial.print(b[2]);
      //     Serial.print(" ");
      //     Serial.println(b[3]);
      Temp_union.temp_byte[0] = a[0];
      Temp_union.temp_byte[1] = a[1];
      Temp_union.temp_byte[2] = a[2];
      Temp_union.temp_byte[3] = a[3];
      if (Temp_union.temp_float > 0)
      {
        presentTemperature1 = Temp_union.temp_float;
        Serial.print("Temperature 1: ");
        Serial.println(presentTemperature1);
      }
    }
    break;
  case TEMPERATURE_2:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from peripheral device #8
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
      // Serial.print("data bytes:");
      //     Serial.print(c[0]);
      //     Serial.print(" ");
      //     Serial.print(c[1]);
      //     Serial.print(" ");
      //     Serial.print(c[2]);
      //     Serial.print(" ");
      //     Serial.println(c[3]);
      Temp_union.temp_byte[0] = a[0];
      Temp_union.temp_byte[1] = a[1];
      Temp_union.temp_byte[2] = a[2];
      Temp_union.temp_byte[3] = a[3];
      if (Temp_union.temp_float > 0)
      {
        presentTemperature2 = Temp_union.temp_float;
        Serial.print("Temperature 2: ");
        Serial.println(presentTemperature2);
      }
    }
    break;
   case TEMPERATURE_3:
    Wire.requestFrom(I2C_ADDRESS, 6); // request 6 bytes from peripheral device #8
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
          // Serial.print("data bytes:");
          // Serial.print(a[0]);
          // Serial.print(" ");
          // Serial.print(a[1]);
          // Serial.print(" ");
          // Serial.print(a[2]);
          // Serial.print(" ");
          // Serial.println(a[3]);
      Temp_union.temp_byte[0] = a[0];
      Temp_union.temp_byte[1] = a[1];
      Temp_union.temp_byte[2] = a[2];
      Temp_union.temp_byte[3] = a[3];
      if (Temp_union.temp_float > 0)
      {
        presentTemperature3 = Temp_union.temp_float;
        Serial.print("Temperature 3: ");
        Serial.println(presentTemperature3);
      }
    }
    break;
  }


  Serial.println(" ");
  transmissionData++;
  if (transmissionData > 7)
    transmissionData = 0;
  delay(2000);
}
