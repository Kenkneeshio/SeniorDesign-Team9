// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

int numberOfDevices = 0;
/*
 * The setup function. We only start the sensors here
 */
void setup(void)
{
  // start serial port
  Serial.begin(9600);
  // Start up the library
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("Number of Devices: ");
  Serial.println(numberOfDevices);
}

/*
 * Main function, get and show the temperature
 */
void loop(void)
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
  delay(1000);
}
