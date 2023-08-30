#include <U8g2lib.h>
#include <Wire.h>
#define I2C_SDA 2
#define I2C_SCL 1
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /*clock=*/I2C_SCL, /*data=*/I2C_SDA, /*reset=*/U8X8_PIN_NONE);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //OLED
  u8g2.begin();
  u8g2.enableUTF8Print();        // enable UTF8 support for the Arduino print() function
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setFontDirection(0);
  for (int i = 128; i > -78; i -= 20)
  {
    u8g2.firstPage();
    do {
      u8g2.drawStr(i, 25, "ELECROW");
      delay(2);
    } while ( u8g2.nextPage() );
  } 
}

void loop() {

}
