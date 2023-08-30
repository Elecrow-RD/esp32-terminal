#define A_PIN 19
#define D_PIN 11
#define E_PIN 40
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(D_PIN, OUTPUT);
  pinMode(E_PIN, OUTPUT);
  digitalWrite(D_PIN, LOW);
  digitalWrite(E_PIN, LOW);
  delay(5000);
}
void loop() {
  // put your main code here, to run repeatedly:

  //int sensorValue = analogRead(A_PIN);
  //Serial.print("Port A value： ");
  //Serial.println(sensorValue);
 
  digitalWrite(D_PIN, HIGH);//pull high
  delay(2000);
  digitalWrite(D_PIN, LOW);
  delay(5000);
  digitalWrite(E_PIN, HIGH);//pull high
  delay(2000);
  digitalWrite(E_PIN, LOW);
  delay(5000);
 
}
