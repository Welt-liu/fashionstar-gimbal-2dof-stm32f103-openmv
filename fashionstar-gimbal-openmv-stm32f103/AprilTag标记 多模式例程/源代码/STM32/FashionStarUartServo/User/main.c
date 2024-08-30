/***************************************************
 * AprilTag 多标记应用例程
 ***************************************************/
#include "stm32f10x.h"
#include "usart.h"
#include "sys_tick.h"
#include "fashion_star_uart_servo.h"
#include "gimbal.h"

#define TRUE 1
#define FALSE 0

#define IMG_WIDTH 160.0			  // blob画面分辨率 宽度
#define IMG_HEIGHT 120.0		  // blob画面分辨率 高度
#define BLOB_PKG_LEN 7			  // blob数据包的长度
#define BLOB_PKG_HEADER 0xFF	  // 帧头
#define BLOB_PKG_CMD_ID 0xF1	  // 指令ID
#define BLOB_PKG_CMD_ID_TAG0 0xA0 // 指令ID
#define BLOB_PKG_CMD_ID_TAG1 0xA1 // 指令ID
#define BLOB_PKG_CMD_ID_TAG2 0xA2 // 指令ID

// 云台偏航角PID控制
#define DEAD_BLOCK 0.05
#define GIMBAL_YAW_KP 40.0
#define GIMBAL_YAW_KD 2.0
// 云台俯仰角PID控制
#define GIMBAL_PITCH_KP 20.0
#define GIMBAL_PITCH_KD 3.0

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
// 使用串口2作为日志输出的端口
// <接线说明>
// STM32F103 PA2(Tx) <----> USB转TTL Rx
// STM32F103 PA3(Rx) <----> USB转TTL Tx
// STM32F103 GND 	 <----> USB转TTL GND
// STM32F103 V5 	 <----> USB转TTL 5V (可选)
// <注意事项>
// 使用前确保已设置usart.h里面的USART2_ENABLE为1
Usart_DataTypeDef *loggingUsart = &usart2;
// 重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	while ((loggingUsart->pUSARTx->SR & 0X40) == 0)
	{
	}
	/* 发送一个字节数据到串口 */
	USART_SendData(loggingUsart->pUSARTx, (uint8_t)ch);
	/* 等待发送完毕 */
	// while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
	return (ch);
}

// 使用串口3接收来自blob的消息
// <接线说明>
// STM32F103 PB10(Tx) <----> blob P5 (UART3 Rx)
// STM32F103 PB11(Rx) <----> blob P4 (UART3 Tx)
// STM32F103 GND 	  <----> blob GND
// STM32F103 V5 	  <----> blob Vin (5v)
// <注意事项>
// 使用前确保已设置usart.h里面的USART3_ENABLE为1
Usart_DataTypeDef *blobUsart = &usart3;

uint8_t blobPkgBuf[BLOB_PKG_LEN]; // blob数据帧缓冲区
uint8_t blobPkgIdx = 0;

float servoSpeed = 400.0; // 云台旋转速度 (单位: °/s)
uint8_t hasTag = FALSE;	  // 画面中有无标记
uint8_t curCmdId = 0XF1;  // 当前指令ID
uint16_t blobCx = 0;	  // 标记中心的x坐标
uint16_t blobCy = 0;	  // 标记中心的y坐标

float curXOffset = 0;  // 当前x轴方向上的偏移量
float lastXOffset = 0; // 上一次x轴方向上的偏移量

float curYOffset = 0;  // 当前y轴方向上的偏移量
float lastYOffset = 0; // 上一次y轴方向上的偏移量
uint8_t curMode = 0;   // 当前模式

// 更新标记的信息
void updateBlobInfo(Usart_DataTypeDef *blobUsart)
{
	uint8_t tempByte;
	while (RingBuffer_GetByteUsed(blobUsart->recvBuf))
	{
		// 弹出队首元素
		tempByte = RingBuffer_Pop(blobUsart->recvBuf);

		/*错误处理*/
		if (blobPkgIdx == 0 && tempByte != BLOB_PKG_HEADER)
		{
			// 帧头还未接收且帧头不匹配
			continue;
		}
		// 帧头接收完成,且cmd_id不在预设范围内
		else if (blobPkgIdx == 1 && tempByte != BLOB_PKG_CMD_ID && tempByte != BLOB_PKG_CMD_ID_TAG0 && tempByte != BLOB_PKG_CMD_ID_TAG1 && tempByte != BLOB_PKG_CMD_ID_TAG2)
		{
			// 数据指令不匹配
			blobPkgIdx = 0;
			continue;
		}

		// 缓冲区内追加数据
		blobPkgBuf[blobPkgIdx] = tempByte;
		blobPkgIdx += 1;

		// 数据接收完成
		if (blobPkgIdx >= BLOB_PKG_LEN)
		{
			// 数据接收完成, 解析更新数据
			curCmdId = blobPkgBuf[1];
			hasTag = blobPkgBuf[2];
			// 高通滤波, 抵抗图像处理噪声
			blobCx = 0.3 * blobCx + 0.7 * (uint16_t)(blobPkgBuf[3] | blobPkgBuf[4] << 8);
			blobCy = 0.3 * blobCy + 0.7 * (uint16_t)(blobPkgBuf[5] | blobPkgBuf[6] << 8);

			blobPkgIdx = 0; // 游标清零
		}
	}
}

// 云台偏航角 PID控制
void GimbalYawPIDCtl()
{
	float dYaw; // 偏航角的增量

	// 更新上一次的偏移量
	lastXOffset = curXOffset;
	// 计算当前的偏移量
	curXOffset = ((blobCx / IMG_WIDTH) - 0.5);
	// 判断是否在死区内
	if (__fabs(curXOffset) <= DEAD_BLOCK)
	{
		curXOffset = 0;
	}

	// 计算得到偏航角的增量
	dYaw = GIMBAL_YAW_KP * curXOffset + GIMBAL_YAW_KD * (lastXOffset - curXOffset);
	// 云台偏航角控制
	Gimbal_SetYaw(servoUsart, curYaw + dYaw, servoSpeed);

	printf("YAW PID: dYaw = %.1f next yaw = %.1f\r\n", dYaw, curYaw + dYaw);
}

// 云台俯仰角 PID控制
void GimbalPitchPIDCtl()
{
	float dPitch; // 俯仰角的增量

	// 更新上一次的偏移量
	lastYOffset = curYOffset;
	// 计算当前的偏移量
	curYOffset = ((blobCy / IMG_HEIGHT) - 0.5);
	// 判断是否在死区内
	if (__fabs(curYOffset) <= DEAD_BLOCK)
	{
		curYOffset = 0;
	}

	// 计算得到偏航角的增量
	dPitch = GIMBAL_PITCH_KP * curYOffset + GIMBAL_PITCH_KD * (lastYOffset - curYOffset);
	// 云台偏航角控制
	Gimbal_SetPitch(servoUsart, curPitch + dPitch, 500);

	printf("YAW PID: dPitch = %.1f next pitch = %.1f\r\n", dPitch, curPitch + dPitch);
}
static uint16_t curState = 0; // 当前状态
// 巡视模式
void PatrolMode()
{
	/*
		识别到标记2：巡视模式
		不能用阻塞去写该模式代码，因为识别标记信息的处理在主循环。
		这里只用了10ms的阻塞，curState计算时间，时间到了发送舵机控制指令
		如此下来，舵机循环执行巡视任务。
	*/
	curState++;
	if (curState > 800)
		curState = 0;

	if (curState == 1)
	{
		// 阶段1
		Gimbal_SetYaw(servoUsart, -90, 100);
		Gimbal_SetPitch(servoUsart, 0, 50);
	}
	else if (curState == 200)
	{
		// 阶段2
		Gimbal_SetYaw(servoUsart, 90, 100);
	}
	else if (curState == 400)
	{
		// 阶段3
		Gimbal_SetPitch(servoUsart, -20, 50);
	}
	else if (curState == 600)
	{
		// 阶段4
		Gimbal_SetYaw(servoUsart, -90, 100);
	}
	SysTick_DelayMs(10);
}

int main(void)
{
	SysTick_Init();			 // 嘀嗒定时器初始化
	Usart_Init();			 // 串口初始化
	Gimbal_Init(servoUsart); // 云台初始化

	while (1)
	{
		// 更新标记的位置信息
		if (RingBuffer_GetByteUsed(blobUsart->recvBuf) >= BLOB_PKG_LEN)
		{
			updateBlobInfo(blobUsart);
		}
		// 更新云台位姿
		Gimbal_Update(servoUsart);
		// 判断CMD_ID
		if (curCmdId != BLOB_PKG_CMD_ID)
		{
			// 切换模式
			// printf("enter %d\r\n",curCmdId);
			curMode = curCmdId;
			// 例程需要，巡视模式做额外处理。
			if (curMode != BLOB_PKG_CMD_ID_TAG1)
				curState = 0;
		}

		switch (curMode)
		{
			// 标记0，复位云台
		case BLOB_PKG_CMD_ID_TAG0:
			if (hasTag) // 识别到标记
				Gimbal_Reset(servoUsart);
			break;
		case BLOB_PKG_CMD_ID_TAG1:
		{
			// 标记1，巡视模式
			PatrolMode();
		}
		break;
		case BLOB_PKG_CMD_ID_TAG2:
			// 标记2，云台追随
			if (hasTag) // 识别到标记
			{
				//双轴PID控制
				GimbalYawPIDCtl();
				GimbalPitchPIDCtl();
			}
			break;
		default:
			break;
		}

		// 延时1ms
		// SysTick_DelayMs(1);
	}
}
