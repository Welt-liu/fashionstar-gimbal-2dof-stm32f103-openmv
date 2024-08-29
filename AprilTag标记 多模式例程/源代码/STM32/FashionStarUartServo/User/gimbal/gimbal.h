
#ifndef __GIMBAL_H
#define __GIMBAL_H

#include "stm32f10x.h"
#include "usart.h"
#include "fashion_star_uart_servo.h"
#include "sys_tick.h"

// 舵机参数定义
#define SERVO_ID_YAW 0 // 水平舵机ID
#define SERVO_ID_PITCH 1 // 垂直舵机ID
#define SERVO_DEAD_BLOCK 2.0 // 死区

// 角度定义
#define YAW1 90.0
#define YAW1_SERVO_ANGLE -90.4
#define YAW2 -90.0
#define YAW2_SERVO_ANGLE 95.1

#define PITCH1 -60.0
#define PITCH1_SERVO_ANGLE 66.2
#define PITCH2 90.0
#define PITCH2_SERVO_ANGLE -86.3

// 角度范围限制
#define YAW_MIN -90
#define YAW_MAX 90
#define PITCH_MIN -90
#define PITCH_MAX 40

// 初始化参数
#define YAW_INIT 0.0
#define YAW_SPEED_INIT 100.0
#define PITCH_INIT 0.0
#define PITCH_SPEED_INIT 100.0

// 角度转换参数
// servo = K_yaw2srv * yaw + b_yaw2srv
extern float K_yaw2srv; // Yaw轴角度到舵机角度的转换系数
extern float b_yaw2srv; // Yaw轴角度到舵机角度的偏移量
// yaw = K_srv2yaw * servo + b_srv2yaw 
extern float K_srv2yaw; // 舵机角度到Yaw轴角度的转换系数
extern float b_srv2yaw; // 舵机角度到Yaw轴角度的偏移量
// Pit轴角度转换 (pitch)
// servo = K_pitch2srv * pitch + b_pitch2srv
extern float K_pitch2srv; // Pit轴角度到舵机角度的转换系数
extern float b_pitch2srv; // Pit轴角度到舵机角度的偏移量
// pitch = K_srv2pitch * servo + b_srv2pitch
extern float K_srv2pitch; // 舵机角度到Pit轴角度的转换系数
extern float b_srv2pitch; // 舵机角度到Pit轴角度的偏移量 

// 当前状态
extern float curSrvYaw; // 当前水平舵机角度 (单位 度)
extern float curSrvPitch; // 当前垂直舵机角度 (单位 度)
extern float nextSrvYaw; // 下一个水平舵机角度 (单位 度)
extern float nextSrvPitch; // 下一个垂直舵机角度 (单位 度)
extern float curYaw; // 当前Yaw轴角度 (单位 度)
extern float curPitch; // 当前Pit轴角度 (单位 度)
extern float nextYaw; // 下一个Yaw轴角度 (单位 度)
extern float nextPitch; // 下一个Pit轴角度 (单位 度)
extern float speedYaw; // 水平速度 
extern float speedPitch; // 垂直速度 

// 初始化
void Gimbal_Init(Usart_DataTypeDef* servoUsart);
// 生成舵机映射参数
void Gimbal_GenSrvMapParams(void);
// 重置
void Gimbal_Reset(Usart_DataTypeDef* servoUsart);
// 更新Yaw轴角度
void Gimbal_UpdateYaw(Usart_DataTypeDef* servoUsart);
// 更新Pit轴角度
void Gimbal_UpdatePitch(Usart_DataTypeDef* servoUsart);
// 更新所有角度
void Gimbal_Update(Usart_DataTypeDef* servoUsart);
// Yaw轴角度转换为舵机角度
float Gimbal_Yaw2Servo(float yaw);
// 舵机角度转换为Yaw轴角度
float Gimbal_Servo2Yaw(float servo);
// Pit轴角度转换为舵机角度
float Gimbal_Pitch2Servo(float pitch);
// 舵机角度转换为Pit轴角度
float Gimbal_Servo2Pitch(float servo);
// 设置Yaw轴角度
uint16_t Gimbal_SetYaw(Usart_DataTypeDef* servoUsart, float yaw, float speed);
// 设置Pit轴角度
uint16_t Gimbal_SetPitch(Usart_DataTypeDef* servoUsart, float pitch, float speed);
// 等待舵机到达目标位置
void Gimbal_Wait(Usart_DataTypeDef* servoUsart);
#endif
