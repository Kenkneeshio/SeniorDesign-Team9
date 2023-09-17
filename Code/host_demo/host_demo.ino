/*
 * host_DEMO
 * Description: This code is example host code that can run on an arduino to demonstrate 
 * the capabilities of the Senior Design Team 9 microcontroller board.
 * It will ask for updates every 500 ms over i2C, and perform the calculation on the host. 
 */

#include <Wire.h> // i2C Library

#define I2C_ADDRESS       0x1C      // From sponsors requirements. They use 0x1C as the i2C address. 

// i2C Request codes. Each of these values are messages to the device to request certain information.
// These codes must be sent first along Wire.write() before initiating a Wire.requestFrom() function call. 
#define CURRENT           0
#define VOLTAGE           1
#define TEMPERATURE_0       2         
#define TEMPERATURE_1       3         
#define TEMPERATURE_2       4         

#define CURRENT_PIN       A0
#define VOLTAGE_PIN       A1

const float LOGIC_VOLTAGE = 5.08;
const float VOLTAGE_DIVIDER_R2 = 510000;
const float VOLTAGE_DIVIDER_R1 = 12000000;

float presentCurrent = 0.0;
float presentVoltage = 0.0;
float presentTemperature = 0.0;

float value;

float shuntResistor = 0.01028;
int loadResistor = 5000;



void setup() {
  Wire.begin(I2C_ADDRESS);        // join i2c bus (address optional for host)
  Serial.begin(9600);  // start serial for output

}


byte c[4];
void loop() {

  requestTemperature();
  delay(500);
  requestCurrent();
  delay(500);
  requestVoltage();
  delay(500);

}

/*
 * ADC2Current(float analogValue)
 * Description: Converts a given analog value from an ADC to the correct voltage based
 * the equation from the INA139 datasheet , where iS = (Vout * 1k) / (rs * rl).
 * Param: analogValue - ADC value
 */

float ADC2Current(float analogValue)
{
  float value;
  value = ((analogValue+1) / 1024 * LOGIC_VOLTAGE); // first convert the analog value based on the logic level voltage
  // then use the equation from the INA139 datasheet , where iS = (Vout * 1k) / (rs * rl)
  value = (value *(1000)/((loadResistor) *(shuntResistor))); 
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
  value = ((analogValue+1) / 1024 * LOGIC_VOLTAGE); // first convert the analog value based on the logic level voltage
  // then get back the input voltage from the voltage divider equation: Vin = Vout * (R2+R1)/R2
  value = (value *(VOLTAGE_DIVIDER_R2 + VOLTAGE_DIVIDER_R1)/(VOLTAGE_DIVIDER_R2)); 
  return value;
}

/*
 * requestVoltage(void)
 * Description: Requests voltage from peripheral on specified i2C address.
 * The variable presentVoltage will be updated after this function is called.
 * Param: None
 */

void requestVoltage()
{
  Serial.print("Requesting Voltage: ");
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(VOLTAGE);
  Wire.endTransmission();

  Wire.requestFrom(I2C_ADDRESS, 6);    // request 6 bytes from peripheral device #8
  int i = 0;
  while (Wire.available()) { // peripheral may send less than requested
    c[i] = Wire.read(); // receive a byte as character
    i++;
    //Serial.println(i);         // print the character
  }

  // Using a union is the only way so far to combine all of the individual bytes from the i2C bus. 
  union voltage {byte temp_byte[4]; float temp_float;}voltage_union;
  voltage_union.temp_byte[0] = c[0];
  voltage_union.temp_byte[1] = c[1];
  voltage_union.temp_byte[2] = c[2];
  voltage_union.temp_byte[3] = c[3];
  presentVoltage = voltage_union.temp_float;
  presentVoltage = ADC2Voltage(presentVoltage);
  Serial.print("Voltage (V):");
  Serial.println(presentVoltage);
}

/*
 * requestCurrent(void)
 * Description: Requests current from peripheral on specified i2C address.
 * The variable presentCurrent will be updated after this function is called.
 * Param: None
 */
 
void requestCurrent()
{
  Serial.print("Requesting Current: ");
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(CURRENT);
  Wire.endTransmission();

  Wire.requestFrom(I2C_ADDRESS, 6);    // request 6 bytes from peripheral device #8
  int i = 0;
  while (Wire.available()) { // peripheral may send less than requested
    c[i] = Wire.read(); // receive a byte as character
    i++;
    //Serial.println(i);         // print the character
  }

  // Using a union is the only way so far to combine all of the individual bytes from the i2C bus. 
  union current {byte temp_byte[4]; float temp_float;}current_union;
  current_union.temp_byte[0] = c[0];
  current_union.temp_byte[1] = c[1];
  current_union.temp_byte[2] = c[2];
  current_union.temp_byte[3] = c[3];
  presentCurrent = current_union.temp_float;
  presentCurrent = ADC2Current(presentCurrent);
  Serial.print("Current (A):");
  Serial.println(presentCurrent);
}

/*
 * requestTemperature(void)
 * Description: Requests temperature from peripheral on specified i2C address.
 * The variable presentTemperature will be updated after this function is called.
 * Param: None
 */
 
void requestTemperature()
{
  Serial.print("Requesting Temperature: ");
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(TEMPERATURE);
  Wire.endTransmission();
  
  Wire.requestFrom(I2C_ADDRESS, 6);    // request 6 bytes from peripheral device #8
  int i = 0;
  while (Wire.available()) { // peripheral may send less than requested
    c[i] = Wire.read(); // receive a byte as character
    i++;
    //Serial.println(i);         // print the character
  }

  // Using a union is the only way so far to combine all of the individual bytes from the i2C bus.   
  union Temp {byte temp_byte[4]; float temp_float;}Temp_union;
  Temp_union.temp_byte[0] = c[0];
  Temp_union.temp_byte[1] = c[1];
  Temp_union.temp_byte[2] = c[2];
  Temp_union.temp_byte[3] = c[3];
  presentTemperature = Temp_union.temp_float;
  Serial.print("Temperature (C):");
  Serial.println(presentTemperature);
}
