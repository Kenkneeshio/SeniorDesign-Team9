//////////////////////////////////////////////////////////
// SDSU Senior Design Team 9 - Robot Power System
// peripheral.ino
// Desc:
//////////////////////////////////////////////////////////

#include <Wire.h> // used for i2c communication
#include <OneWire.h> // used for temperature sensor one wire 
#include <DallasTemperature.h> // temperature sensor library

#define ONE_WIRE_BUS 5 // THE DIGITAL PIN THAT THE TEMPERATURE SENSORS ARE CONNECTED TO
#define VOLTAGE0_PIN A0
#define VOLTAGE1_PIN A1
#define CURRENT0_PIN A2
#define CURRENT1_PIN A3

//#define 5V_RAIL_IN XX
//#define 3V_RAIL_IN XY
//#define 19V_RAIL_IN YX

//////////////////////////////////////////////////////////
// i2C ASSIGNMENTS
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
// LED PIN ASSIGNMENTS
const int LED_PIN_RED   = 3;
const int LED_PIN_GREEN = 10;
const int LED_PIN_BLUE  = 9;
/////////////////////////////////////
// LED SYSTEM HEALTH CONDITIONS
const int RED = 0; // COMMUNICATION LOSS
const int GREEN = 1; // ALL GOOD
const int BLUE = 2; // BATTERY 1 BAD
const int PURPLE = 3; // BATTERY 2 BAD
const int TEAL = 4; // TEMPERATURE FAULT 
const int ORANGE = 5;  //TWO BATTERY ARE NOT BALANCED
const int WHITE = 6;  //CURRENT 1
const int OFF = 7;   //
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
//const uint8_t temperatureProbe2_SHORT[8] = {0x28, 0xFE, 0x8D, 0x81, 0xE3, 0x73, 0x3C, 0xD3}; //on demo board
//const uint8_t temperatureProbe3_SHORT[8] = {0x28, 0x9C, 0x56, 0x81, 0xE3, 0xD9, 0x3C, 0x59}; //on demo board

const uint8_t temperatureProbe2_SHORT[8] = {0x28, 0x3A, 0x81, 0x81, 0xE3, 0xF1, 0x3C, 0x0F};
const uint8_t temperatureProbe3_SHORT[8] = {0x28, 0x26, 0x2F, 0x81, 0xE3, 0x6C, 0x3C, 0xF2};
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

//////////////////////////////////////////////////////////
// Time Values
// These values store how long it was in  milisceonds since the last communication with the host.
float timeSinceReceived;
float timeSinceRequested;
int LED_KEEP_ON_TIME = 500; // keep any LED state on for 500ms
int LED_RAINBOW_DURATION = 1000; // keep any LED state on for 500ms


bool BATTERY1_FALUT = false;
bool BATTERY2_FALUT = false;
bool COMMUNICATION_FAULT = false;
bool CURRENT_FAULT = false;

//////////////////////////////////////////////////////////
// Debug
// Enable this for logging of all messages to the Serial monitor. 
bool debug = debug;

//float value;

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

//////////////////////////////////////////////////////////
/*
 * CollectTemperatureInformation(void)
 * Description: 
 * Param: 
 */
//////////////////////////////////////////////////////////
void CollectTemperatureInformation(void)
{
  sensors.begin();
  Serial.print("deviceCount: ");
  Serial.println(sensors.getDeviceCount());
  //uint8_t p_device = sensors.getDeviceCount();
  if(sensors.getDeviceCount() < numberOfDevices){
    // if we lost a temperature sensor, then turn on the ORANGE LED for 500ms to indicate an issue,
          // then pass on the led cycle to the next state 
          float temperature_ctr = millis();
          while(millis() < temperature_ctr + (float)LED_KEEP_ON_TIME) // Keep LED on for 500ms
          {
            
            SetLEDColour(ORANGE);
          }
          Serial.println("WARNING: Lost connection to one or more temperature sensors. Double check connections.");
  }

  // loop through all of the known number of temperature sensors. See setup(). 
  for (uint8_t i = 0; i < numberOfDevices; i++) 
    {
      // Search the wire for address
      // at each index in our loop, check the address of the given sensor.
      // the variable tempDeviceAddress is used here to store this information.
      if (sensors.getAddress(tempDeviceAddress, i))
      {
        // if we find a match with a probe, then request the temperature, and store it.
        // using the memcmp function was the best method to compare the serial number array of a known
        // address of a temp sensor, and the one we are currently trying to compare it to.
        if (memcmp(temperatureProbe0_LONG, tempDeviceAddress, 8) == 0) 
        {
          sensors.requestTemperatures();
          presentTemperature0f = sensors.getTempC(tempDeviceAddress); // request temperature in celsius 
          if(debug)
          {
            Serial.print("Device ");
            Serial.print(i);
            Serial.print(" - Temp C: ");
            Serial.println(presentTemperature0f);
          }
        }
        else if (memcmp(temperatureProbe1_LONG, tempDeviceAddress, 8) == 0)
        {
          sensors.requestTemperatures();
          presentTemperature1f = sensors.getTempC(tempDeviceAddress);
          if(debug)
          {
            Serial.print("Device ");
            Serial.print(i);
            Serial.print(" - Temp C: ");
            Serial.println(presentTemperature1f);
          }
        }
        else if (memcmp(temperatureProbe2_SHORT, tempDeviceAddress, 8) == 0)
        {
          sensors.requestTemperatures();
          presentTemperature2f = sensors.getTempC(tempDeviceAddress);
          if(debug)
          {
            Serial.print("Device ");
            Serial.print(i);
            Serial.print(" - Temp C: ");
            Serial.println(presentTemperature2f);
          }
        }
        else if (memcmp(temperatureProbe3_SHORT, tempDeviceAddress, 8) == 0)
        {
          sensors.requestTemperatures();
          presentTemperature3f = sensors.getTempC(tempDeviceAddress);
          if(debug)
          {
            Serial.print("Device ");
            Serial.print(i);
            Serial.print(" - Temp C: ");
            Serial.println(presentTemperature3f);
          }
        }
        else
        {
          
        }
      }
      
      
    }
}

//////////////////////////////////////////////////////////
/*
 * SetLEDState(int colour)
 * Description: Sets LED
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
  case ORANGE:
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
 * Description: receiveEvent is a unique i2C function, that is called when the peripheral
 * receives data from the host. In our particular application, we are supposed
 * to read a byte as an integer, and use this integer to determine what type of 
 * measurement the host desires when we receive a requestEvent. For more on this, see requestEvent(). 
 * Param: int howMany
 */
//////////////////////////////////////////////////////////

void receiveEvent(int howMany)
{
  while (1 < Wire.available())
  {                       // loop through all but the last
    
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);      // print the character
    if(c == "RESET")
    lastRequestedEvent = -1;
    
  }

  
  lastRequestedEvent = Wire.read(); // receive byte as an integer
  timeSinceReceived = millis();
  
//  // if a message is detected, then turn on the TEAL LED for 500ms to indicate an issue,
//  // then pass on the led cycle to the next state 
//  float temperature_ctr = millis();
//  while(millis() < temperature_ctr + (float)LED_KEEP_ON_TIME) // Keep LED on for 500ms
//  {
//    SetLEDColour(WHITE);
//  }

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

//////////////////////////////////////////////////////////
/*
 * requestEvent()
 * Description: 
 * Param: 
 */
//////////////////////////////////////////////////////////

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  // Since all of the data we are reading is in floats, we have to send
  // the values in a total of four bytes, one at a time. On the host end,
  // it will have to reconstruct the float from the bytes sent.
  timeSinceRequested = millis();
  byte *data;
  switch (lastRequestedEvent)
  {
  case CURRENT_0:
    data = (byte *)&presentCurrent0; // casting as a byte pointer to address of variable
    break;
  case CURRENT_1:
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

  // because a float is four bytes, we have to send each byte one at a time
  Wire.write(data[0]);
  Wire.write(data[1]);
  Wire.write(data[2]);
  Wire.write(data[3]);
}

void setup()
{
      // Set up Serial Communication.
    // Putting the Serial monitor here should help the MCU resetting when the com port is accessed. 
    Serial.begin(9600); // start serial for output
    BootScreen();
    
  // Set up Dallas Temperature Sensors
  Serial.println("Starting DallasTemperature Library....");
  sensors.begin();                            // start up the dallas temperature library
  Serial.println("Finding DallasTemperature devices....");
  numberOfDevices = sensors.getDeviceCount(); // number of temperatures
  DisplayDallasTemperatureSerialNumbers();
  Serial.println("Done.");

  WaitFor(150);
  // Set up i2C Communication
  Serial.print("Joining i2C Address:");
  Serial.print(I2C_ADDRESS);
  Serial.print(".....");
  Wire.begin(I2C_ADDRESS);      // join i2c bus with address #8
  Serial.println("Done.");
    WaitFor(150);

  Serial.print("Setting i2C ReceiveEvent....");
  Wire.onReceive(receiveEvent); // register a receive on event
  Serial.println("Done.");
    WaitFor(150);

  Serial.print("Setting i2C RequestEvent....");
  Wire.onRequest(requestEvent); // register a request on event
  Serial.println("Done.");



  

// Set the LEDs to output mode
  Serial.print("Setting LED Outputs...");
  pinMode(LED_PIN_RED, OUTPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);
  pinMode(LED_PIN_BLUE, OUTPUT);
    Serial.println("Done.");
  SetLEDRainbow(LED_RAINBOW_DURATION); // play this LED sequence for LED_RAINBOW_DURATION seconds, then jump in main loop
    Serial.println("Entering loop...");
}

void loop()
{
  static uint32_t millis_ctr = 0; // To not use delay(), we use a timer counter based on the clock. 
  // Serial.println("Requesting Temps");

  if (millis() > millis_ctr + 1000)
  {
    presentCurrent0 = ADC2Current(analogRead(CURRENT0_PIN)); // read analog value from adc, and pass to ADC2Current to convert into a real current
    presentVoltage0 = ADC2Voltage(analogRead(VOLTAGE0_PIN)); // read analog value from adc, and pass to ADC2Voltage to convert into a real voltage
    presentCurrent1 = ADC2Current(analogRead(CURRENT1_PIN)); // read analog value from adc, and pass to ADC2Current to convert into a real current
    presentVoltage1 = ADC2Voltage(analogRead(VOLTAGE1_PIN)); // read analog value from adc, and pass to ADC2Voltage to convert into a real voltage
    CollectTemperatureInformation(); // call the temperature collecting function

    if(12 > presentVoltage0 || presentVoltage0 > 16.8) // if battery voltage is greater than zero but less than 16.8
    {
      // Battery should not be less than 12V or greater than 16.8V
      millis_ctr = millis();
      
          Serial.println("Battery 1 Error: under or overvoltage");
      while(millis() < millis_ctr + (float)LED_KEEP_ON_TIME) // Keep LED on for 500ms
      {
        SetLEDColour(BLUE);
      }
    }
    if(12 > presentVoltage1 || presentVoltage1 > 16.8) // if battery voltage is greater than zero but less than 16.8
    {
      // Battery should not be less than 12V or greater than 16.8V
      millis_ctr = millis();
      
          Serial.println("Battery 2 Error: under or overvoltage");
      while(millis() < millis_ctr + (float)LED_KEEP_ON_TIME) // Keep LED on for 500ms
      {
        
        SetLEDColour(GREEN);
      }
    }
    if(millis() > timeSinceReceived + 2500)
    {
        // time has passed 2.5 seconds since the last received value
        // this should have changed by now 
        SetLEDColour(RED); // Set the led red to indicate haven't received a command
        Serial.println("Communication to Host: BAD");
    }
    else // this should be the last case if no error was reported
    {
      
        Serial.println("Communication to Host: OK");
      SetLEDColour(WHITE);
    }
    // delay(5000);

    millis_ctr = millis();
  }
}


void SetLEDRainbow(float duration)
{
  float millis_ctr = millis(); // counter value from clock
  int i = 0; // variable used for incrementing LED state
    while (millis() < millis_ctr + duration) // Keep LED on for 500ms
    {
        
        switch (i)
        {
        case 0:
            digitalWrite(LED_PIN_RED, HIGH);  // turn the LED on (HIGH is the voltage level)
            digitalWrite(LED_PIN_GREEN, LOW); // turn the LED on (HIGH is the voltage level)
            digitalWrite(LED_PIN_BLUE, LOW);  // turn the LED on (HIGH is the voltage level)
            break;
        case 1:
            digitalWrite(LED_PIN_RED, LOW);    // turn the LED on (HIGH is the voltage level)
            digitalWrite(LED_PIN_GREEN, HIGH); // turn the LED on (HIGH is the voltage level)
            digitalWrite(LED_PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
            break;
        case 2:
            digitalWrite(LED_PIN_RED, LOW);   // turn the LED on (HIGH is the voltage level)
            digitalWrite(LED_PIN_GREEN, LOW); // turn the LED on (HIGH is the voltage level)
            digitalWrite(LED_PIN_BLUE, HIGH); // turn the LED on (HIGH is the voltage level)
            break;
        case 3:
            digitalWrite(LED_PIN_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
            digitalWrite(LED_PIN_GREEN, HIGH); // turn the LED on (HIGH is the voltage level)
            digitalWrite(LED_PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
            break;
        case 4:
            digitalWrite(LED_PIN_RED, HIGH);  // turn the LED on (HIGH is the voltage level)
            digitalWrite(LED_PIN_GREEN, LOW); // turn the LED on (HIGH is the voltage level)
            digitalWrite(LED_PIN_BLUE, HIGH); // turn the LED on (HIGH is the voltage level)
            break;
        case 5:
            digitalWrite(LED_PIN_RED, LOW);    // turn the LED on (HIGH is the voltage level)
            digitalWrite(LED_PIN_GREEN, HIGH); // turn the LED on (HIGH is the voltage level)
            digitalWrite(LED_PIN_BLUE, HIGH);  // turn the LED on (HIGH is the voltage level)
            break;
        }
        i++;
        if (i > 5) // if we hit the end of our cases then reset to red 
            i = 0;
        
        float millis_ctr2 = millis();
        while(millis() < millis_ctr2 + 200)
        {
            // wait for 200 ms and do nothing, getting around delay()
        }
}
}

void DisplayDallasTemperatureSerialNumbers(void)
{
  for(uint8_t i=0; i < numberOfDevices; i++) {
      // Search the wire for address
      if(sensors.getAddress(tempDeviceAddress, i))
      {
        for (uint8_t i = 0; i < 8; i++) {
            if (tempDeviceAddress[i] < 16) Serial.print("0");
              Serial.print(tempDeviceAddress[i], HEX);
        }
            Serial.println(" ");

      }
    }
}

void BootScreen(void)
{
  Serial.println("//////////////////////////////////////////////////////////");
    Serial.println("SDSU Senior Design Team 9 - Robot Power System");
    Serial.println("Boot");
      Serial.println("//////////////////////////////////////////////////////////");
}

void WaitFor(float milliseconds)
{
    float waitCounter = millis();
        while(millis() < waitCounter + milliseconds)
        {
            // wait for 200 ms and do nothing, getting around delay()
        }
}
