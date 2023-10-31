import lcd
import machine

def config():

    tft = lcd.DPI(
        data = (
            machine.Pin(8),  \
            machine.Pin(3),  \
            machine.Pin(46), \
            machine.Pin(9),  \
            machine.Pin(1),  \
            machine.Pin(5),  \
            machine.Pin(6),  \
            machine.Pin(7),  \
            machine.Pin(15), \
            machine.Pin(16), \
            machine.Pin(4),  \
            machine.Pin(45), \
            machine.Pin(48), \
            machine.Pin(47), \
            machine.Pin(21), \
            machine.Pin(14), \
        ), \
        hsync = machine.Pin(39),          \
        vsync = machine.Pin(41),          \
        de = machine.Pin(40),             \
        pclk_pin = machine.Pin(0),       \
        timings = (4, 43, 8, 4, 12, 8), \
        backlight = machine.Pin(2),      \
        pclk = 16000000,          \
        width = 800,                      \
        height = 480
    )
    tft.reset()
    tft.init()
    tft.backlight_on()
    return tft
