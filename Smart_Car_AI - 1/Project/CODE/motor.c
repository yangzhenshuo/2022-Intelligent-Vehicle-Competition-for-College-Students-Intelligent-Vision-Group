#include "motor.h"

#define DIR_L				D0
#define DIR_R				D2

#define PWM_L				PWM1_MODULE3_CHB_D1
#define PWM_R				PWM2_MODULE3_CHB_D3
/***********************************************************
 * @brief ����������ų�ʼ��
 * @param
 * @return
***********************************************************/
void motor_init(void)
{
	gpio_init(DIR_L, GPO, GPIO_HIGH, GPIO_PIN_CONFIG);
	gpio_init(DIR_R, GPO, GPIO_HIGH, GPIO_PIN_CONFIG);

	pwm_init(PWM_L, 10000, 0);												// PWM ͨ��2 ��ʼ��Ƶ��10KHz ռ�ձȳ�ʼΪ0
	pwm_init(PWM_R, 10000, 0);												// PWM ͨ��4 ��ʼ��Ƶ��10KHz ռ�ձȳ�ʼΪ0
}

/***********************************************************
 * @brief ˫���ת��,��������
 * @param LeftMotorPwm ����pwm
 * @param RightMotorPwm ����pwm
 * @return
***********************************************************/
void SetMotorPwmAndDir(int32 LeftMotorPwm, int32 RightMotorPwm)
{
    if (LeftMotorPwm >= 0) //������ת
    {
        pwm_duty(PWM_L, 0);                    //����PWM���
        pwm_duty(PWM_R, (uint32)LeftMotorPwm); //����PWM���
    }

    else //���ַ�ת
    {
        pwm_duty(PWM_L, -(uint32)LeftMotorPwm); //����PWM���
        pwm_duty(PWM_R,0);                     //����PWM���
    }
    if (RightMotorPwm >= 0) //������ת
    {
        pwm_duty(PWM_L, 0);                     //����PWM���
        pwm_duty(PWM_R, (uint32)RightMotorPwm); //����PWM���
    }
    else //���ַ�ת
    {
        pwm_duty(PWM_L, -(uint32)RightMotorPwm); //����PWM���
        pwm_duty(PWM_R, 0);                      //����PWM���
    }
}