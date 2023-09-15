
const int PIN_RED   = 5;
const int PIN_GREEN = 6;
const int PIN_BLUE  = 9;
int brightness = 0;  // how bright the LED is
int fadeAmount = 5;  // how many points to fade the LED by

void setup() {
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);
}

void loop() {

  //analogWrite(led, brightness);
  analogWrite(PIN_RED,   brightness);
  analogWrite(PIN_GREEN, brightness);
  analogWrite(PIN_BLUE,  brightness);
  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  // wait for 30 milliseconds to see the dimming effect
  delay(300);
  // color code #00C9CC (R = 0,   G = 201, B = 204)
  

  //delay(2000); // keep the color 1 second

  // color code #F7788A (R = 247, G = 120, B = 138)
  // analogWrite(PIN_RED,   255);
  // analogWrite(PIN_GREEN, 0);
  // analogWrite(PIN_BLUE,  0);

  //delay(2000); // keep the color 1 second

  // color code #34A853 (R = 52,  G = 168, B = 83)
  // analogWrite(PIN_RED,   0);
  // analogWrite(PIN_GREEN, 255);
  // analogWrite(PIN_BLUE,  0);

  // delay(2000); // keep the color 1 second
}
