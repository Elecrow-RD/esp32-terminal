#define BUZZER_PIN 45

#define G4 392
#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define E5 659
#define F5 698
#define G5 784
int Tone[] = { B4, B4, B4, B4, B4, B4,
               B4, D5, G4, A4, B4,
               C5, C5, C5, C5, C5, B4, B4, B4,
               D5, D5, C5, A4, G4, G5
             };
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
    //Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  ledcSetup(4, 5000, 8);
  ledcAttachPin(BUZZER_PIN, 4); 
}

void loop() {
  ledcWriteTone(4, Tone[6]);//turn on the buzzer
  delay(1000);
  ledcWrite(4, 0);//turn off the buzzer
  delay(1000);
}
