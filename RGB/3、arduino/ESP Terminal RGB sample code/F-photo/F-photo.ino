#define LGFX_USE_V1

#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <LovyanGFX.hpp>
#include <Ticker.h>          //Call the ticker. H Library
Ticker ticker1;
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#include <WiFi.h>
#include <FT6236.h>

#include "BLEDevice.h"              //BLE驱动库
#include "BLEServer.h"              //BLE蓝牙服务器库
#include "BLEUtils.h"               //BLE实用程序库
#include "BLE2902.h"                //特征添加描述符库
#include <BLECharacteristic.h>      //BLE特征函数库
//#include "BluetoothSerial.h"
//BluetoothSerial SerialBT;
BLEAdvertising* pAdvertising = NULL;
BLEServer* pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic* pCharacteristic = NULL;
#define bleServerName "ESP32RGB-BLE"  //BLE服务器的名称
#define SERVICE_UUID "6479571c-2e6d-4b34-abe9-c35116712345"  //服务的UUID
#define CHARACTERISTIC_UUID "826f072d-f87c-4ae6-a416-6ffdcaa02d73"

#endif
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C  u8g2(U8G2_R0, /* clock=*/ 39, /* data=*/ 38, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED

#define I2C_SCL 39
#define I2C_SDA 38

const int i2c_touch_addr = TOUCH_I2C_ADD;
#define get_pos ft6236_pos

#define SPI_MOSI 2 //1
#define SPI_MISO 41
#define SPI_SCK 42
#define SD_CS 1 //2

#define LCD_CS -1
#define LCD_BLK 46


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
    //lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Panel_ILI9488 _panel_instance;
    lgfx::Bus_Parallel16 _bus_instance; // 8位并行总线实例（仅限ESP32）

  public:
    //创建构造函数并在此处进行各种设置。
    //如果更改了类名，构造函数也应指定相同的名称。
    LGFX(void)
    {
      { //设置总线控制。
        auto cfg = _bus_instance.config(); //获取用于总线设置的结构。

        // 16位设置
        cfg.port = 0; // 选择要使用的I2S端口（0或1）（使用ESP32的I2S LCD模式）
        cfg.freq_write = 20000000; // 发送时钟（最大可将20MHz、80MHz舍入为整数）
        cfg.pin_wr = 18; // 连接WR的针脚编号
        cfg.pin_rd = 48; // 连接RD的针脚编号
        cfg.pin_rs = 45; // 连接RS（D/C）的针脚编号

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

        _bus_instance.config(cfg);              // 在总线上反映设置值。
        _panel_instance.setBus(&_bus_instance);
      }

      { // 设置显示面板控制
        auto cfg = _panel_instance.config(); // 获取用于设置显示面板的结构。

        cfg.pin_cs = -1; // CS要拉低
        cfg.pin_rst = -1; // RST与开发板RST关联
        cfg.pin_busy = -1; // 连接BUSY的管脚编号（-1=disable）
        //※以下的设定值在每个面板上都设定了一般的初始值，所以不明的项目请留言尝试。

        cfg.memory_width = 320; // 驱动器IC支持的最大宽度
        cfg.memory_height = 480; // 驱动器IC支持的最大高度
        cfg.panel_width = 320; // 实际可见宽度
        cfg.panel_height = 480; // 实际可见高度
        cfg.offset_x = 0; // 面板X方向偏移量
        cfg.offset_y = 0; // 面板Y方向偏移量
        cfg.offset_rotation = 0; // 旋转方向值的偏移0~7（4~7上下反转）
        cfg.dummy_read_pixel = 8; // 像素读取前虚拟读位数
        cfg.dummy_read_bits = 1; // 读取非像素数据之前的虚拟读位数
        cfg.readable = true; // 可读取数据时设置为真
        cfg.invert = false; // 如果面板的明暗颠倒，则设置为真
        cfg.rgb_order = false; // 如果面板中的红色和蓝色替换为真
        cfg.dlen_16bit = true; // 在以16 bit为单位发送数据长度的面板的情况下设定为真
        cfg.bus_shared = true; // 与SD卡共享总线时设置为真（通过drawJpgFile等进行总线控制）
        _panel_instance.config(cfg);
      }

      setPanel(&_panel_instance); // 设置要使用的面板。
    }
};

void callback1()  //Callback function
{
  //IO40
  digitalWrite(40, HIGH);
  delay(500);
  digitalWrite(40, LOW);
  delay(500);
}
bool connected_state = false;   //创建设备连接标识符

class MyServerCallbacks: public BLEServerCallbacks  //创建连接和断开调用类
{
    void onConnect(BLEServer *pServer)//开始连接函数
    {
      connected_state = true;   //设备正确连接
    }
    void onDisconnect(BLEServer *pServer)//断开连接函数
    {
      connected_state = false;  //设备连接错误
    }

};
LGFX lcd;
int pos[2] = {0, 0};
int err_code = 0;
char LU[10];
char B[10];
int i = 0;
int rect_length = 40;
char CloseData;
int NO_Test_Flag = 0;
int Test_Flag = 0;
int Close_Flag = 0;
int wifi_close_flag = 0;

char UWB;
char *uwb_info[128] = {};
char uwb_buf[128] = {};
int uwb_bufindex = 0;
int uwb_flag = 0;
char buf[128] = {};
int bufindex = 0;
int first_Flag = 0;
char wifi_account[20] = {}, wifi_password[20] = {} , dtm[100] = {};
char *info[128] = {};
int wifi_flag = 0;
const char *ssid = "elecrow888"; //你的网络名称
const char *password = "elecrow2014"; //你的网络密码

int print_img(fs::FS & fs, String filename, int x, int y)
{
  File f = fs.open(filename, "r");
  if (!f)
  {
    Serial.println("Failed to open file for reading");
    f.close();
    return 0;
  }

  f.seek(54);
  int X = x;
  int Y = y;
  uint8_t RGB[3 * X];
  for (int row = 0; row < Y; row++)
  {
    f.seek(54 + 3 * X * row);
    f.read(RGB, 3 * X);
    lcd.pushImage(0, row, X, 1, (lgfx::rgb888_t *)RGB);
  }

  f.close();
  return 0;
}

//显示文件中的图像
//BMP is 32 bit...
int print_img_small(fs::FS & fs, String filename, int x, int y, int w, int h)
{
  File f = fs.open(filename, "r");
  if (!f)
  {
    Serial.println("Failed to open file for reading");
    f.close();
    return 0;
  }

  f.seek(54);
  int X = w;
  int Y = h;
  uint8_t RGB[3 * X];
  uint8_t RGBA[4 * X];
  for (int row = 0; row < Y; row++)
  {
    f.seek(54 + 4 * X * row);
    f.read(RGBA, 4 * X);

    for (int i = 0; i < X; i++)
    {
      RGB[3 * i] = RGBA[4 * X - 4 * i - 4];
      RGB[3 * i + 1] = RGBA[4 * X - 4 * i - 3];
      RGB[3 * i + 2] = RGBA[4 * X - 4 * i - 2];
    }
    lcd.pushImage(x, y + row, X, 1, (lgfx::rgb888_t *)RGB);
  }

  f.close();
  return 0;
}



void setup()
{
  //串口初始化--115200
  Serial.begin(115200);

  //背光引脚初始化
  pinMode(LCD_BLK, OUTPUT);
  
  //BLE
  BLEDevice::init(bleServerName);  //创建BLE并设置名称
  pServer = BLEDevice::createServer();  //创建BLE服务器
  pServer->setCallbacks(new MyServerCallbacks());  //设置连接和断开调用类
  pService = pServer->createService(SERVICE_UUID); //创建BLE服务

  pCharacteristic = pService->createCharacteristic(  //Create ble feature（Characterristic_UUID）
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->setValue("ELECROW");
  //开始广播
  pAdvertising = BLEDevice::getAdvertising();  //A bleadvertising class pointer padvertising is defined, which points to bledevice:: getadvertising()
  pAdvertising->addServiceUUID(SERVICE_UUID);
  
  //OLED
  u8g2.begin();
  u8g2.enableUTF8Print();        // enable UTF8 support for the Arduino print() function
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setFontDirection(0);
  Wire.end();
  //灯
  pinMode(40, OUTPUT);
  digitalWrite(40, LOW);
  ticker1.attach(0, callback1);

  //显示屏
  lcd.init();
  //lcd.setSwapBytes(true);
  lcd.fillScreen(TFT_BLACK);
  delay(500);
  digitalWrite(LCD_BLK, HIGH);
  lcd.setTextColor(TFT_RED);
  lcd.setTextSize(2);
  lcd.setTextFont(2);
  
  //Buzzer
  pinMode(20, OUTPUT);
  ledcSetup(4, 5000, 8);
  ledcAttachPin(20, 4);
  print_img(SD, "/1.bmp", 480, 320);//显示SD卡照片
  delay(2000);
}



void loop()
{
    Serial.println("Display TF card photos");
    //lcd.fillScreen(TFT_BLACK);
    //lcd.setRotation(7);
   // lcd.fillScreen(TFT_BLACK);
}
