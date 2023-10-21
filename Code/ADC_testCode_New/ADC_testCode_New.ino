
float val;
void setup() {
  Serial.begin(9600);// put your setup code here, to run once:

}

void loop() {
val = analogRead(A2);
  // put your main code here, to run repeatedly:
Serial.println(val,4);
//delay(100);
}
