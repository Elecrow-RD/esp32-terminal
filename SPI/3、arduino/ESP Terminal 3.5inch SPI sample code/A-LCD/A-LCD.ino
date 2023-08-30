#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <Wire.h>

#define LCD_MOSI 13
#define LCD_MISO 14 // 12
#define LCD_SCK 12
#define LCD_CS 3
#define LCD_RST -1 // 26
#define LCD_DC 42  // 33
#define LCD_BL 46
class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9488 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

  public:
    LGFX(void)
    {
      {
        auto cfg = _bus_instance.config();
        cfg.spi_host = SPI3_HOST;
        cfg.spi_mode = 0;
        cfg.freq_write = 60000000;
        cfg.freq_read = 16000000;
        cfg.spi_3wire = true;
        cfg.use_lock = true;
        cfg.dma_channel = SPI_DMA_CH_AUTO;
        cfg.pin_sclk = LCD_SCK;
        cfg.pin_mosi = LCD_MOSI;
        cfg.pin_miso = LCD_MISO;
        cfg.pin_dc = LCD_DC;
        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);
      }

      {
        auto cfg = _panel_instance.config();

        cfg.pin_cs = LCD_CS;
        cfg.pin_rst = LCD_RST;
        cfg.pin_busy = -1;
        cfg.memory_width = 320;
        cfg.memory_height = 480;
        cfg.panel_width = 320;
        cfg.panel_height = 480;
        cfg.offset_x = 0;
        cfg.offset_y = 0;
        cfg.offset_rotation = 0;
        cfg.dummy_read_pixel = 8;
        cfg.dummy_read_bits = 1;
        cfg.readable = true;
        cfg.invert = false;
        cfg.rgb_order = false;
        cfg.dlen_16bit = false;
        cfg.bus_shared = true;
        _panel_instance.config(cfg);
      }
      setPanel(&_panel_instance);
    }
};


LGFX lcd;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  //LCD initialization
  lcd.init();
  lcd.fillScreen(TFT_WHITE);
  lcd.setTextFont(4);
  pinMode(LCD_BL, OUTPUT);
  lcd.fillScreen(TFT_BLACK);
  digitalWrite(LCD_BL, HIGH);
  delay(500);
  lcd.setRotation(3); /* to rotate */
  lcd.fillScreen(TFT_RED);
  Serial.println( "111111111" );
  delay(500);
  lcd.fillScreen(TFT_GREEN);
  Serial.println( "222222222" );
  delay(500);
  lcd.fillScreen(TFT_BLUE);
  Serial.println( "33333333" );
  delay(500);
  lcd.fillScreen(TFT_BLACK);
  delay(500);
  lcd.setCursor(50, 100);
  lcd.printf("Setup done");

}

void loop() {
  // put your main code here, to run repeatedly:

}
