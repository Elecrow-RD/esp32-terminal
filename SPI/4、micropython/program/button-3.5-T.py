"""
www.elecrow.cn
lv+micropython：1.19.1
"""

import lvgl as lv
import time
from espidf import VSPI_HOST
from ili9XXX import ili9488
from ft6x36 import ft6x36 
from machine import Pin, I2C

# ------------------------------ Initialize the screen --start------------------------

# Light up the screen
p16 = Pin(16, Pin.OUT)
p16.value(1)

# Creating the display object
disp = ili9488(miso=14, mosi=13, clk=12, cs=3, dc=42, rst=18,backlight=46, backlight_on=1, power_on=1,
               spihost=VSPI_HOST, mhz=20,power=-1,
               factor=16, hybrid=True, width=320, height=480,
               invert=False, double_buffer=True, half_duplex=False)

# Create a touch screen object
touch = ft6x36(sda=2, scl=1)
# --------------------------------stop------------------------

# 1. Create a display screen. Will need to display the component added to the screen to display
scr = lv.obj()  # scr====> screen 
scr = lv.scr_act()
scr.clean()

# 2. Encapsulated buttons that need to be displayed
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

        # Get the first child of the button which is the label and change its text
        label = btn.get_child(0)
        label.set_text("Button: " + str(self.cnt))  # 


# 3. create button
counterBtn = CounterBtn(scr)

# 4. Displays the contents of the screen object
lv.scr_load(scr)


