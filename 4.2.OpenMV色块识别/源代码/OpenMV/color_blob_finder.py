'''
色块识别
'''
import sensor
import image
import time
import ustruct as struct
from pyb import LED

# 红色小球的LAB色彩空间阈值 (L Min, L Max, A Min, A Max, B Min, B Max)
RED_BALL_THRESHOLD = (12,80,17,58,-15,55)


red_led   = LED(1)
green_led = LED(2)
blue_led  = LED(3)

def led_control(x):
    if  x == 1:
        red_led.on()
        green_led.off()
        blue_led.off()
    elif  x == 2:
        red_led.off()
        green_led.on()
        blue_led.off()
    elif  x == 3:
        red_led.off()
        green_led.off()
        blue_led.on()
    else :
        red_led.off()
        green_led.off()
        blue_led.off()

# OpenMV感光芯片初始化
sensor.reset() # 重置感芯片
sensor.set_pixformat(sensor.RGB565) # 设置像素格式为RGB565
sensor.set_framesize(sensor.QVGA) # 设置分辨率为QVGA (320 * 240)
sensor.set_vflip(True) # 画面反转
sensor.skip_frames(time = 2000) # 跳过2s内的帧, 等待画质稳定
sensor.set_auto_gain(False) # 关闭自动增益
sensor.set_auto_whitebal(False) # 关闭自动白平衡

# 初始化时钟
clock = time.clock()

while(True):
    clock.tick() # 开始计时
    img = sensor.snapshot() # 拍摄一张照片
    # 获取画面中的色块(选定阈值，像素数量大于100，色块面积大于100，合并重叠的blob)
    blobs = img.find_blobs([RED_BALL_THRESHOLD], pixels_threshold=100, area_threshold=100, merge=True)
    if len(blobs) != 0:
        # 获得画面中的最大的色块
        blob = max(blobs, key=lambda  b: b.area())
        # 可视化
        led_control(1)
        img.draw_rectangle(blob.rect()) # 绘制色块的矩形区域
        img.draw_cross(blob.cx(), blob.cy()) # 绘制色块的中心位置
        print("色块中心坐标: {} {}".format(blob.cx(), blob.cy()))
    else:
        # 目标丢失
        led_control(0)
        print("色块丢失!!!")
    # 打印当前的帧率
    print(clock.fps())
