
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define I2C_ADDRESS       0x1C      // From sheet

uint8_t requested_data;

#define VOLTAGE_SENSORS   16
#define TEMP_SENSORS      16
#define CURRENT_SENSORS   16

#define VOLTAGE_BASE_VADDRESS   0x10
#define TEMP_BASE_VADDRESS      0x30
#define CURRENT_BASE_VADDRESS   0x50

float voltages[VOLTAGE_SENSORS];
float temperatures[TEMP_SENSORS];
float currents[CURRENT_SENSORS];

#define ONE_WIRE_BUS 4
int numberOfDevices; // Number of temperature devices found
OneWire oneWire(ONE_WIRE_BUS);//temp
DallasTemperature sensors(&oneWire);//temp
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

const uint8_t testAddr1[8] = {0x28, 0xEE, 0x0A, 0x81, 0xE3, 0x37, 0x3C, 0xB2};
const uint8_t testAddr2[8] = {0x28, 0xA9, 0x9A, 0x81, 0xE3, 0x07, 0x3C, 0xD8};
const uint8_t testAddr3[8] = {0x28, 0x1F, 0x53, 0x81, 0xE3, 0xBC, 0x3C, 0x9D};


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