/***************************************************
 * 二轴云台多模式运动
 ***************************************************/
#include "stm32f10x.h"
#include "math.h"
#include "usart.h"
#include "sys_tick.h"
#include "fashion_star_uart_servo.h"
// #include "gimbal.h"

// 使用串口1作为舵机控制的端口
// <接线说明>
// STM32F103 PA9(Tx)  <----> 串口舵机转接板 Rx
// STM32F103 PA10(Rx) <----> 串口舵机转接板 Tx
// STM32F103 GND 	  <----> 串口舵机转接板 GND
// STM32F103 V5 	  <----> 串口舵机转接板 5V
// <注意事项>
// 使用前确保已设置usart.h里面的USART1_ENABLE为1
// 设置完成之后, 将下行取消注释
Usart_DataTypeDef *servoUsart = &usart1;

#define YAW_SERVO 0
#define PIT_SERVO 1

// 选择角度控制方式
#define SET_ANGLE_BY_INTERVAL 1 //带加减速的角度控制(指定周期)
#define SET_ANGLE 0 //简易角度控制 


int main(void)
{
	// 嘀嗒定时器初始化
	SysTick_Init();
	Usart_Init(); // 串口初始化

	FSUS_SetServoAngleByInterval(servoUsart, YAW_SERVO, -10, 500, 100, 100, 0, 0);
	SysTick_DelayMs(1000);

	FSUS_SetServoAngleByInterval(servoUsart, PIT_SERVO, 0, 500, 100, 100, 0, 0);
	// 等待2s
	SysTick_DelayMs(1000);

	while (1)
	{
		#if SET_ANGLE_BY_INTERVAL
		FSUS_SetServoAngleByInterval(servoUsart, YAW_SERVO, -30, 300, 100, 100, 0, 1);
		FSUS_SetServoAngleByInterval(servoUsart, YAW_SERVO, 30, 600, 300, 300, 0, 1);
		FSUS_SetServoAngleByInterval(servoUsart, YAW_SERVO, -9, 300, 100, 100, 0, 1);

		FSUS_SetServoAngleByInterval(servoUsart, PIT_SERVO, -20, 300, 100, 100, 0, 1);
		FSUS_SetServoAngleByInterval(servoUsart, PIT_SERVO, 20, 600, 300, 300, 0, 1);
		FSUS_SetServoAngleByInterval(servoUsart, PIT_SERVO, 2.5, 300, 100, 100, 0, 1);
		
		FSUS_SetServoAngleByInterval(servoUsart, YAW_SERVO, -30, 300, 100, 100, 0, 1);
		FSUS_SetServoAngleByInterval(servoUsart, YAW_SERVO, 30, 600, 300, 300, 0, 1);
		FSUS_SetServoAngleByInterval(servoUsart, YAW_SERVO, 10, 300, 100, 100, 0, 1);
		
		FSUS_SetServoAngleByInterval(servoUsart, PIT_SERVO, -20, 300, 100, 100, 0, 1);
		FSUS_SetServoAngleByInterval(servoUsart, PIT_SERVO, 20, 600, 300, 300, 0, 1);
		FSUS_SetServoAngleByInterval(servoUsart, PIT_SERVO, 2.5, 300, 100, 100, 0, 1);
		
		#elif SET_ANGLE
		FSUS_SetServoAngle(servoUsart, YAW_SERVO, -30, 300,  0, 1);
		FSUS_SetServoAngle(servoUsart, YAW_SERVO, 30, 600, 0, 1);
		FSUS_SetServoAngle(servoUsart, YAW_SERVO, -9, 300, 0, 1);

		FSUS_SetServoAngle(servoUsart, PIT_SERVO, -20, 300, 0, 1);
		FSUS_SetServoAngle(servoUsart, PIT_SERVO, 20, 600, 0, 1);
		FSUS_SetServoAngle(servoUsart, PIT_SERVO, 2.5, 300, 0, 1);
		
		FSUS_SetServoAngle(servoUsart, YAW_SERVO, -30, 300, 0, 1);
		FSUS_SetServoAngle(servoUsart, YAW_SERVO, 30, 600, 0, 1);
		FSUS_SetServoAngle(servoUsart, YAW_SERVO, 10, 300, 0, 1);
		
		FSUS_SetServoAngle(servoUsart, PIT_SERVO, -20, 300,  0, 1);
		FSUS_SetServoAngle(servoUsart, PIT_SERVO, 20, 600, 0, 1);
		FSUS_SetServoAngle(servoUsart, PIT_SERVO, 2.5, 300,  0, 1);		
		#endif
	}
}
