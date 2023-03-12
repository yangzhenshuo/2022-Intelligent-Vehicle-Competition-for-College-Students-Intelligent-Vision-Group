/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2019,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		main
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ3184284598)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		IAR 8.3 or MDK 5.28
 * @Target core		NXP RT1064DVL6A
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2019-04-30
 ********************************************************************************************************************/

//�����Ƽ�IO�鿴Projecct�ļ����µ�TXT�ı�

//���µĹ��̻��߹����ƶ���λ�����ִ�����²���
//��һ�� �ر��������д򿪵��ļ�
//�ڶ��� project  clean  �ȴ��·�����������

//���ش���ǰ������Լ�ʹ�õ��������ڹ���������������Ϊ�Լ���ʹ�õ�

#include "headfile.h"

#include "System.h"
#include "image.h"
#include "smotor.h"
#include "openart_mini.h"
#include "motor.h"
//#include "communicate.h"
#include "ProjectMath.h"
//#include "timer_pit.h"
//#include "encoder.h"
#include "buzzer.h"
//#include "button.h"

//#include "server.h"

rt_sem_t camera_sem; //����ͷ�ź���
INIT_APP_EXPORT(communication_init);
INIT_APP_EXPORT(thread_init);
INIT_APP_EXPORT(hardware_init);
INIT_APP_EXPORT(timer_init);

char test0[] = "server1\n";
int flag=0;
int main(void)
{
  //�����ź���
  camera_sem = rt_sem_create("camera", 0, RT_IPC_FLAG_FIFO);
  //��ʼ��pit��ʱ��������ʱ
  pit_init();
  //����Ϣ��ʼ��
  CarInformation_init();
  //ȫ���ж�ʹ��
  EnableGlobalIRQ(0);
  while (1)
  { 		
    rt_sem_take(camera_sem, RT_WAITING_FOREVER); //�ȴ�����ͷ�ɼ���ϣ�18ms���ɼ�һ��ͼƬ
    if (SystemSettings.Binary_start == 'T')      //��ֵ����
    {
      Binary_renew(CarInfo.BinaryMethod);
      SystemSettings.Binary_start = 'F';
    }
    Binary_image();    		//��ֵ��ͼ��		
    if (SystemSettings.IsFound_dot == 'T') //��ʼ��һ���������Ѱ�㣩
    {
      rt_sem_release(server1_sem);
      SystemSettings.IsFound_dot = 'F';			
    }	
		
		if (SystemSettings.Is_arrival == 'F')
		{
			rt_sem_release(server2_sem);
		}
		if (SystemSettings.Iscorrect == 'T')
		{
			//Ѱ��ͼƬ������
			find_barycen(BinaryImage);
			rt_sem_release(server3_sem);
		}
		if (SystemSettings.IsAiOn == 'T')
		{
			//����ʶ���ź�
			SendCommand(3);
			SystemSettings.IsAiOn = 'F';
			CarInfo.num++;
		}
//**ת�Ƿ���**//
		if (SystemSettings.IsCarry == 'T' )
		{
			 if(DetAbs(CarInfo.AngleSet, CarInfo.yaw)<=1) //�ж�ת����Ӧ�Ƕ�
			 {
				 position.x = CarInfo.s;
				 if(DetAbs(coordinate.y, CarInfo.s * 200) < 15 ) //�жϵ����λ��
			   {
			     rt_mb_send(buzzer_mailbox, 500);
			     SystemSettings.IsCarry = 'F';   //�رհ��˱�־λ
			     MotorStopped();
			     orignal_pos.x = CarInfo.goal_x;
			     orignal_pos.y = CarInfo.goal_y;
				   rt_sem_release(server4_sem);	  //���ͷ���ͼƬ�ź���
		      }
				}
			}
  }
}
