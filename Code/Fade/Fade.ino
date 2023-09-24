/*
  Fade

  This example shows how to fade an LED on pin 9 using the analogWrite()
  function.

  The analogWrite() function uses PWM, so if you want to change the pin you're
  using, be sure to use another PWM capable pin. On most Arduino, the PWM pins
  are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Fade
*/

const int PIN_RED   = 3;
const int PIN_GREEN = 10;
const int PIN_BLUE  = 9;
int i = 0;

int led = 9;           // the PWM pin the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// the setup routine runs once when you press reset:
void setup() {
  // declare pin 9 to be an output:
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // set the brightness of pin 9:
  analogWrite(PIN_BLUE, brightness);
  Serial.println("hi");
  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  // wait for 30 milliseconds to see the dimming effect
  delay(30);

    switch(i)
  {
    case 0:
      digitalWrite(PIN_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, LOW);   // turn the LED on (HIGH is the voltage level)
      //digitalWrite(PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
    break;
    case 1:
    digitalWrite(PIN_RED, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, HIGH);   // turn the LED on (HIGH is the voltage level)
      //digitalWrite(PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
    break;
    case 2:
    digitalWrite(PIN_RED, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, LOW);   // turn the LED on (HIGH is the voltage level)
      //digitalWrite(PIN_BLUE, HIGH);   // turn the LED on (HIGH is the voltage level)
    break;
    case 3:
    digitalWrite(PIN_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, HIGH);   // turn the LED on (HIGH is the voltage level)
      //digitalWrite(PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
    break;
    case 4:
    digitalWrite(PIN_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, LOW);   // turn the LED on (HIGH is the voltage level)
      //digitalWrite(PIN_BLUE, HIGH);   // turn the LED on (HIGH is the voltage level)
    break;
    case 5:
    digitalWrite(PIN_RED, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, HIGH);   // turn the LED on (HIGH is the voltage level)
      //digitalWrite(PIN_BLUE, HIGH);   // turn the LED on (HIGH is the voltage level)
    break;
    case 6:
      digitalWrite(PIN_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, HIGH);   // turn the LED on (HIGH is the voltage level)
      //digitalWrite(PIN_BLUE, HIGH);   // turn the LED on (HIGH is the voltage level)
    break;
    case 7:
    digitalWrite(PIN_RED, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, LOW);   // turn the LED on (HIGH is the voltage level)
      //digitalWrite(PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
    break;
  }
  i++;
  if(i > 5)
    i = 0;
      delay(200);                       // wait for a second

}
