#include "button.h"


float yaw_set = 0;			 // 云台偏航角
float pitch_set = 0;		 // 云台俯仰角
uint8_t button_yaw_Flag = 0; // 按键标志位
uint8_t button_pit_Flag = 0; // 按键标志位

void EXTI4_IRQHandler(void)
{

	if (EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
			EXTI_ClearITPendingBit(EXTI_Line4);

		if (button_yaw_Flag != 0)
			return;
		button_yaw_Flag = 1;
		// 偏航角增加
		yaw_set = yaw_set + BUTTON_ANGLE_STEP;
		yaw_set = yaw_set > YAW_MAX ? YAW_MAX : yaw_set;
		// 清除中断标志
	}
}

void EXTI9_5_IRQHandler(void)
{

	if (EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
			EXTI_ClearITPendingBit(EXTI_Line5);

		if (button_yaw_Flag != 0)
			return;
		button_yaw_Flag = 1;
		yaw_set = yaw_set - BUTTON_ANGLE_STEP; // 偏航角减少
		yaw_set = yaw_set < YAW_MIN ? YAW_MIN : yaw_set;
		// 清除中断标志
	}

	if (EXTI_GetITStatus(EXTI_Line6) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line6); // 清除中断标志

		if (button_pit_Flag != 0)
			return;
		button_pit_Flag = 1;
		pitch_set = pitch_set + BUTTON_ANGLE_STEP; // 俯仰角增加
		pitch_set = pitch_set > PITCH_MAX ? PITCH_MAX : pitch_set;
	}
	if (EXTI_GetITStatus(EXTI_Line7) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line7); // 清除中断标志

		if (button_pit_Flag != 0)
			return;
		button_pit_Flag = 1;
		pitch_set = pitch_set - BUTTON_ANGLE_STEP; // 俯仰角减少
		pitch_set = pitch_set < PITCH_MIN ? PITCH_MIN : pitch_set;
	}
}

// 配置嵌套向量中断控制器
void Button_NVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// 按键GPIO配置
void Button_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;		  // IO配置结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // 上拉输入

	// PB4: 按键-偏航角增加
	GPIO_InitStructure.GPIO_Pin = BUTTON_YAW_ADD_PIN;
	GPIO_Init(BUTTON_YAW_ADD_PORT, &GPIO_InitStructure);
	// PB5: 按键-偏航角减少
	GPIO_InitStructure.GPIO_Pin = BUTTON_YAW_MINUS_PIN;
	GPIO_Init(BUTTON_YAW_MINUS_PORT, &GPIO_InitStructure);
	// PB6: 按键-俯仰角增加
	GPIO_InitStructure.GPIO_Pin = BUTTON_PITCH_ADD_PIN;
	GPIO_Init(BUTTON_PITCH_ADD_PORT, &GPIO_InitStructure);
	// PB7: 按键-俯仰角减少
	GPIO_InitStructure.GPIO_Pin = BUTTON_PITCH_MINUS_PIN;
	GPIO_Init(BUTTON_PITCH_MINUS_PORT, &GPIO_InitStructure);
}

// 按键外部中断配置
void Button_EXTI(void)
{
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_Init(&EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_Init(&EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_Init(&EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7);
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_Init(&EXTI_InitStructure);
}

// 按键初始化
void Button_Init(void)
{
	// 开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	// 配置嵌套向量中断控制器
	Button_NVIC();
	// 按键GPIO配置
	Button_GPIO();
	// 按键外部中断配置
	Button_EXTI();
}
