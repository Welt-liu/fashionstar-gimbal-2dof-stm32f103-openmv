/***************************************************
 * 标定结果测试
 * 主循环里设置云台控制目标，在两个姿态之间重复旋转。	
 ***************************************************/
#include "stm32f10x.h"
#include "usart.h"
#include "sys_tick.h"
#include "fashion_star_uart_servo.h"
#include "gimbal.h"


// 使用串口1作为舵机控制的端口
// <接线说明>
// STM32F103 PA9(Tx)  <----> 串口舵机转接板 Rx
// STM32F103 PA10(Rx) <----> 串口舵机转接板 Tx
// STM32F103 GND 	  <----> 串口舵机转接板 GND
// STM32F103 V5 	  <----> 串口舵机转接板 5V
// <注意事项>
// 使用前确保已设置usart.h里面的USART1_ENABLE为1
// 设置完成之后, 将下行取消注释
Usart_DataTypeDef* servoUsart = &usart1; 

float servoSpeed = 200.0; // 云台旋转速度 (单位: °/s)
int main (void)
{
	// 嘀嗒定时器初始化
	SysTick_Init();
	Usart_Init(); // 串口初始化
	// 云台初始化
	Gimbal_Init(servoUsart);
	// 等待2s
	SysTick_DelayMs(2000);	
	
	while (1){
		// 设置云台目标位姿
		Gimbal_SetYaw(servoUsart, 60, servoSpeed);
		Gimbal_SetPitch(servoUsart, 45, servoSpeed);
		// 等待云台旋转到目标位置
		Gimbal_Wait(servoUsart);
		
		// 延时1s
		SysTick_DelayMs(1000);
		
		// 设置云台目标位姿
		Gimbal_SetYaw(servoUsart, -60, servoSpeed);
		Gimbal_SetPitch(servoUsart, -45, servoSpeed);
		// 等待云台旋转到目标位置
		Gimbal_Wait(servoUsart);
		
		// 延时1s
		SysTick_DelayMs(1000);
    }
}
