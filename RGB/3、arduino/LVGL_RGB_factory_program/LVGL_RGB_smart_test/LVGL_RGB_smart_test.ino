#include<Arduino.h>
#include <lvgl.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "FT6236.h"
#include <Ticker.h>          //Call the ticker. H Library
Ticker ticker1;
Ticker ticker2;

#include <Wire.h>
#include <Digital_Light_TSL2561.h>

#include "DHT.h"

#define DHTPIN 40     // what pin we're connected to

// Uncomment whatever type you're using!
// #define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
/*
   If you want to use the LVGL examples,
  make sure to install the lv_examples Arduino library
  and uncomment the following line.
*/
#include <examples/lv_examples.h>
#include <demos/lv_demos.h>
const int i2c_touch_addr = TOUCH_I2C_ADD;


#include <ui.h>
extern lv_obj_t * ui_Screen1;
static lv_obj_t * ui_Label;//
static lv_obj_t * ui_Tem;//
extern lv_obj_t * ui_Light;
extern lv_obj_t * ui_AN;
extern lv_obj_t * ui_AX;
extern lv_obj_t * ui_DN;
extern lv_obj_t * ui_DX;
extern lv_obj_t * ui_IN;
extern lv_obj_t * ui_IX;
extern lv_obj_t * ui_TextArea7;
extern lv_obj_t * ui_TextArea8;
extern lv_obj_t * ui_TextArea9;

#define LCD_BL 46

#define SDA_FT6236 38
#define SCL_FT6236 39

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
class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9488 _panel_instance;
    lgfx::Bus_Parallel16 _bus_instance;
  public:
    LGFX(void)
    {
      {
        auto cfg = _bus_instance.config();
        cfg.port = 0;
        cfg.freq_write = 80000000;
        cfg.pin_wr = 18;
        cfg.pin_rd = 48;
        cfg.pin_rs = 45;

        cfg.pin_d0 = 47;
        cfg.pin_d1 = 21;
        cfg.pin_d2 = 14;
        cfg.pin_d3 = 13;
        cfg.pin_d4 = 12;
        cfg.pin_d5 = 11;
        cfg.pin_d6 = 10;
        cfg.pin_d7 = 9;
        cfg.pin_d8 = 3;
        cfg.pin_d9 = 8;
        cfg.pin_d10 = 16;
        cfg.pin_d11 = 15;
        cfg.pin_d12 = 7;
        cfg.pin_d13 = 6;
        cfg.pin_d14 = 5;
        cfg.pin_d15 = 4;
        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);
      }

      {
        auto cfg = _panel_instance.config();

        cfg.pin_cs = -1;
        cfg.pin_rst = -1;
        cfg.pin_busy = -1;
        cfg.memory_width = 320;
        cfg.memory_height = 480;
        cfg.panel_width = 320;
        cfg.panel_height = 480;
        cfg.offset_x = 0;
        cfg.offset_y = 0;
        cfg.offset_rotation = 2;
        cfg.dummy_read_pixel = 8;
        cfg.dummy_read_bits = 1;
        cfg.readable = true;
        cfg.invert = false;
        cfg.rgb_order = false;
        cfg.dlen_16bit = true;
        cfg.bus_shared = true;
        _panel_instance.config(cfg);
      }
      setPanel(&_panel_instance);
    }
};

LGFX tft;
/*Change to your screen resolution*/
static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 8 ];


/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
  uint32_t w = ( area->x2 - area->x1 + 1 );
  uint32_t h = ( area->y2 - area->y1 + 1 );

  tft.startWrite();
  tft.setAddrWindow( area->x1, area->y1, w, h );
  tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  tft.endWrite();

  lv_disp_flush_ready( disp );
}

void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
  int pos[2] = {0, 0};

  ft6236_pos(pos);
  if (pos[0] > 0 && pos[1] > 0)
  {
    data->state = LV_INDEV_STATE_PR;
    //    data->point.x = tft.width()-pos[1];
    //    data->point.y = pos[0];
    data->point.x = tft.width() - pos[1];
    data->point.y = pos[0];
//    Serial.printf("x-%d,y-%d\n", data->point.x, data->point.y);
  }
  else {
    data->state = LV_INDEV_STATE_REL;
  }
}


void touch_init()
{
  // I2C init
  Wire.begin(SDA_FT6236, SCL_FT6236);
  byte error, address;

  Wire.beginTransmission(i2c_touch_addr);
  error = Wire.endTransmission();

  if (error == 0)
  {
    Serial.print("I2C device found at address 0x");
    Serial.print(i2c_touch_addr, HEX);
    Serial.println("  !");
  }
  else if (error == 4)
  {
    Serial.print("Unknown error at address 0x");
    Serial.println(i2c_touch_addr, HEX);
  }
}


const float AirValue = 3600;                       //初始化最大干燥 （传感器在空中的情况。这个需要根据你自己传感器情况初始化）
const float WaterValue = 1900;                     //初始化最大湿度

int sensorValue = 0;
float t = 0;
int iic_value=0;
void callback1()  //Callback function
{
  String str1 = "@#";
  String str2 = "#";
  String str3 = "#@";
  Serial.print(str1 + (int)sensorValue + str2 + (int)t + str2 + (int)iic_value + str3);
  Serial.println("");
}


char CloseData = 0;
static int NUM = 0;
char buf_lora[128] = {};
int bufindex = 0;
char *info[128] = {};
int Bengin_flag = 0;

void setup()
{
  Serial.begin( 9600 ); /* prepare for possible serial debug */
  //Buzzer
  pinMode(20, OUTPUT);
  ledcSetup(4, 5000, 8);
  ledcAttachPin(20, 4);

  //DHT
  dht.begin();

  //LIGHT
  Wire.setPins(38, 39);
  Wire.begin();
  TSL2561.init();


  tft.begin();          /* TFT init */
  tft.setRotation( 1 ); /* Landscape orientation, flipped */
  tft.fillScreen(TFT_BLACK);
  delay(500);
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);

  touch_init();

  lv_init();
  lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 8 );

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init( &disp_drv );
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register( &disp_drv );

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init( &indev_drv );
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register( &indev_drv );


  ui_init();
  label_xy();
  lv_timer_handler();
  Serial.println( "Setup done" );
  //  ticker1.attach_ms(100, callback1);//每50ms调用callback1
  //  ticker2.attach_ms(500, callback2);//每50ms调用callback2
  Serial.println("AT+CTXADDRSET=12");
  delay(500);
  Serial.println("AT+CADDRSET=13");
  delay(500);
  Serial.println("AT+CTXADDRSET=12");
  delay(500);
  Serial.println("AT+CADDRSET=13");
  delay(500);
  Serial.println("AT+CTXADDRSET=12");
  delay(500);
  Serial.println("AT+CADDRSET=13");
  delay(500);
  Serial.println("AT+CTX=868100000,5,0,1,21,1");
  delay(500);
  lv_scr_load_anim(ui_Screen1, LV_SCR_LOAD_ANIM_NONE, 0, 200, false);
  lv_timer_handler();
}

void loop()
{
  //***************************************************湿度********************
  sensorValue = 100 - (((analogRead(19)) - WaterValue) / (AirValue - WaterValue)) * 100;
  if (sensorValue < 3)sensorValue = 0;
  const char * AN = lv_textarea_get_text(ui_AN);
  const char * AX = lv_textarea_get_text(ui_AX);
  int an = atoi(AN);
  int ax = atoi(AX);
  if (sensorValue > ax )
  {
    ledcWriteTone(4, Tone[6]);//打开蜂鸣器
    lv_label_set_text_fmt(ui_Label, "%d", sensorValue);
  }
  else if (sensorValue < an)
  {
    ledcWriteTone(4, Tone[6]);//打开蜂鸣器
    lv_label_set_text_fmt(ui_Label, "%d", sensorValue);
  }
  else if ((an == 0 && ax == 0) || (an > ax))
  {
    ledcWrite(4, 0);//关闭蜂鸣器
    lv_label_set_text_fmt(ui_Label, "%d", sensorValue);
  }
  else {
    lv_label_set_text_fmt(ui_Label, "%d", sensorValue);
  }


  //***************************************************温度********************
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    //    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  const char * DN = lv_textarea_get_text(ui_DN);
  const char * DX = lv_textarea_get_text(ui_DX);
  int dn = atoi(DN);
  int dx = atoi(DX);
  if ((int)t > dx )
  {
    ledcWriteTone(4, Tone[6]);//打开蜂鸣器
    lv_label_set_text_fmt(ui_Tem, "%d", (int)t);
  }
  else if ((int)t < dn)
  {
    ledcWriteTone(4, Tone[6]);//打开蜂鸣器
    lv_label_set_text_fmt(ui_Tem, "%d", (int)t);
  }
  else if ((dn == 0 && dx == 0) || (dn > dx))
  {

    lv_label_set_text_fmt(ui_Tem, "%d", (int)t);
  }
  else
  {    

    lv_label_set_text_fmt(ui_Tem, "%d", (int)t);
  }



  //***************************************************光照强度********************
  //  Serial.print("The Light value is: ");
  //  Serial.println(TSL2561.readVisibleLux());
  const char * IN = lv_textarea_get_text(ui_IN);
  const char * IX = lv_textarea_get_text(ui_IX);
  int in = atoi(IN);
  int ix = atoi(IX);
  if ((int)TSL2561.readVisibleLux() > ix )
  {
    ledcWriteTone(4, Tone[6]);//打开蜂鸣器
    lv_label_set_text_fmt(ui_Light, "%d", (int)TSL2561.readVisibleLux());
  }
  else if ((int)TSL2561.readVisibleLux() < in)
  {
    ledcWriteTone(4, Tone[6]);//打开蜂鸣器
    lv_label_set_text_fmt(ui_Light, "%d", (int)TSL2561.readVisibleLux());
  }
  else if ((in == 0 && ix == 0) || (in > ix))
  {
    lv_label_set_text_fmt(ui_Light, "%d", (int)TSL2561.readVisibleLux());
  }
  else {
    iic_value=(int)TSL2561.readVisibleLux();
    lv_label_set_text_fmt(ui_Light, "%d", (int)TSL2561.readVisibleLux());
  }
  

  
  //***************************************************LORA********************
  if (Bengin_flag == 0)
  {
    ticker1.attach_ms(2000, callback1);//每50ms调用callback1
    Bengin_flag = 1;
  }


  //***************************************************lvgl刷新********************
  lv_timer_handler(); /* let the GUI do its work */
  ledcWrite(4, 0);//关闭蜂鸣器
  delay(5);
}

//触摸Label控件
void label_xy()
{
  ui_Label = lv_label_create(ui_Screen1);
  lv_obj_enable_style_refresh(true);
  lv_obj_set_width(ui_Label, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Label, LV_SIZE_CONTENT);    /// 1
  lv_obj_set_x(ui_Label, -128);
  lv_obj_set_y(ui_Label, -96);
  lv_obj_set_align(ui_Label, LV_ALIGN_CENTER);
  lv_obj_set_style_text_color(ui_Label, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_Label, &lv_font_montserrat_38, LV_PART_MAIN | LV_STATE_DEFAULT);


  ui_Tem = lv_label_create(ui_Screen1);
  lv_obj_enable_style_refresh(true);
  lv_obj_set_width(ui_Tem, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Tem, LV_SIZE_CONTENT);    /// 1
  lv_obj_set_x(ui_Tem, 125);
  lv_obj_set_y(ui_Tem, -96);
  lv_obj_set_align(ui_Tem, LV_ALIGN_CENTER);
  lv_obj_set_style_text_color(ui_Tem, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Tem, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_Tem, &lv_font_montserrat_38, LV_PART_MAIN | LV_STATE_DEFAULT);


}
static void btn_event_cb(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * btn = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED) {
    static uint8_t cnt = 0;
    cnt++;

    /*Get the first child of the button which is the label and change its text*/
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "Button: %d", cnt);
  }
}

/**
   Create a button with a label and react on click event.
*/
void lv_example_get_started_4(void)
{
  lv_obj_t * btn = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
  lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
  lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

  lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
  lv_label_set_text(label, "Button");                     /*Set the labels text*/
  lv_obj_center(label);
}
