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
#include "TSP.h"
#include "communicate.h"
//#include "timer_pit.h"
//#include "encoder.h"
//#include "buzzer.h"
//#include "button.h"
//#include "motor.h"
//#include "openart_mini.h"
//#include "smotor.h"

rt_sem_t camera_sem;
INIT_APP_EXPORT(software_init);
INIT_APP_EXPORT(hardware_init);
int main(void)
{
  //��������ͷ�ź���
  camera_sem = rt_sem_create("camera", 0, RT_IPC_FLAG_FIFO);
  uint32 use_time;
  //��ʼ��pit��ʱ��������ʱ
  pit_init();
  //����Ϣ��ʼ��
  CarInformation_init();
  //ȫ���ж�ʹ��
  EnableGlobalIRQ(0);
  while (1)
  {
    //�ȴ�����ͷ�ɼ����
    rt_sem_take(camera_sem, RT_WAITING_FOREVER);
    //��ʼ��������ͷͼ��
    //rt_kprintf("Start process\n");
    pit_start(PIT_CH0);
    Binary_image();
		
    use_time = pit_get_ms(PIT_CH0);
    pit_close(PIT_CH0);

    //����ͼ��������ģ��ͼ��֮��ĽǶ�
		Computing_angle(); 
		ReportImageStatus();
    //����ƫ�����PD����

    // PD����֮���ֵ����Ѱ������Ŀ���

    // �����ٽ��� �����ӡ��ʱ�������̴߳��
    rt_kprintf("main_end:%d\n", use_time);
	  rt_thread_mdelay(10);
  }
}
