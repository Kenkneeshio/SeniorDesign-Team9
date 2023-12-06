

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
const int SYSTEM_NOP = 100;
const int SYSTEM_RESET_VAL = 101;
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

byte transmissionData = -1; // variable for sending data across the i2c bus
byte data[4];              // stores the temporary byte information coming from the i2c peripheral float values
int lastRequestedEvent = -1;

int MEASUREMENT_INTERVAL = 500;

union floatByte // two different data types in one area of memory.
{
  byte byteVal[4];
  float floatVal;
};

//////////////////////////////////////////////////////////
// Debug
// Enable this for logging of all messages to the Serial monitor.
bool debug = false;

void setup()
{
  Wire.begin();       // join i2c bus (address optional for host)
  Serial.begin(9600); // for this demo, keep serial monitor on regardless
  BootScreen();
  Serial.println(" ");
  // When the host resets, or powers on, tell the peripheral to reset it's stored value of the requested measurement.
  Wire.beginTransmission(I2C_ADDRESS); // transmit to our intended device along the bus
  Wire.write(SYSTEM_RESET_VAL);                 // send the currently requested sensing value
  Wire.endTransmission();              // now stop transmitting
  Serial.println("Please enter one of the following:");
  Serial.println("0 - Current, 2 - Battery 1, 3 - Battery 2");
  Serial.println("4 - Battery 1 Temperature, 5 Battery 2 Temperature");
  Serial.println("6 - Air Temperature, 7 - Heatsink Temperature");
  Serial.println(" ");
}

void loop()
{
  static uint32_t millis_ctr = 0; // To not use delay(), we use a timer counter based on the clock.
  if (millis() > millis_ctr + MEASUREMENT_INTERVAL) // take a measurement every 10 seconds. 
  {

    transmissionData = Serial.parseInt(); // explain what this is doing
    if (Serial.read() == '\n') {
    Serial.print("Requesting: ");
    Serial.println(transmissionData);
    Wire.beginTransmission(I2C_ADDRESS); // transmit to our intended device along the bus
    Wire.write(transmissionData);        // send the currently requested sensing value
    Wire.endTransmission();              // now stop transmitting

    switch (transmissionData)
    {
    case CURRENT_0:
      Wire.requestFrom(I2C_ADDRESS, 4); // request 6 bytes from peripheral device #8
      while (Wire.available())
      { // peripheral may send less than requested

        int i = 0;
        while (i < 4)
        {                        // peripheral may send less than requested
          data[i] = Wire.read(); // receive a byte as character
          i++;
        }
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
      if (!isnan(current.floatVal) && current.floatVal > 0)
      {
        presentCurrent0 = current.floatVal;
        Serial.print("Current 0: ");
        Serial.println(presentCurrent0);
      }
      else
      {
        // Serial.println("Invalid Data received: ");
        // Serial.print(data[0]);
        // Serial.print(" ");
        // Serial.print(data[1]);
        // Serial.print(" ");
        // Serial.print(data[2]);
        // Serial.print(" ");
        // Serial.println(data[3]);
        Serial.println("Unable to read below 1.5A.");
      }

      break;
    case CURRENT_1:
      Wire.requestFrom(I2C_ADDRESS, 4); // request 6 bytes from peripheral device #8
      while (Wire.available())
      { // peripheral may send less than requested

        int i = 0;
        while (i < 4)
        {                        // peripheral may send less than requested
          data[i] = Wire.read(); // receive a byte as character
          i++;
        }
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

      union floatByte current1;
      current1.byteVal[0] = data[0];
      current1.byteVal[1] = data[1];
      current1.byteVal[2] = data[2];
      current1.byteVal[3] = data[3];
      if (!isnan(current1.floatVal) && current1.floatVal > 0)
      {
        presentCurrent1 = current1.floatVal;
        Serial.print("Current 1: ");
        Serial.println(presentCurrent1);
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

      break;

    case VOLTAGE_0:
      Wire.requestFrom(I2C_ADDRESS, 4); // request 4 bytes from peripheral device #8
      while (Wire.available())
      { // peripheral may send less than requested
        int i = 0;
        while (i < 4)
        {                        // peripheral may send less than requested
          data[i] = Wire.read(); // receive a byte as character
          i++;
        }
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
      if (!isnan(voltage.floatVal) && voltage.floatVal > 0)
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

      break;

    case VOLTAGE_1:
      Wire.requestFrom(I2C_ADDRESS, 4); // request 4 bytes from peripheral device #8
      while (Wire.available())
      { // peripheral may send less than requested
        int i = 0;
        while (i < 4)
        {                        // peripheral may send less than requested
          data[i] = Wire.read(); // receive a byte as character
          i++;
        }
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

      union floatByte voltage1;
      voltage1.byteVal[0] = data[0];
      voltage1.byteVal[1] = data[1];
      voltage1.byteVal[2] = data[2];
      voltage1.byteVal[3] = data[3];
      if (!isnan(voltage1.floatVal) && voltage1.floatVal > 0)
      {
        presentVoltage1 = voltage1.floatVal;
        Serial.print("Voltage 1: ");
        Serial.println(presentVoltage1);
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

      break;

    case TEMPERATURE_0:
      Wire.requestFrom(I2C_ADDRESS, 4); // request 4 bytes from peripheral device #8
      while (Wire.available())
      { // peripheral may send less than requested
        int i = 0;
        while (i < 4)
        {                        // peripheral may send less than requested
          data[i] = Wire.read(); // receive a byte as character
          i++;
        }
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

      union floatByte temperature0;
      temperature0.byteVal[0] = data[0];
      temperature0.byteVal[1] = data[1];
      temperature0.byteVal[2] = data[2];
      temperature0.byteVal[3] = data[3];
      if (!isnan(temperature0.floatVal) && temperature0.floatVal > 0)
      {
        presentTemperature0 = temperature0.floatVal;
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
      break;
    case TEMPERATURE_1:
      Wire.requestFrom(I2C_ADDRESS, 4); // request 6 bytes from peripheral device #8
      while (Wire.available())
      { // peripheral may send less than requested
        int i = 0;
        while (i < 4)
        {                        // peripheral may send less than requested
          data[i] = Wire.read(); // receive a byte as character
          i++;
        }
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

      union floatByte temperature1;
      temperature1.byteVal[0] = data[0];
      temperature1.byteVal[1] = data[1];
      temperature1.byteVal[2] = data[2];
      temperature1.byteVal[3] = data[3];
      if (!isnan(temperature1.floatVal) && temperature1.floatVal > 0)
      {
        presentTemperature1 = temperature1.floatVal;
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
      break;
    case TEMPERATURE_2:
      Wire.requestFrom(I2C_ADDRESS, 4); // request 6 bytes from peripheral device #8
      while (Wire.available())
      { // peripheral may send less than requested
        int i = 0;
        while (i < 4)
        {                        // peripheral may send less than requested
          data[i] = Wire.read(); // receive a byte as character
          i++;
        }
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

      union floatByte temperature2;
      temperature2.byteVal[0] = data[0];
      temperature2.byteVal[1] = data[1];
      temperature2.byteVal[2] = data[2];
      temperature2.byteVal[3] = data[3];
      if (!isnan(temperature2.floatVal) && temperature2.floatVal > 0)
      {
        presentTemperature2 = temperature2.floatVal;
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
      break;
    case TEMPERATURE_3:
      Wire.requestFrom(I2C_ADDRESS, 4); // request 6 bytes from peripheral device #8
      while (Wire.available())
      { // peripheral may send less than requested
        int i = 0;
        while (i < 4)
        {                        // peripheral may send less than requested
          data[i] = Wire.read(); // receive a byte as character
          i++;
        }
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

      union floatByte temperature3;
      temperature3.byteVal[0] = data[0];
      temperature3.byteVal[1] = data[1];
      temperature3.byteVal[2] = data[2];
      temperature3.byteVal[3] = data[3];
      if (!isnan(temperature3.floatVal) && temperature3.floatVal > 0)
      {
        presentTemperature3 = temperature3.floatVal;
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
      break;
    }

    Serial.println(" ");
    Serial.println("Please enter one of the following:");
    Serial.println("0 - Current, 2 - Battery 1, 3 - Battery 2");
    Serial.println("4 - Battery 1 Temperature, 5 Battery 2 Temperature");
    Serial.println("6 - Air Temperature, 7 - Heatsink Temperature");
    Serial.println(" ");
    //WaitFor(2000); // wait for 2 seconds before requesting another measurement
    }
    

    millis_ctr = millis();
  }
  else
  {
    // while waiting for another measurement, send the NOP command to ensure that the remote device knows we have connection.
    Wire.beginTransmission(I2C_ADDRESS); // transmit to our intended device along the bus
    Wire.write(SYSTEM_NOP);        // send the currently requested sensing value
    Wire.endTransmission();              // now stop transmitting
    WaitFor(1000); // delay for 100 ms 
  }
}

void BootScreen(void)
{
  Serial.println("//////////////////////////////////////////////////////////");
  Serial.println("SDSU Senior Design Team 9 - Robot Power System");
  Serial.println("Boot - Host Demonstration");
  Serial.println("Please connect RPS01 to SDA, SCL, and GND.");
  Serial.println("//////////////////////////////////////////////////////////");
}

void WaitFor(float milliseconds)
{
  float waitCounter = millis();
  while (millis() < waitCounter + milliseconds)
  {
    // wait for 200 ms and do nothing, getting around delay()
  }
}
