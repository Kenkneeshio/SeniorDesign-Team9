//////////////////////////////////////////////////////////
// SDSU Senior Design Team 9 - Robot Power System
// peripheral.ino
// Desc:
//////////////////////////////////////////////////////////

#include <Wire.h>              // used for i2c communication
#include <OneWire.h>           // used for temperature sensor one wire
#include <DallasTemperature.h> // temperature sensor library

#define ONE_WIRE_BUS 5 // THE DIGITAL PIN THAT THE TEMPERATURE SENSORS ARE CONNECTED TO
//////////////////////////////
// FINAL BOARD PINS
#define VOLTAGE0_PIN A0
#define VOLTAGE1_PIN A1
#define CURRENT0_PIN A2
#define CURRENT1_PIN A3 // Reserved for later
/////////////////////////////

//////////////////////////////
// PROTO BOARD PINS
// #define VOLTAGE0_PIN A0
// #define VOLTAGE1_PIN A2
// #define CURRENT0_PIN A1
// #define CURRENT1_PIN A3
/////////////////////////////

//////////////////////////////////////////////////////////
// i2C ASSIGNMENTS
int I2C_ADDRESS = 8;         // Example i2C address
int lastRequestedEvent = -1; // This is the last requested event stored. This could be any one of the codes below:
//////////////////////////////////////////////////////////
// i2C Request codes. Each of these values are messages to the device to request certain information.
// These codes must be sent first along Wire.write() before initiating a Wire.requestFrom() function call.
//////////////////////////////////////////////////////////
const int CURRENT_0 = 0;
const int CURRENT_1 = 1; // Reserved for later
const int VOLTAGE_0 = 2;
const int VOLTAGE_1 = 3;
const int TEMPERATURE_0 = 4;
const int TEMPERATURE_1 = 5;
const int TEMPERATURE_2 = 6;
const int TEMPERATURE_3 = 7;
const int SYSTEM_NOP = 100;
const int SYSTEM_RESET_VAL = 101;
//////////////////////////////////////////////////////////
// LED PIN ASSIGNMENTS
const int LED_PIN_RED = 3;
const int LED_PIN_GREEN = 10;
const int LED_PIN_BLUE = 9;
/////////////////////////////////////
// LED SYSTEM HEALTH CONDITIONS
const int RED = 0;    // COMMUNICATION LOSS
const int GREEN = 1;  // ALL GOOD
const int BLUE = 2;   // BATTERY 1 BAD
const int PURPLE = 3; // BATTERY 2 BAD
const int TEAL = 4;   // RESERVED
const int YELLOW = 5; // TEMPERATURE FAULT
const int WHITE = 6;  // BOOT SEQUENCE INDICATOR
const int OFF = 7;    //
//////////////////////////////////////////////////////////
const float LOGIC_VOLTAGE = 4.979;
const float BATT2_VOLTAGE_DIVIDER_R2 = 509940;
const float BATT2_VOLTAGE_DIVIDER_R1 = 1199990;
const float BATT1_VOLTAGE_DIVIDER_R2 = 509870;
const float BATT1_VOLTAGE_DIVIDER_R1 = 1199850;
const float SHUNT_RESISTOR = 0.000339290391;
const float LOAD_RESISTOR = 150186;

//////////////////////////////////////////////////////////
// DallasTemperature Sensor Serial Numbers
// Do not change the short serial numbers, as you will need to desolder them from the board.

DeviceAddress temperatureProbe0_LONG = {0x28, 0x0D, 0x7E, 0x81, 0xE3, 0x69, 0x3C, 0x1B}; // Serial Number for External Temperature Probe
DeviceAddress temperatureProbe1_LONG = {0x28, 0x47, 0x3F, 0x81, 0xE3, 0x9F, 0x3C, 0xB6}; // Serial Number for External Temperature Probe
DeviceAddress temperatureProbe2_SHORT = {0x28, 0x5B, 0x8A, 0x81, 0xE3, 0xF8, 0x3C, 0x39}; // Serial Number for Internal Temperature Probe
DeviceAddress temperatureProbe3_SHORT = {0x28, 0x37, 0xCF, 0x81, 0xE3, 0xAF, 0x3C, 0xA5}; // Serial Number for Internal Temperature Probe

//////////////////////////////////////////////////////////
int tempSensorsAtBoot; // Number of temperature devices found
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress DallasTemperatureDevice; // Temporary device address variable for use with the getAddress function. 

//////////////////////////////////////////////////////////
// Present Values
// These floats represent the present current, voltage, or temperature value
// from the ADC.
float presentCurrent0 = 0.0;
float presentCurrent1 = 0.0; // Reserved for later
float presentVoltage0 = 0.0;
float presentVoltage1 = 0.0;
float presentTemperature0f = 0.0;
float presentTemperature1f = 0.0;
float presentTemperature2f = 0.0;
float presentTemperature3f = 0.0;
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// Time Values
// These values store how long it was in  milisceonds since the last communication with the host.
float timeSinceCommunicated;
int LED_KEEP_ON_TIME = 500;      // keep any LED state on for 500ms
int TIME_SINCE_COMMUNICATION_INTERVAL = 2500; // 2.5 seconds

//////////////////////////////////////////////////////////
// Debug
// Enable this for logging of all messages to the Serial monitor.
bool debug = true;

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
  value = analogValue * (LOGIC_VOLTAGE / 1023.0); // first convert the analog value based on the logic level voltage
  // then use the equation from the INA139 datasheet , where iS = (Vout * 1k) / (rs * rl)
  value = (value * (1000) / ((LOAD_RESISTOR) * (SHUNT_RESISTOR)));
  return value;
}

//////////////////////////////////////////////////////////
/*
 * ADC2Voltage(float analogValue, int batteryNumber)
 * Description: Converts a given analog value from an ADC to the correct voltage based
 * on the voltage divider resistor values, and output voltage from the ADC to recover the input.
 * Param: analogValue - ADC
 *        batteryNumber - battery number. Can be either 1 or 2.
 */
//////////////////////////////////////////////////////////

float ADC2Voltage(float analogValue, int batteryNumber)
{
  float value;
  value = analogValue * (LOGIC_VOLTAGE / 1023.0); // first convert the analog value based on the logic level voltage
  // then get back the input voltage from the voltage divider equation: Vin = Vout * (R2+R1)/
  switch (batteryNumber)
  {
  case 1:
    value = (value * (BATT1_VOLTAGE_DIVIDER_R2 + BATT1_VOLTAGE_DIVIDER_R1) / (BATT1_VOLTAGE_DIVIDER_R2));
    break;

  case 2:
    value = (value * (BATT2_VOLTAGE_DIVIDER_R2 + BATT2_VOLTAGE_DIVIDER_R1) / (BATT2_VOLTAGE_DIVIDER_R2));
    break;
  }

  return value;
}

//////////////////////////////////////////////////////////
/*
 * CollectTemperatureInformation(void)
 * Description: This function will reinitialize how many sensors are on the bus, and get the number of them to compare with the amount at boot.
 * If there are fewer, then the diagnostic LED is set to yellow for 500ms. After this check, each known serial number probe gets the temperature
 * collected from and stored into a global variable for that measurement. 
 * Param:
 */
//////////////////////////////////////////////////////////
void CollectTemperatureInformation(void)
{
  sensors.begin(); // must call sensors.begin() each time to reinitalize how many sensors are on the bus
  if (sensors.getDeviceCount() < tempSensorsAtBoot)
  {
    // if we lost a temperature sensor, then turn on the YELLOW LED for 500ms to indicate an issue,
    // then pass on the led cycle to the next state
    float temperature_ctr = millis();
    while (millis() < temperature_ctr + (float)LED_KEEP_ON_TIME) // Keep LED on for 500ms
    {
      SetLEDColour(YELLOW);
    }
    Serial.println("WARNING: Lost connection to one or more temperature sensors. Double check connections.");
  }
  
    // First have the library tell sensors to collect measurements. 
    // Then obtain the temperature in celsius by passing the serial number as a parameter. 
    // Store this value into the respective global variable.
    // Finally, check the value if it is the disconnected value of -127.
    // If it is, then reset the value to zero.

    sensors.requestTemperatures();
    presentTemperature0f = sensors.getTempC(temperatureProbe0_LONG); // request temperature in celsius
    if(presentTemperature0f == DEVICE_DISCONNECTED_C)
    {
      presentTemperature0f = 0.0f;
    }

    presentTemperature1f = sensors.getTempC(temperatureProbe1_LONG); // request temperature in celsius
    if(presentTemperature1f == DEVICE_DISCONNECTED_C)
    {
      presentTemperature1f = 0.0f;
    }

    presentTemperature2f = sensors.getTempC(temperatureProbe2_SHORT); // request temperature in celsius
    if(presentTemperature2f == DEVICE_DISCONNECTED_C)
    {
      presentTemperature2f = 0.0f;
    }

    presentTemperature3f = sensors.getTempC(temperatureProbe3_SHORT); // request temperature in celsius
    if(presentTemperature3f == DEVICE_DISCONNECTED_C)
    {
      presentTemperature3f = 0.0f;
    }

  if (debug)
     {
          Serial.print("Battery 1 - Temp C: ");
          Serial.println(presentTemperature0f);
          Serial.print("Battery 2 - Temp C: ");
          Serial.println(presentTemperature1f);
          Serial.print("Heatsink - Temp C: ");
          Serial.println(presentTemperature2f);
          Serial.print("Enclosure - Temp C: ");
          Serial.println(presentTemperature3f);
     }
}

//////////////////////////////////////////////////////////
/*
 * SetLEDState(int colour)
 * Description: Sets LED color based on input parameter. Refer to top of code for reference. 
 * Param: int colour
 */
//////////////////////////////////////////////////////////

void SetLEDColour(int colour)
{
  switch (colour)
  {
  case RED:
    digitalWrite(LED_PIN_RED, HIGH);  // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_GREEN, LOW); // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_BLUE, LOW);  // turn the LED on (HIGH is the voltage level)
    break;
  case GREEN:
    digitalWrite(LED_PIN_RED, LOW);    // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_GREEN, HIGH); // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
    break;
  case BLUE:
    digitalWrite(LED_PIN_RED, LOW);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_GREEN, LOW); // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_BLUE, HIGH); // turn the LED on (HIGH is the voltage level)
    break;
  case YELLOW:
    digitalWrite(LED_PIN_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_GREEN, HIGH); // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
    break;
  case PURPLE:
    digitalWrite(LED_PIN_RED, HIGH);  // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_GREEN, LOW); // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_BLUE, HIGH); // turn the LED on (HIGH is the voltage level)
    break;
  case TEAL:
    digitalWrite(LED_PIN_RED, LOW);    // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_GREEN, HIGH); // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_BLUE, HIGH);  // turn the LED on (HIGH is the voltage level)
    break;
  case WHITE:
    digitalWrite(LED_PIN_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_GREEN, HIGH); // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_BLUE, HIGH);  // turn the LED on (HIGH is the voltage level)
    break;
  case OFF:
    digitalWrite(LED_PIN_RED, LOW);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_GREEN, LOW); // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_BLUE, LOW);  // turn the LED on (HIGH is the voltage level)
    break;
  }
}

//////////////////////////////////////////////////////////
/*
 * requestEvent()
 * Description: The i2C interrupt function that is called when the host sends data to the peripheral.
 * Here we are checking to see what type of command it is. 
 * Param: int howMany
 */
//////////////////////////////////////////////////////////

void receiveEvent(int howMany)
{
  int wireRead = Wire.read(); // read the data from the Wire bus

  if (wireRead == SYSTEM_RESET_VAL) // if we receive the reset command, make sure to reset the lastrequestedevent to -1. 
  {
    lastRequestedEvent = -1;
    timeSinceCommunicated = millis(); // update the time since we last received a command
  }
  if (wireRead == SYSTEM_NOP) // if we receive the system NOP command, just update the time recevied variable only
  {
    // Serial.println("NOP Command");
    timeSinceCommunicated = millis(); // update the time since we last received a command
  }
  else // if(Wire.read() != -1) but if we get data we are expecting, then store that requested event
  {
    lastRequestedEvent = wireRead; 
    timeSinceCommunicated = millis(); // update the time since we last received a command
  }

//  if (debug) // if debug is enabled, print out the last requested event type to serial monitor
//  {
//    switch (lastRequestedEvent)
//    {
//    case CURRENT_0:
//      Serial.println("Current 0");
//      break;
//    case CURRENT_1:
//      Serial.println("Current 1");
//      break;
//    case VOLTAGE_0:
//      Serial.println("Voltage 0");
//      break;
//    case VOLTAGE_1:
//      Serial.println("Voltage 1");
//      break;
//    case TEMPERATURE_0:
//      Serial.println("Temperature 0");
//      break;
//    case TEMPERATURE_1:
//      Serial.println("Temperature 1");
//      break;
//    case TEMPERATURE_2:
//      Serial.println("Temperature 2");
//      break;
//    case TEMPERATURE_3:
//      Serial.println("Temperature 3");
//      break;
//    }
//  }
}

//////////////////////////////////////////////////////////
/*
 * requestEvent()
 * Description: The i2C interrupt function that is called when the host requests data from the peripheral.
 * Param:
 */
//////////////////////////////////////////////////////////
void requestEvent()
{
  // Since all of the data we are reading is in floats, we have to send
  // the values in a total of four bytes, one at a time. On the host end,
  // it will have to reconstruct the float from the bytes sent.
  timeSinceCommunicated = millis();  // update the time since we last received a command
  byte *data;
  switch (lastRequestedEvent)
  {
  case CURRENT_0:
    data = (byte *)&presentCurrent0; // casting as a byte pointer to address of variable
    break;
  case CURRENT_1: // Reserved for later
    data = (byte *)&presentCurrent1; // casting as a byte pointer to address of variable
    break;
  case VOLTAGE_0:
    data = (byte *)&presentVoltage0; // casting as a byte pointer to address of variable
    break;
  case VOLTAGE_1:
    data = (byte *)&presentVoltage1; // casting as a byte pointer to address of variable
    break;
  case TEMPERATURE_0:
    data = (byte *)&presentTemperature0f; // casting as a byte pointer to address of variable
    break;
  case TEMPERATURE_1:
    data = (byte *)&presentTemperature1f; // casting as a byte pointer to address of variable
    break;
  case TEMPERATURE_2:
    data = (byte *)&presentTemperature2f; // casting as a byte pointer to address of variable
    break;

  case TEMPERATURE_3:
    data = (byte *)&presentTemperature3f; // casting as a byte pointer to address of variable
    break;
  }

  // if (debug) // if debug enabled, the hex of the code about to be sent will print
  // {
  //   Serial.print("write: ");
  //   Serial.print(data[0]);
  //   Serial.print(" ");
  //   Serial.print(data[1]);
  //   Serial.print(" ");
  //   Serial.print(data[2]);
  //   Serial.print(" ");
  //   Serial.println(data[3]);
  // }

  // because a float is four bytes, we have to send each byte one at a time
  Wire.write(data[0]);
  Wire.write(data[1]);
  Wire.write(data[2]);
  Wire.write(data[3]);
}

//////////////////////////////////////////////////////////
/*
 * setup()
 * Description: Sets up the functionality of the firmware. This includes the LEDS, Serial, Temperature sensors, and the i2C communication.
 */
//////////////////////////////////////////////////////////
void setup()
{
  // First set up LED state
  SetupLEDPins();
  // Set up Serial Communication.
  // Putting the Serial monitor here should help the MCU resetting when the com port is accessed.
  Serial.begin(9600); // start serial for output
  BootScreen();
  // Set up the Dallas Temperature sensors
  SetupDallasTemperatureSensors();
  //WaitFor(150); // let system catch up 
  // Set up i2C Communication
  SetupI2CCommunication();
  //WaitFor(150); // let system catch up 
  Serial.println("Entering main loop...");
}

//////////////////////////////////////////////////////////
/*
 * loop()
 * Description: The main loop. A timer is created to continually collect data and check validity of data.
 */
//////////////////////////////////////////////////////////
void loop()
{
  static uint32_t millis_ctr = 0; // To not use delay(), we use a timer counter based on the clock.

  if (millis() > millis_ctr)
  {
    presentCurrent0 = ADC2Current(analogRead(CURRENT0_PIN));    // read analog value from adc, and pass to ADC2Current to convert into a real current
    presentVoltage0 = ADC2Voltage(analogRead(VOLTAGE0_PIN), 1); // read analog value from adc, and pass to ADC2Voltage to convert into a real voltage
    presentVoltage1 = ADC2Voltage(analogRead(VOLTAGE1_PIN), 2); // read analog value from adc, and pass to ADC2Voltage to convert into a real voltage
    
    CollectTemperatureInformation();                            // call the temperature collecting function
    CheckBattery1Voltage(millis_ctr);                           // call the function to check battery one's health
    CheckBattery2Voltage(millis_ctr);                            // call the function to check battery two's health
    CheckHostCommunication();                                   // call the function to check the communication state with the host

    if (debug)
    {
//      Serial.print("Current 0 ADC: ");
//      Serial.println(analogRead(CURRENT0_PIN));
      Serial.print("Current 0 Conversion: ");
      Serial.println(presentCurrent0);
      Serial.print("Voltage 0: ");
      Serial.println(presentVoltage0);
      Serial.print("Voltage 1: ");
      Serial.println(presentVoltage1);
    }

    millis_ctr = millis();
  }
}

//////////////////////////////////////////////////////////
/*
 * CheckBattery1Voltage()
 * Description: This function checks the battery voltage to determine if it is undervoltage or overvoltage.
 * If there is an error, then the LED is set to blue for 500ms.
 * Param: uint32_t counter
 */
//////////////////////////////////////////////////////////
void CheckBattery1Voltage(uint32_t counter)
{
  if (12 > presentVoltage0 || presentVoltage0 > 16.8) // if battery voltage is greater than zero but less than 16.8
  {
    // Battery should not be less than 12V or greater than 16.8V
    counter = millis();

    Serial.println("Battery 1 Error: under or overvoltage");
    while (millis() < counter + (float)LED_KEEP_ON_TIME) // Keep LED on for 500ms
    {
      SetLEDColour(BLUE);
    }
  }
}

//////////////////////////////////////////////////////////
/*
 * CheckBattery2Voltage()
 * Description: This function checks the battery voltage to determine if it is undervoltage or overvoltage.
 * If there is an error, then the LED is set to blue for 500ms.
 * Param: uint32_t counter
 */
//////////////////////////////////////////////////////////
void CheckBattery2Voltage(uint32_t counter)
{
  if (12 > presentVoltage1 || presentVoltage1 > 16.8) // if battery voltage is greater than zero but less than 16.8
  {
    // Battery should not be less than 12V or greater than 16.8V
    counter = millis();

    Serial.println("Battery 2 Error: under or overvoltage");
    while (millis() < counter + (float)LED_KEEP_ON_TIME) // Keep LED on for 500ms
    {

      SetLEDColour(PURPLE);
    }
  }
}

//////////////////////////////////////////////////////////
/*
 * CheckHostCommunication()
 * Description: This function checks the last time the device has received a command from the host.
 * If there is an error, then the LED is set to RED for 500ms. Otherwise, the LED is set to GREEN.
 * Param: 
 */
//////////////////////////////////////////////////////////
void CheckHostCommunication()
{
  if (millis() > timeSinceCommunicated + TIME_SINCE_COMMUNICATION_INTERVAL)
  {
    // time has passed 2.5 seconds since the last received value.
    // this should have changed by now, but it has not, so indicate an error occured
    Serial.println("Communication to Host: BAD");
    SetLEDColour(RED); // Set the led red to indicate haven't received a command
  }
  else // this should be the last case if no error was reported
  {

    Serial.println("Communication to Host: OK");
    SetLEDColour(GREEN);
  }
}


//////////////////////////////////////////////////////////
/*
 * DisplayDallasTemperatureSerialNumbers()
 * Description: A modified function from the DallasTemperature example library to print the serial numbers
 * of each sensor found on the bus. 
 * Param: 
 */
//////////////////////////////////////////////////////////
void DisplayDallasTemperatureSerialNumbers(void)
{
  for (uint8_t i = 0; i < tempSensorsAtBoot; i++)
  {
    if (sensors.getAddress(DallasTemperatureDevice, i))
    {
      for (uint8_t i = 0; i < 8; i++)
      {
        if (DallasTemperatureDevice[i] < 16)
          Serial.print("0");
        Serial.print(DallasTemperatureDevice[i], HEX);
      }
      Serial.println(" ");
    }
  }
}

//////////////////////////////////////////////////////////
/*
 * DisplayDallasTemperatureSerialNumbers()
 * Description: A modified function from the DallasTemperature example library to print the serial numbers
 * of each sensor found on the bus. 
 * Param: 
 */
//////////////////////////////////////////////////////////
void SetupLEDPins(void)
{
    // Set the LEDs to output mode
  Serial.print("Setting LED Outputs...");
  pinMode(LED_PIN_RED, OUTPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);
  pinMode(LED_PIN_BLUE, OUTPUT);
  Serial.println("Done.");
  SetLEDColour(WHITE); 
}

//////////////////////////////////////////////////////////
/*
 * SetupDallasTemperatureSensors()
 * Description: Sets up the temperature sensors by obtaining device count, displaying the sensor serial numbers
 * and setting the resolution.
 * Param: 
 */
//////////////////////////////////////////////////////////
void SetupDallasTemperatureSensors(void)
{
  // Set up Dallas Temperature Sensors
  Serial.println("Starting DallasTemperature Library....");
  sensors.begin(); // start up the dallas temperature library
  Serial.println("Finding DallasTemperature devices....");
  tempSensorsAtBoot = sensors.getDeviceCount(); // number of temperatures
  DisplayDallasTemperatureSerialNumbers();
  Serial.println("Done.");
  Serial.print("Setting probes to 12 bit resolution....");
  sensors.setResolution(temperatureProbe0_LONG, 12);
  sensors.setResolution(temperatureProbe1_LONG, 12);
  sensors.setResolution(temperatureProbe2_SHORT, 12);
  sensors.setResolution(temperatureProbe3_SHORT, 12);
  Serial.println("Done.");
}

//////////////////////////////////////////////////////////
/*
 * SetupI2CCommunication()
 * Description: Sets up the i2C functionality. This includes joining the address, and the interrupt functions.
 * Param: 
 */
//////////////////////////////////////////////////////////
void SetupI2CCommunication(void)
{
  Serial.print("Joining i2C Address:");
  Serial.print(I2C_ADDRESS);
  Serial.print(".....");
  Wire.begin(I2C_ADDRESS); // join i2c bus with address #8
  Serial.println("Done.");
  //WaitFor(150); // let system catch up 
  Serial.print("Setting i2C ReceiveEvent....");
  Wire.onReceive(receiveEvent); // register a receive on event
  Serial.println("Done.");
  //WaitFor(150); // let system catch up 
  Serial.print("Setting i2C RequestEvent....");
  Wire.onRequest(requestEvent); // register a request on event
  Serial.println("Done.");
}

//////////////////////////////////////////////////////////
/*
 * BootScreen()
 * Description: Prints boot screen logo.
 * Param: 
 */
//////////////////////////////////////////////////////////
void BootScreen(void)
{
  Serial.println("//////////////////////////////////////////////////////////");
  Serial.println("SDSU Senior Design Team 9 - Robot Power System");
  Serial.println("Boot");
  Serial.println("//////////////////////////////////////////////////////////");
}

//////////////////////////////////////////////////////////
/*
 * WaitFor()
 * Description: A timer function to create a timer for X amount of milliseconds.
 * Param: float milliseconds
 */
//////////////////////////////////////////////////////////
void WaitFor(float milliseconds)
{
  float waitCounter = millis();
  while (millis() < waitCounter + milliseconds)
  {
    // wait for 200 ms and do nothing, getting around delay()
  }
}
