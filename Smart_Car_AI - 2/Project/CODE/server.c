/*
 * serve.c
 * �������
 * Author: ����˶
 */
#include "System.h"
#include "server.h"
#include "image.h"
#include "TSP.h"
#include "motor.h"
#include "ProjectMath.h"

rt_sem_t server1_sem;
rt_sem_t server2_sem;
extern rt_mailbox_t buzzer_mailbox;
/***********************************************
 * @brief ���ܳ���������Ŀ������Ѱ���滮·��
 * @param
 * @return
 * *********************************************/
void server_find_dot(void *parameter)
{
  rt_thread_mdelay(10); //�ӳٵȴ�Finsh�����ʼ��
  char test[] = "enter1\n";
	int n;
	int i;
	double s;
  while (1)
  {
    rt_sem_take(server1_sem, RT_WAITING_FOREVER);
		for(i=0; i < CarInfo.dotnum; i++)
		{
			if(dot[i].flag == 1)
			{
				if((dot[i].x-1-coordinate.x)==0)
				{
					CarInfo.AngleSet=0;
				}
				else 
				{
					if(atan2((dot[i].y-coordinate.y),(dot[i].x-coordinate.x))*180/PI<=-90&&atan2((dot[i].y-coordinate.y),(dot[i].x-coordinate.x))*180/PI>=-180)
				  {
					   CarInfo.AngleSet=atan2((dot[i].y-coordinate.y),(dot[i].x-coordinate.x))*180/PI+270;
					}
					else CarInfo.AngleSet=atan2((dot[i].y-coordinate.y),(dot[i].x-coordinate.x))*180/PI-90;
				}
				while(CarInfo.yaw!=CarInfo.AngleSet){}
				position.x=sqrt((dot[i].x-1-coordinate.x)*(dot[i].x-1-coordinate.x)+(dot[i].y-coordinate.y)*(dot[i].y-coordinate.y));
				s=(CarInfo.RunDistance1 / 210) * 5.5 * PI;
				while(s<position.x){}
				//�ҵ���һ����ر�Ѳ���־
				SystemSettings.IsFound_dot = 'F';
				dot[i].flag =0;
				n++;
				coordinate.x=dot[i].x-1;
				coordinate.y=dot[i].y;
				break;
			}
		}
		if(n == CarInfo.dotnum)
		{
			//����ͼƬʶ����ɰ�ԭ��λ�ø��趨ֵ
			 if((1-coordinate.x)==0)
				{
					CarInfo.AngleSet=0;
				}
				else 
				{
					if(atan2((1-coordinate.y),(1-coordinate.x))*180/PI<=-90&&atan2((1-coordinate.y),(1-coordinate.x))*180/PI>=-180)
				  {
					   CarInfo.AngleSet=atan2((1-coordinate.y),(1-coordinate.x))*180/PI+270;
					}
					else CarInfo.AngleSet=atan2((1-coordinate.y),(1-coordinate.x))*180/PI-90;
				}
				while(CarInfo.yaw!=CarInfo.AngleSet){}
				position.x=sqrt((1-coordinate.x)*(1-coordinate.x)+(1-coordinate.y)*(1-coordinate.y));
				while(s<position.x){}
				MotorStopped();
				SystemSettings.IsControl = 'F';                    
			  SystemSettings.IsFound_dot = 'F';
		}
    seekfree_wireless_send_buff((uint8 *)test, sizeof(test) - 1);
  }
}
/***********************************************************
 * @brief Ѳ���߳�
 * @param
 * @return
 ***********************************************************/
void server1_thread_init(void)
{
  rt_thread_t server1;
  //��������һ�ź���
  server1_sem = rt_sem_create("server1", 0, RT_IPC_FLAG_FIFO);
  server1 = rt_thread_create("server1_thread", //�߳�����
                             server_find_dot,  //�߳���ں���
                             RT_NULL,          //��ڲ���
                             1024,             //�߳�ջ
                             11,               //���ȼ�
                             30);              //ʱ��Ƭ
  if (RT_NULL != server1)
  {
    rt_thread_startup(server1);
  }
}
/***********************************************************
 * @brief ��̬����
 * @param
 * @return
 ***********************************************************/
void pos_rectify(void *parameter)
{
  char test[] = "rectify\n";
  //Ѱ�Ҷ���
  while (1)
  {
    rt_sem_take(server2_sem, RT_WAITING_FOREVER);
    Computing_angle();
        //�������ýǶ�
    CarInfo.angle = CarInfo.current_angle;
    //�ж��Ƿ���ת���
    if (CarInfo.current_angle <= 2)
     {
    find_midpoint();
    //�����е�x���������꣨���Ƶ�94��
    CarInfo.distance2 = 94-midpoint[0]; //x��ƫ��
    CarInfo.distance1 = midpoint[1] - 117;//y��ƫ��
    if (CarInfo.distance1<3 && CarInfo.distance1>-3 && CarInfo.distance2 == 0)
    {
      CarInfo.Iscorrect = 'F';
			MotorStopped();
			SystemSettings.IsControl = 'F';            
			rt_mb_send(buzzer_mailbox, 100);
    }
    }
    //seekfree_wireless_send_buff((uint8 *)test, sizeof(test) - 1);
  }
}
/***********************************************************
 * @brief �����̳߳�ʼ��
 * @param
 * @return
 ***********************************************************/
void server2_thread_init(void)
{
  rt_thread_t server2;
  //��������һ�ź���
  server2_sem = rt_sem_create("server2", 0, RT_IPC_FLAG_FIFO);
  server2 = rt_thread_create("server2_thread", //�߳�����
                             pos_rectify,      //�߳���ں���
                             RT_NULL,          //��ڲ���
                             1024,             //�߳�ջ
                             11,               //���ȼ�
                             30);              //ʱ��Ƭ
  if (RT_NULL != server2)
  {
    rt_thread_startup(server2);
  }
}