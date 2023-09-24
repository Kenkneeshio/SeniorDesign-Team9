float value0;
float value1;
#define CURRENT_PIN A2
#define VOLTAGE_PIN A1
//////////////////////////////////////////////////////////
// LED PIN ASSIGNMENTS
const int LED_PIN_RED   = 12;
const int LED_PIN_GREEN = 13;
const int LED_PIN_BLUE  = 11;
/////////////////////////////////////
const int RED = 0; // COMMUNICATION
const int GREEN = 1; // ALL GOOD
const int BLUE = 2; // BATTERY 1 BAD
const int PINK = 3; // BATTERY 2 BAD
const int LIGHT_BLUE = 4; // TEMPERATURE FAULT 
const int ORANGE = 5;  //TWO BATTERY ARE NOT BALANCED
const int WHITE = 6;  //CURRENT 1
const int OFF = 7;   //

float timeSinceReceived = 10000;
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
  case PINK:
    digitalWrite(LED_PIN_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_GREEN, HIGH); // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
    break;
  case LIGHT_BLUE:
    digitalWrite(LED_PIN_RED, HIGH);  // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_GREEN, LOW); // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_PIN_BLUE, HIGH); // turn the LED on (HIGH is the voltage level)
    break;
  case ORANGE:
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

float ADC2Voltage(float analogValue)
{
  float value;
  value = analogValue * (5 / 1023.0); // first convert the analog value based on the logic level voltage
  return value;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN_RED, OUTPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);
  pinMode(LED_PIN_BLUE, OUTPUT);
    Serial.begin(9600); // start serial for output
    // SET UP rainbow fun
    while(millis() < millis_ctr + 3000) // Keep LED on for 500ms
      {
          switch(i)
  {
    case 0:
      digitalWrite(PIN_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
    break;
    case 1:
    digitalWrite(PIN_RED, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
    break;
    case 2:
    digitalWrite(PIN_RED, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_BLUE, HIGH);   // turn the LED on (HIGH is the voltage level)
    break;
    case 3:
    digitalWrite(PIN_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
    break;
    case 4:
    digitalWrite(PIN_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_BLUE, HIGH);   // turn the LED on (HIGH is the voltage level)
    break;
    case 5:
    digitalWrite(PIN_RED, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_BLUE, HIGH);   // turn the LED on (HIGH is the voltage level)
    break;
    case 6:
      digitalWrite(PIN_RED, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_BLUE, HIGH);   // turn the LED on (HIGH is the voltage level)
    break;
    case 7:
    digitalWrite(PIN_RED, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_GREEN, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(PIN_BLUE, LOW);   // turn the LED on (HIGH is the voltage level)
    break;
  }
  i++;
  if(i > 5)
    i = 0;
  delay(200);                       // wait for a second
          SetLEDColour(BLUE);
          //Serial.println("Blue LED On");
      }
}

void loop()
{
  static uint32_t millis_ctr = 0;
  // Serial.println("Requesting Temps");

  if (millis() > millis_ctr + 1000)
  {
    value0 = ADC2Voltage(analogRead(CURRENT_PIN)); // read analog value from adc, and pass to ADC2Current to convert into a real current
    value1 = ADC2Voltage(analogRead(VOLTAGE_PIN)); // read analog value from adc, and pass to ADC2Voltage to convert into a real voltage
    Serial.print("value0 ");
    Serial.print(value0);
    Serial.println("");
    Serial.print("value1 ");
    Serial.print(value1);
    Serial.println("");
    if(1 > value0 || value0 > 3.3) // if battery voltage is greater than zero but less than 16.8
    {
      // Battery should not be less than 12V or greater than 16.8V
      millis_ctr = millis();
      while(millis() < millis_ctr + 500) // Keep LED on for 500ms
      {
          SetLEDColour(BLUE);
          //Serial.println("Blue LED On");
      }
    }
    if(1 > value1 || value1 > 3.3) // if battery voltage is greater than zero but less than 16.8
    {
      // Battery should not be less than 12V or greater than 16.8V
      millis_ctr = millis();
      while(millis() < millis_ctr + 500) // Keep LED on for 500ms
      {
          SetLEDColour(PINK);
          //Serial.println("ORANGE LED On");
      }
    }
    if(millis() > timeSinceReceived + 2500)
    {
        // time has passed 2.5 seconds since the last received value
        // this should have changed by now 
        SetLEDColour(RED); // Set the led red to indicate haven't received a command
    }
    else // this should be the last case if no error was reported
    {
      SetLEDColour(GREEN);
      //Serial.println("Green LED On");
    }
    // delay(5000);

    millis_ctr = millis();
  }
}
