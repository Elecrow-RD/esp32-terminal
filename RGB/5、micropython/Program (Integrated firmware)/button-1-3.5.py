import lvgl as lv
import lv_utils
import tft_config
import ft6x36
from machine import Pin, I2C
import time


WIDTH = 480
HEIGHT = 320

# tft drvier
tft = tft_config.config()

# touch drvier
i2c = I2C(1, scl=Pin(39), sda=Pin(38), freq=400000)

tp = ft6x36.FT6x36(i2c)

lv.init()

if not lv_utils.event_loop.is_running():
    event_loop=lv_utils.event_loop()
    print(event_loop.is_running())

# create a display 0 buffer
disp_buf0 = lv.disp_draw_buf_t()
buf1_0 = bytearray(WIDTH * HEIGHT * 2)
disp_buf0.init(buf1_0, None, len(buf1_0) // lv.color_t.__SIZE__)

def lvgl_flush(disp_drv, area, color_p):
    size = (area.x2 - area.x1 + 1) * (area.y2 - area.y1 + 1)
    tft.bitmap(area.x1, area.y1, area.x2, area.y2, color_p.__dereference__(size * lv.color_t.__SIZE__))
    disp_drv.flush_ready()

# register display driver
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.draw_buf = disp_buf0
disp_drv.flush_cb = tft.flush#lvgl_flush
disp_drv.hor_res = WIDTH
disp_drv.ver_res = HEIGHT
# disp_drv.user_data = {"swap": 0}
disp0 = disp_drv.register()
lv.disp_t.set_default(disp0)

# touch driver init
indev_drv = lv.indev_drv_t()
indev_drv.init()
indev_drv.disp = disp0
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = tp.lvgl_read
indev = indev_drv.register()

scr = lv.obj()


class CounterBtn():
    def __init__(self, scr):
        self.cnt = 0
        btn = lv.btn(scr)
        btn.set_size(120, 50)
        btn.align(lv.ALIGN.CENTER,0,0)
        btn.add_event_cb(self.btn_event_cb, lv.EVENT.ALL, None)
        label = lv.label(btn)
        label.set_text("Button")
        label.center()

    def btn_event_cb(self, evt):
        code = evt.get_code()
        btn = evt.get_target()
        if code == lv.EVENT.CLICKED:
            self.cnt += 1


        label = btn.get_child(0)
        label.set_text("Button: " + str(self.cnt))


counterBtn = CounterBtn(scr)
lv.scr_load(scr)

try:
    from machine import WDT
    wdt = WDT(timeout=1000)  # enable it with a timeout of 2s
    print("Hint: Press Ctrl+C to end the program")
    while True:
        wdt.feed()
        time.sleep(0.9)
except KeyboardInterrupt as ret:
    print("The program stopped running, ESP32 has restarted...")
