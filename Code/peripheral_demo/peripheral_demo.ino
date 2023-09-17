
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define I2C_ADDRESS       0x1C      // From sheet

uint8_t requested_data;

// i2C Request codes. Each of these values are messages to the device to request certain information.
// These codes must be sent first along Wire.write() before initiating a Wire.requestFrom() function call. 
#define CURRENT           0
#define VOLTAGE           1
#define TEMPERATURE_0       2         
#define TEMPERATURE_1       3         
#define TEMPERATURE_2       4         


#define CURRENT_PIN       A0
#define VOLTAGE_PIN       A1


#define VOLTAGE_SENSORS   16
#define TEMP_SENSORS      16
#define CURRENT_SENSORS   16

#define VOLTAGE_BASE_VADDRESS   0x10
#define TEMP_BASE_VADDRESS      0x30
#define CURRENT_BASE_VADDRESS   0x50

float voltages[VOLTAGE_SENSORS];
float temperatures[TEMP_SENSORS];
float currents[CURRENT_SENSORS];

#define ONE_WIRE_BUS 5 // We are using PD5 for one wire
int numberOfDevices; // Number of temperature devices found
OneWire oneWire(ONE_WIRE_BUS);//temp
DallasTemperature sensors(&oneWire);//temp
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address


// DALLAS TEMPERATURE PROBE SERIAL NUMBERS
const uint8_t testAddr1[8] = {0x28, 0x75, 0x56, 0x81, 0xE3, 0xD5, 0x3C, 0xAB};
const uint8_t testAddr2[8] = {0x28, 0x0D, 0x7E, 0x81, 0xE3, 0x69, 0x3C, 0x1B};
const uint8_t testAddr3[8] = {0x28, 0x1F, 0x53, 0x81, 0xE3, 0xBC, 0x3C, 0x9D};

const float LOGIC_VOLTAGE = 5.08;
const float VOLTAGE_DIVIDER_R2 = 510000;
const float VOLTAGE_DIVIDER_R1 = 12000000;
const float SHUNT_RESISTOR = 0.01028;
const int LOAD_RESISTOR = 5000;

float presentCurrent = 0.0;
float presentVoltage = 0.0;
float presentTemperature0 = 0.0;
float presentTemperature1 = 0.0;
float presentTemperature2 = 0.0;

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
  value = ((analogValue+1) / 1024 * LOGIC_VOLTAGE); // first convert the analog value based on the logic level voltage
  // then use the equation from the INA139 datasheet , where iS = (Vout * 1k) / (rs * rl)
  value = (value *(1000)/((LOAD_RESISTOR) *(SHUNT_RESISTOR))); 
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

void requestTemperature(int temperatureLocation)
{
 Serial.println("Temperature requested.");
  switch(temperatureLocation)
  {
    case TEMPERATURE_0: 
      for(uint8_t i=0; i < numberOfDevices; i++) {
        // Search the wire for address
        
        if(sensors.getAddress(tempDeviceAddress, i)) {        
          if (memcmp(testAddr1, tempDeviceAddress, 8) == 0)
          {
            // device 0
            presentTemperature0 = sensors.getTempC(tempDeviceAddress);
          }
        }
      }
    break; 
  
    case TEMPERATURE_1: 
      for(uint8_t i=0; i < numberOfDevices; i++) {
        // Search the wire for address
        
        if(sensors.getAddress(tempDeviceAddress, i)) {        
          if (memcmp(testAddr2, tempDeviceAddress, 8) == 0)
          {
            // device 1
            presentTemperature1 = sensors.getTempC(tempDeviceAddress);
          }
        }
      }
    break; 

    case TEMPERATURE_2: 
      for(uint8_t i=0; i < numberOfDevices; i++) {
        // Search the wire for address
        
        if(sensors.getAddress(tempDeviceAddress, i)) {        
          if (memcmp(testAddr3, tempDeviceAddress, 8) == 0)
          {
            // device 2
            presentTemperature2 = sensors.getTempC(tempDeviceAddress);
          }
        }
      }
    break; 
  }
  
}

void setup()
{
  Serial.begin(115200);
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  // Initialize values to NaN
  for(uint8_t n = 0; n < VOLTAGE_SENSORS; n++){
    voltages[n] = (float)n;
  }
  for(uint8_t n = 0; n < TEMP_SENSORS; n++){
    temperatures[n] = NAN;
  }
  for(uint8_t n = 0; n < CURRENT_SENSORS; n++){
    currents[n] = NAN;
  }

  // Do this last
  Wire.begin(I2C_ADDRESS);                
  Wire.onReceive(receiveEvent);       // Get which datapoint we want to send back
  Wire.onRequest(requestEvent);       // Send back the datapoint selected earlier
  Serial.print("setup");
}

void loop()
{
  static uint32_t millis_ctr = 0;

  if(millis() > millis_ctr + 5000){
    for(uint8_t i=0; i < numberOfDevices; i++) {
      // Search the wire for address
      
      if(sensors.getAddress(tempDeviceAddress, i)) {
        float tempC = sensors.getTempC(tempDeviceAddress);
        
        if (memcmp(testAddr1, tempDeviceAddress, 8) == 0)
        {
          // device 0
                  temperatures[0] = tempC;
            Serial.print("Device 0 - Temp C: ");
            Serial.println(temperatures[0]);
        }
        else if (memcmp(testAddr2, tempDeviceAddress, 8) == 0)
        {
          // device 1
          temperatures[1] = tempC;
            Serial.print("Device 1 - Temp C: ");
            Serial.println(temperatures[1]);
        }    
        else if (memcmp(testAddr3, tempDeviceAddress, 8) == 0)
        {
          // device 2
          temperatures[4] = tempC;
            Serial.print("Device 3 - Temp C: ");
            Serial.println(temperatures[4]);
        }
        else {
          Serial.println("No mattches :(");
        }
      } 	
    }
    //delay(5000);

    sensors.requestTemperatures();
    Serial.println("Requesting Temps");
    millis_ctr = millis();
  }
}

// See what data needs to be sent here
void receiveEvent(){
  requested_data = Wire.read();
}

// Send data from here
void requestEvent()
{
  if(requested_data > (CURRENT_BASE_VADDRESS - 1)){                 // We want current
    Wire.write(((uint8_t *)(currents + (requested_data - CURRENT_BASE_VADDRESS))), sizeof(float));
  } else if (requested_data > (TEMP_BASE_VADDRESS - 1)){            // We want temperature
    Wire.write(((uint8_t *)(temperatures + (requested_data - TEMP_BASE_VADDRESS))), sizeof(float));
  } else {                                                          // We want voltage
    Wire.write(((uint8_t *)(voltages + (requested_data - VOLTAGE_BASE_VADDRESS))), sizeof(float));
  }
}
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    //if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}
