import time as _time
try:
    from micropython import const as _const
    _a = _const(1)
except:
    def _const(a):
        return a
try:
    import lvgl as lv
except ImportError:
    pass

_FT6x36_ADDR = _const(0x38)

_DEV_MODE_REG            = _const(0x00)
_GEST_ID_REG             = _const(0x01)
_TD_STATUS_REG           = _const(0x02)
_P1_XH_REG               = _const(0x03)
_P1_XL_REG               = _const(0x04)
_P1_YH_REG               = _const(0x05)
_P1_YL_REG               = _const(0x06)
_P1_WEIGHT_REG           = _const(0x07)
_P1_MISC_REG             = _const(0x08)
_P2_XH_REG               = _const(0x09)
_P2_XL_REG               = _const(0x0A)
_P2_YH_REG               = _const(0x0B)
_P2_YL_REG               = _const(0x0C)
_P2_WEIGHT_REG           = _const(0x0D)
_P2_MISC_REG             = _const(0x0E)
_TH_GROUP_REG            = _const(0x80)
_TH_DIFF_REG             = _const(0x85)
_CTRL_REG                = _const(0x86)
_TIMEENTERMONITOR_REG    = _const(0x87)
_PERIODACTIVE_REG        = _const(0x88)
_PERIODMONITOR_REG       = _const(0x89)
_RADIAN_VALUE_REG        = _const(0x91)
_OFFSET_LEFT_RIGHT_REG   = _const(0x92)
_OFFSET_UP_DOWN_REG      = _const(0x93)
_DISTANCE_LEFT_RIGHT_REG = _const(0x94)
_DISTANCE_UP_DOWN_REG    = _const(0x95)
_DISTANCE_ZOOM_REG       = _const(0x96)
_LIB_VER_H_REG           = _const(0xA1)
_LIB_VER_L_REG           = _const(0xA2)
_CIPHER_REG              = _const(0xA3)
_G_MODE_REG              = _const(0xA4)
_PWR_MODE_REG            = _const(0xA5)
_FIRMID_REG              = _const(0xA6)
_FOCALTECH_ID_REG        = _const(0xA8)
_RELEASE_CODE_ID_REG     = _const(0xAF)
_STATE_REG               = _const(0xBC)

GESTURE_NO_GESTRUE = _const(0)
GESTURE_MOVE_UP    = _const(1)
GESTURE_MOVE_LEFT  = _const(2)
GESTURE_MOVE_DOWN  = _const(3)
GESTURE_MOVE_RIGHT = _const(4)
GESTURE_ZOOM_IN    = _const(5)
GESTURE_ZOOM_OUT   = _const(6)

POLLING_MODE = _const(0x00)
TRIGGER_MODE = _const(0x01)

class FT6x36():
    '''
    FocalTech Self-Capacitive Touch Panel Controller module

    :param I2C i2c: The board I2C object
    :param int address: The I2C address
    :param Pin rst: The reset Pin object
    '''
    def __init__(self, i2c: I2C, address: int = _FT6x36_ADDR, rst: Pin = None) -> None:
        self._i2c = i2c
        self._address = address
        self._rst = rst
        self._read_buffer = bytearray(4)
        self._write_buffer = bytearray(1)

    def get_gesture(self) -> int:
        '''
        Get Gesture events. Should be a value of:

        * ``GESTURE_NO_GESTRUE``: No Gesture
        * ``GESTURE_MOVE_UP``: Move Up
        * ``GESTURE_MOVE_RIGHT``: Move Right
        * ``GESTURE_MOVE_DOWN``: Move Down
        * ``GESTURE_MOVE_LEFT``: Move Left
        * ``GESTURE_ZOOM_IN``: Zoom In
        * ``GESTURE_ZOOM_OUT``: Zoom Out
        '''
        gesture = self._i2c.readfrom_mem(self._address, _GEST_ID_REG, 1)[0]
        if 0x10 == gesture:
            return GESTURE_MOVE_UP
        elif 0x14 == gesture:
            return GESTURE_MOVE_RIGHT
        elif 0x18 == gesture:
            return GESTURE_MOVE_DOWN
        elif 0x1C == gesture:
            return GESTURE_MOVE_LEFT
        elif 0x48 == gesture:
            return GESTURE_ZOOM_IN
        elif 0x49 == gesture:
            return GESTURE_ZOOM_OUT
        else:
            return GESTURE_NO_GESTRUE

    def get_positions(self) -> list:
        positions = []
        num_points = self._i2c.readfrom_mem(self._address, _TD_STATUS_REG, 1)[0] & 0x0F
        if num_points > 0:
            positions.append(self._get_p1())
        if num_points > 1:
            positions.append(self._get_p2())
        return positions

    @property
    def theshold(self) -> int:
        '''
        Threshold for touch detection.
        '''
        return self._i2c.readfrom_mem(self._address, _TH_GROUP_REG, 1)[0]

    @theshold.setter
    def theshold(self, val: int) -> None:
        self._write_buffer[0] = val
        self._i2c.writeto_mem(self._address, _TH_GROUP_REG, self._write_buffer)

    @property
    def monitor_time(self) -> int:
        '''
        The time period of switching from Active mode to Monitor mode when there is no touching.
        '''
        return self._i2c.readfrom_mem(self._address, _TIMEENTERMONITOR_REG, 1)[0]

    @monitor_time.setter
    def monitor_time(self, val: int) -> None:
        self._write_buffer[0] = val
        self._i2c.writeto_mem(self._address, _TIMEENTERMONITOR_REG, self._write_buffer)

    @property
    def active_period(self) -> int:
        '''
        Report rate in Active mode.
        '''
        return self._i2c.readfrom_mem(self._address, _PERIODACTIVE_REG, 1)[0]

    @active_period.setter
    def active_period(self, val: int) -> None:
        self._write_buffer[0] = val
        self._i2c.writeto_mem(self._address, _PERIODACTIVE_REG, self._write_buffer)

    @property
    def monitor_period(self) -> int:
        '''
        Report rate in Monitor mode.
        '''
        return self._i2c.readfrom_mem(self._address, _PERIODMONITOR_REG, 1)[0]

    @monitor_period.setter
    def monitor_period(self, val: int) -> None:
        self._write_buffer[0] = val
        self._i2c.writeto_mem(self._address, _PERIODMONITOR_REG, self._write_buffer)

    @property
    def library_version(self):
        '''
        Library Version info.
        '''
        buffer = self._i2c.readfrom_mem(self._address, _LIB_VER_H_REG, 2)
        return buffer[0] << 8 | buffer[1]

    @property
    def firmware_version(self) -> int:
        '''
        Firmware Version.
        '''
        return self._i2c.readfrom_mem(self._address, _FIRMID_REG, 1)

    @property
    def interrupt_mode(self) -> int:
        '''
        Interrupt mode for valid data. Should be a value of:

        * ``POLLING_MODE``: Interrupt Polling mode
        * ``TRIGGER_MODE``:  Interrupt Trigger mode
        '''
        return self._i2c.readfrom_mem(self._address, _G_MODE_REG, 1)[0]

    @interrupt_mode.setter
    def interrupt_mode(self, val: int) -> None:
        self._write_buffer[0] = val
        self._i2c.writeto_mem(self._address, _G_MODE_REG, self._write_buffer)

    @property
    def power_mode(self) -> int:
        '''
        Current power mode which system is in.
        '''
        return self._i2c.readfrom_mem(self._address, _PWR_MODE_REG, 1)[0]

    @power_mode.setter
    def power_mode(self, val: int) -> None:
        self._write_buffer[0] = val
        self._i2c.writeto_mem(self._address, _PWR_MODE_REG, self._write_buffer)

    @property
    def vendor_id(self) -> None:
        '''
        Chip Selecting.
        '''
        return self._i2c.readfrom_mem(self._address, _CIPHER_REG, 1)[0]

    @property
    def panel_id(self) -> None:
        '''
        FocalTech’s Panel ID.
        '''
        return self._i2c.readfrom_mem(self._address, _FOCALTECH_ID_REG, 1)[0]

    def reset(self) -> None:
        '''
        Hardware reset touch screen.
        '''
        if self._rst is None:
            return
        self._rst.off()
        _time.sleep_ms(1)
        self._rst.on()

    def _get_p1(self) -> tuple:
        self._i2c.readfrom_mem_into(self._address, _P1_XH_REG, self._read_buffer)
        return ((self._read_buffer[0] << 8 | self._read_buffer[1]) & 0x0FFF,
                (self._read_buffer[2] << 8 | self._read_buffer[3]) & 0x0FFF)

    def _get_p2(self) -> tuple:
        self._i2c.readfrom_mem_into(self._address, _P2_XH_REG, self._read_buffer)
        return ((self._read_buffer[0] << 8 | self._read_buffer[1]) & 0x0FFF,
                (self._read_buffer[2] << 8 | self._read_buffer[3]) & 0x0FFF)

    def lvgl_read(self, indev_drv, data):
        positions = self.get_positions()
        if len(positions):
            data.point.x = 480 - positions[0][1]
            data.point.y = positions[0][0]
            data.state = lv.INDEV_STATE.PRESSED
            return False
        data.state = lv.INDEV_STATE.RELEASED
        return False
