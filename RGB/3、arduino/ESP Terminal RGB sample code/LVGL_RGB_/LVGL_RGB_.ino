#include<Arduino.h>
#include <lvgl.h>
//#include <WiFi.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "FT6236.h"

/*
   If you want to use the LVGL examples,
  make sure to install the lv_examples Arduino library
  and uncomment the following line.
*/
#include <examples/lv_examples.h>
#include <demos/lv_demos.h>
const int i2c_touch_addr = TOUCH_I2C_ADD;

static lv_obj_t * ui_MENU;
static lv_obj_t * ui_TOUCH;
static lv_obj_t * ui_JIAOZHUN;
static lv_obj_t * ui_Label2;
static lv_obj_t * ui_Label;//TOUCH界面label
static lv_obj_t * ui_Label3;//TOUCH界面label3
static lv_obj_t * ui_Labe2;//Menu界面进度条label
#define LCD_BL 46

#define SDA_FT6236 38
#define SCL_FT6236 39
//FT6236 ts = FT6236();

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
static lv_color_t buf[ screenWidth * screenHeight / 5 ];


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
    Serial.printf("x-%d,y-%d\n", data->point.x, data->point.y);
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

void setup()
{

  Serial.begin( 115200 ); /* prepare for possible serial debug */

//  WiFi.mode(WIFI_OFF);
//  if (WiFi.status() != WL_CONNECTED)
//  {
//    Serial.println("WIFI OFF");
//  }


  tft.begin();          /* TFT init */
  tft.setRotation( 1 ); /* Landscape orientation, flipped */
  tft.fillScreen(TFT_BLACK);
  delay(500);
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);


  //  if (!ts.begin(0, SDA_FT6236, SCL_FT6236)) {
  //    Serial.println("Unable to start the capacitive touch Screen.");
  //  }
  touch_init();

  lv_init();
  lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 5 );

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

#if 0
  /* Create simple label */
  lv_example_get_started_4();

#else
  /* Try an example from the lv_examples Arduino library
     make sure to include it as written above.

  */
  // uncomment one of these demos
  lv_demo_widgets();            // OK
  //  label_xy();
#endif
  Serial.println( "Setup done" );
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay( 5 );
}
static void Label_event_cb(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * btn = lv_event_get_target(e);
  if (code == LV_EVENT_ALL) {
    int sensorValue = analogRead(19);
    Serial.print("A口数值： ");
    Serial.println(sensorValue);
    lv_label_set_text_fmt(btn, "A: %d", sensorValue);
  }
}
//触摸Label控件
void label_xy()
{
  ui_Label = lv_label_create(lv_scr_act());
  lv_obj_enable_style_refresh(true);
  lv_obj_set_width(ui_Label, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Label, LV_SIZE_CONTENT);    /// 1
  lv_obj_set_x(ui_Label, -55);
  lv_obj_set_y(ui_Label, -40);
  lv_obj_set_align(ui_Label, LV_ALIGN_CENTER);
  lv_obj_set_style_text_color(ui_Label, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_Label, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(ui_Label, Label_event_cb, LV_EVENT_ALL, NULL);
  //  ui_Label3 = lv_label_create(lv_scr_act());
  //  lv_obj_enable_style_refresh(true);
  //  lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);   /// 1
  //  lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT);    /// 1
  //  lv_obj_set_x(ui_Label3, 85);
  //  lv_obj_set_y(ui_Label3, -40);
  //  lv_obj_set_align(ui_Label3, LV_ALIGN_CENTER);
  //  lv_obj_set_style_text_color(ui_Label3, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
  //  lv_obj_set_style_text_opa(ui_Label3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  //  lv_obj_set_style_text_font(ui_Label3, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

}
