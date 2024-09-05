'''
AprilTag识别
'''

import sensor, image, time, math
import ustruct as struct
from pyb import UART
from pyb import LED

# 串口3 速率115200
uart = UART(3, 115200)
# 设置led
red_led   = LED(1)
green_led = LED(2)
blue_led  = LED(3)
# 定义led控制，0:红灯，1：绿灯，2：蓝灯
def led_control(x):
    if  x == 0:
        red_led.on()
        green_led.off()
        blue_led.off()
    elif  x == 1:
        red_led.off()
        green_led.on()
        blue_led.off()
    elif  x == 2:
        red_led.off()
        green_led.off()
        blue_led.on()
    else :
        red_led.off()
        green_led.off()
        blue_led.off()

# OpenMV感光芯片初始化
sensor.reset()
sensor.set_pixformat(sensor.RGB565)# 设置像素格式为RGB565
sensor.set_framesize(sensor.QQVGA) # 设置分辨率为QQVGA (160 * 120)
sensor.skip_frames(30) # 跳过30ms内的帧, 等待画质稳定
sensor.set_auto_gain(False)   # 关闭自动增益
sensor.set_auto_whitebal(False)  # 关闭自动白平衡
sensor.set_vflip(True) # 纵向翻转

# 初始化时钟
clock = time.clock()


def degrees(radians):
    return (180 * radians) / math.pi

while(True):
    clock.tick()# 开始计时
    img = sensor.snapshot()# 拍摄一张照片
    # 获取画面中的标记()
    tags = img.find_apriltags()

    if tags:
        # 找到至少一个 AprilTag
        for tag in tags:
            # 可视化
            img.draw_rectangle(tag.rect(), color = (255, 0, 0))
            img.draw_cross(tag.cx(), tag.cy(), color = (0, 255, 0))
            # 找到TAG36H11标签
            if tag.family() == image.TAG36H11:
                # 识别到id为0
                # 红灯亮
                # 通过串口发送数据(二进制　低字节序)
                if tag.id() == 0:
                    led_control(0)
                    uart.write(struct.pack('<BBBHH', 0xFF,0xA0 , True, tag.cx(), tag.cy()))
                # 识别到id为1
                # 绿灯亮
                # 通过串口发送数据(二进制　低字节序)
                elif tag.id() == 1:
                    led_control(1)
                    uart.write(struct.pack('<BBBHH', 0xFF,0xA1 , True, tag.cx(), tag.cy()))
                # 识别到id为2
                # 蓝灯亮
                # 通过串口发送数据(二进制　低字节序)
                elif tag.id() == 2:
                    led_control(2)
                    uart.write(struct.pack('<BBBHH', 0xFF,0xA2 , True, tag.cx(), tag.cy()))
    else:
        # 没有找到任何 AprilTag
        # 熄灯
        # 通过串口发送数据(二进制　低字节序)
        led_control(3)
        uart.write(struct.pack('<BBBHH', 0xFF, 0xF1, False, 0, 0))
#    print(clock.fps())
