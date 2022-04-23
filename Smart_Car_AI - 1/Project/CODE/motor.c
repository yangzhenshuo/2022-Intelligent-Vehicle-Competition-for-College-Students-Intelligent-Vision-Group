#include "motor.h"

#define f_DIR_L				D0
#define f_DIR_R				D1
#define r_DIR_L       D14
#define r_DIR_R       D15

#define f_PWM_L				PWM2_MODULE3_CHA_D2
#define f_PWM_R				PWM2_MODULE3_CHB_D3
#define r_PWM_L       PWM1_MODULE0_CHA_D12
#define r_PWM_R       PWM1_MODULE0_CHB_D13
/***********************************************************
 * @brief ����������ų�ʼ��
 * @param
 * @return
***********************************************************/
void motor_init(void)
{
	gpio_init(f_DIR_L, GPO, GPIO_HIGH, GPIO_PIN_CONFIG);
	gpio_init(f_DIR_R, GPO, GPIO_HIGH, GPIO_PIN_CONFIG);
	gpio_init(r_DIR_L, GPO, GPIO_HIGH, GPIO_PIN_CONFIG);
	gpio_init(r_DIR_R, GPO, GPIO_HIGH, GPIO_PIN_CONFIG);

	pwm_init(f_PWM_L, 17000, 0);												// 
	pwm_init(f_PWM_R, 17000, 0);												// 
	pwm_init(r_PWM_L, 17000, 0);												// 
	pwm_init(r_PWM_R, 17000, 0);												// ��ʼ��Ƶ��17KHz ռ�ձȳ�ʼΪ0
}

/***********************************************************
 * @brief ˫���ת��,��������
 * @param LeftMotorPwm ����pwm
 * @param RightMotorPwm ����pwm
 * @return
***********************************************************/
void motor_control(int32 f_duty_l, int32 f_duty_r, int32 r_duty_l, int32 r_duty_r)
{
    //��ռ�ձ��޷�
	f_duty_l = limit(f_duty_l, PWM_DUTY_MAX);
	f_duty_r = limit(f_duty_r, PWM_DUTY_MAX);
	r_duty_l = limit(r_duty_l, PWM_DUTY_MAX);
	r_duty_r = limit(r_duty_r, PWM_DUTY_MAX);
    
    if(r_duty_l >= 0)											// �����ת
    {
        gpio_set(r_DIR_L, GPIO_HIGH);							// DIR����ߵ�ƽ
        pwm_duty(r_PWM_L, r_duty_l);						    // ����ռ�ձ�
    }
    else													// ��෴ת
    {
        gpio_set(r_DIR_L, GPIO_LOW);							// DIR����͵�ƽ
        pwm_duty(r_PWM_L, -r_duty_l);							// ����ռ�ձ�
    }
    
    if(r_duty_r >= 0)											// �Ҳ���ת
    {
        gpio_set(r_DIR_R, GPIO_HIGH);							// DIR����ߵ�ƽ
        pwm_duty(r_PWM_R, r_duty_r);							// ����ռ�ձ�
    }
    else													// �Ҳ෴ת
    {
        gpio_set(r_DIR_R, GPIO_LOW);							// DIR����͵�ƽ
        pwm_duty(r_PWM_R, -r_duty_r);							// ����ռ�ձ�
    }
    
    if(f_duty_l >= 0)											// �����ת
    {
        gpio_set(f_DIR_L, GPIO_HIGH);							// DIR����ߵ�ƽ
        pwm_duty(f_PWM_L, f_duty_l);						    // ����ռ�ձ�
    }
    else													// ��෴ת
    {
        gpio_set(f_DIR_L, GPIO_LOW);							// DIR����͵�ƽ
        pwm_duty(f_PWM_L, -f_duty_l);							// ����ռ�ձ�
    }
    
    if(f_duty_r >= 0)											// �Ҳ���ת
    {
        gpio_set(f_DIR_R, GPIO_HIGH);							// DIR����ߵ�ƽ
        pwm_duty(f_PWM_R, f_duty_r);							// ����ռ�ձ�
    }
    else													// �Ҳ෴ת
    {
        gpio_set(f_DIR_R, GPIO_LOW);							// DIR����͵�ƽ
        pwm_duty(f_PWM_R, -f_duty_r);							// ����ռ�ձ�
    }
}
/***********************************************************
 * @brief ˫���ͣת
 * @param
 * @return
***********************************************************/
void MotorStopped(void)
{
    //ǰ��ͣת
    pwm_duty(f_PWM_L, 0); 
    pwm_duty(f_PWM_R, 0); 
    //����ͣת
    pwm_duty(r_PWM_L, 0); 
    pwm_duty(r_PWM_R, 0);
}