

#include <Wire.h>

void setup() {
  Wire.begin(8);        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}
byte c[4];
void loop() {
  Wire.requestFrom(8, 6);    // request 6 bytes from peripheral device #8
  int i = 0;
  while (Wire.available()) { // peripheral may send less than requested
    c[i] = Wire.read(); // receive a byte as character
    i++;
    Serial.println(i);         // print the character
  }

  delay(500);

  union Temp {byte ken_b[4]; float ken_f;}Temp_union;
  Temp_union.ken_b[0] = c[0];
  Temp_union.ken_b[1] = c[1];
  Temp_union.ken_b[2] = c[2];
  Temp_union.ken_b[3] = c[3];
  float Temp = Temp_union.ken_f;
  Serial.print("Temperature C:");
  Serial.println(Temp);
}