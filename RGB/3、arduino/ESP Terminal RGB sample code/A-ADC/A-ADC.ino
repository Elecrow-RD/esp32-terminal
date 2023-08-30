lcd.fillScreen(TFT_BLACK);
int sensorValue = analogRead(19);
Serial.print("ADC_value： ");
Serial.println(sensorValue);
lcd.setCursor(50, 200);
lcd.printf("                  ");
lcd.setCursor(50, 200);
lcd.printf("ADC_value:%d", sensorValue);
delay(150);
CloseData = Serial.read();
if (CloseData == 'I')  //Exit
{
Close_Flag = 0;
lcd.fillScreen(TFT_BLACK);
Serial.println("close A port");
break;
}
