/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
  //5.08 = Logical Level
 */

// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
// give it a name:
int led = 13;
int analogPin = A1; // potentiometer wiper (middle terminal) connected to analog pin 3
                    // outside leads to ground and +5V
float val = 0;  // variable to store the value read
float volt = 0;
float current = 0;
float shunt = 0.01028;
float rl = 10000;
float logicvolt = 5.08;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  Serial.begin(9600);           //  setup serial
}

// the loop routine runs over and over again forever:
void loop() {
  val = analogRead(analogPin);  // read the input pin
  Serial.print("ADC Output: ");          // debug value
  Serial.println(val,5);          // debug value
  
  volt = ((val+1) / 1024 * logicvolt);
  Serial.print("Voltage: ");          // debug value
  Serial.println(volt,5);          // debug value
  
  current = (volt*(1000)/(rl)/(0.01028));   //Vout = (iS)(rs)(rl)/(1k)
  Serial.print("Current: ");          // debug value
  Serial.println(current,5);          // debug value  
  
  Serial.print("R Shunt: ");          // debug value
  Serial.println(shunt,5);          // debug value
  
  Serial.println();          // space
  
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(500);               // wait for a second

}
