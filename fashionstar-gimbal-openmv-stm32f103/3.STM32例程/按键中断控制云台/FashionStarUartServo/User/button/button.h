/*
 * 
 * 作者: FashionStar
 * 日期: 2024-08-29
 */

#ifndef __BUTTON_H
#define __BUTTON_H

#include "stm32f10x.h"
#include "sys_tick.h"

// PB0: 偏航角增加按钮端口和引脚
#define BUTTON_YAW_ADD_PORT GPIOB
#define BUTTON_YAW_ADD_PIN GPIO_Pin_4
// PB1: 偏航角减少按钮端口和引脚
#define BUTTON_YAW_MINUS_PORT GPIOB
#define BUTTON_YAW_MINUS_PIN GPIO_Pin_5
// PB5: 俯仰角增加按钮端口和引脚
#define BUTTON_PITCH_ADD_PORT GPIOB
#define BUTTON_PITCH_ADD_PIN GPIO_Pin_6
// PB4: 俯仰角减少按钮端口和引脚
#define BUTTON_PITCH_MINUS_PORT GPIOB
#define BUTTON_PITCH_MINUS_PIN GPIO_Pin_7

#define BUTTON_NUM 4 // 按钮数量

// 按钮ID定义
#define BUTTON_ID_YAW_ADD 0
#define BUTTON_ID_YAW_MINUS 1
#define BUTTON_ID_PITCH_ADD 2
#define BUTTON_ID_PITCH_MINUS 3

// 云台的角度范围
#define YAW_MIN -90
#define YAW_MAX 90
#define PITCH_MIN -90
#define PITCH_MAX 50

// 角度步进值
#define BUTTON_ANGLE_STEP 5

// 配置NVIC中断
void Button_NVIC_Config(void);
// 配置GPIO
void Button_GPIO(void);
// 配置外部中断
void Button_EXTI(void);
// 初始化按钮
void Button_Init(void);

extern float yaw_set; // 云台偏航角
extern float pitch_set; // 云台俯仰角

extern uint8_t button_yaw_Flag; // 偏航角按钮标志位
extern uint8_t button_pit_Flag; // 俯仰角按钮标志位

#endif
