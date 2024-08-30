/*
 * 2DoF�����̨����
 * UpdateTime: 2019/12/13
 */
 
#ifndef __GIMBAL_H
#define __GIMBAL_H

#include "stm32f10x.h"
#include "usart.h"
#include "fashion_star_uart_servo.h"
#include "sys_tick.h"

// ��̨�������������
#define SERVO_ID_YAW 0 // ƫ���ǵĶ��ID
#define SERVO_ID_PITCH 1 // �����ǵĶ��ID
#define SERVO_DEAD_BLOCK 2.0 // �������

// ?????????
#define YAW1 90.0
#define YAW1_SERVO_ANGLE 90.4
#define YAW2 -90.0
#define YAW2_SERVO_ANGLE -90

#define PITCH1 0.0
#define PITCH1_SERVO_ANGLE -2.4
#define PITCH2 -90.0
#define PITCH2_SERVO_ANGLE -92.6

// ��̨�ĽǶȷ�Χ
#define YAW_MIN -90
#define YAW_MAX 90
#define PITCH_MIN -90
#define PITCH_MAX 60

// ��̨�ĳ�ʼλ��
#define YAW_INIT 0.0
#define YAW_SPEED_INIT 100.0
#define PITCH_INIT -45.0
#define PITCH_SPEED_INIT 100.0

// ƫ���������Ƕ� ����ӳ��
// servo = K_yaw2srv * yaw + b_yaw2srv
extern float K_yaw2srv; // ƫ����ת��Ϊ����Ƕȵı���ϵ��
extern float b_yaw2srv; // ƫ����ת��Ϊ����Ƕȵ�ƫ����
// yaw = K_srv2yaw * servo + b_srv2yaw 
extern float K_srv2yaw; // ����Ƕ�ת��Ϊƫ���ǵı���ϵ��
extern float b_srv2yaw; // ����Ƕ�ת��Ϊƫ���ǵ�ƫ����
// �����������Ƕ� (����ӳ��)
// servo = K_pitch2srv * pitch + b_yaw2srv
extern float K_pitch2srv; // ������ת��Ϊ����Ƕȵı���ϵ��
extern float b_pitch2srv; // ������ת��Ϊ����Ƕȵ�ƫ����
// pitch = K_srv2pitch * servo + b_srv2pitch
extern float K_srv2pitch; // ����Ƕ�ת��Ϊ�����ǵı���ϵ��
extern float b_srv2pitch; // ����Ƕ�ת��Ϊ�����ǵ�ƫ���� 

// ��̨λ��״̬
extern float curSrvYaw; // ƫ���ǵ�ԭʼ�����ǰ�Ƕ� (��λ ��)
extern float curSrvPitch; // �����ǵ�ԭʼ�����ǰ�Ƕ� (��λ ��)
extern float nextSrvYaw; // ƫ���ǵ�ԭʼ���Ŀ��Ƕ� (��λ ��)
extern float nextSrvPitch; // �����ǵ�ԭʼ���Ŀ��Ƕ� (��λ ��)
extern float curYaw; // ��̨��ǰ��ƫ���� (��λ ��)
extern float curPitch; // ��̨��ǰ�ĸ����� (��λ ��)
extern float nextYaw; // ��̨Ŀ��ĸ����� (��λ ��)
extern float nextPitch; // ��̨Ŀ���ƫ���� (��λ ��)
extern float speedYaw; // ƫ����ת�� (��λ ��/s)
extern float speedPitch; // ������ת�� (��λ ��/s)


// ��̨��ʼ��
void Gimbal_Init(Usart_DataTypeDef* servoUsart);
// ���ö����̨
// ������̨Ϊ����ģʽ
void Gimbal_Damping(Usart_DataTypeDef* servoUsart);
// ���ɶ���Ƕ�ӳ����ص�ϵ��
void Gimbal_GenSrvMapParams(void);
// ����ƫ����
void Gimbal_UpdateYaw(Usart_DataTypeDef* servoUsart);
// ���¸�����
void Gimbal_UpdatePitch(Usart_DataTypeDef* servoUsart);
// ���¶����̨λ��
void Gimbal_Update(Usart_DataTypeDef* servoUsart);
// ƫ����ת��Ϊ����Ƕ�
float Gimbal_Yaw2Servo(float yaw);
// ����Ƕ�ת��Ϊƫ����
float Gimbal_Servo2Yaw(float servo);
// ������ת��Ϊ����Ƕ�
float Gimbal_Pitch2Servo(float pitch);
// ����Ƕ�ת��Ϊ������
float Gimbal_Servo2Pitch(float servo);
// ������̨��ƫ����
uint16_t Gimbal_SetYaw(Usart_DataTypeDef* servoUsart, float yaw, float speed);
// ������̨�ĸ�����
uint16_t Gimbal_SetPitch(Usart_DataTypeDef* servoUsart, float pitch, float speed);
// �ȴ������ת���ض���λ��
void Gimbal_Wait(Usart_DataTypeDef* servoUsart);
#endif
