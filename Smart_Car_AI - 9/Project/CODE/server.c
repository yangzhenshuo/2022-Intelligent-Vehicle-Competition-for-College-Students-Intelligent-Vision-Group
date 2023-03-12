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
#include "openart_mini.h"

#define square(x1, y1, x2 ,y2) ((x2) - (x1))*((x2) - (x1))+((y2) - (y1))*((y2) - (y1));
extern rt_mailbox_t buzzer_mailbox;
/***********************************************
 * @brief ���³�ʼ����̼�
 * @param
 * @return
 * *********************************************/
static inline void renew_Carpulse()
{
	rt_enter_critical();      //�ٽ籣��
	CarInfo.RunDistance1 = 0;                    
	CarInfo.RunDistance2 = 0;
	rt_exit_critical();	
}
/***********************************************
 * @brief �滮��һ����
 * @param p Ŀ�������ָ��
 * @param pos �������λ��
 * @return ��һ���������������
 * *********************************************/
static inline int sort_dot(DotTypedef *p, coordinateTypedef pos)
{
	DotTypedef ready_dot[20]; //����׼��״̬�ĵ�
	int temp, j = 0;
	int s = 50000;            //��ʼ����
	int s1;                   //����Ŀ���ľ���
	for(int i =0; i < 20; i++)//��ȡ����׼��״̬��Ŀ���
	{
		if(p[i].flag == 1)
		{
			ready_dot[j].x = p[i].x;
			ready_dot[j].y = p[i].y;
			ready_dot[j].flag = i;
			j++;
		}
	}
	for(int i =0; i < j; i++)
	{
		s1 = square(pos.x, pos.y, ready_dot[i].x, ready_dot[i].y);
		if(s1 < s)
		{
			s = s1;
			temp = ready_dot[i].flag;
		}
	}
	return temp;
}
/***********************************************
 * @brief ���ܳ���������Ŀ������Ѱ���滮·��
 * @param
 * @return
 * *********************************************/
void server_find_dot(void *parameter)
{
  rt_thread_mdelay(10); //�ӳٵȴ�Finsh�����ʼ��
	int i, j;
  while (1)
  {
    rt_sem_take(server1_sem, RT_WAITING_FOREVER);
		renew_Carpulse();                   //���³�ʼ����̼�
		if(CarInfo.num < CarInfo.dotnum)            //û����������ͼƬ
		{
			j = sort_dot(dot, orignal_pos);            //�ҵ�������������
			dot[j].flag = 0;                           //״̬���Ϊ�ѱ�Ѱ��
			CarInfo.goal_x = dot[j].x;                 //�ѵ���һ�������긳ֵ��Ŀ��λ��
			CarInfo.goal_y = dot[j].y - 1;
		}
		if(CarInfo.num == CarInfo.dotnum)            //��Ѱ�꣬��ʼ�ع������
		{
				CarInfo.goal_x = 0;
				CarInfo.goal_y = 0;
		}
		rt_sem_release(server2_sem);
  }
}
/***********************************************************
 * @brief �˶�����
 * @param
 * @return
 ***********************************************************/
static inline void stop_run()
{
	SystemSettings.Is_arrival = 'T'; //�����־λ����
	SystemSettings.Iscorrect = 'T';  //��������
}
void run_control(void *parameter)
{
	rt_thread_mdelay(10); //�ӳٵȴ�Finsh�����ʼ��
	rt_uint32_t e;
	while(1)
	{
		rt_sem_take(server2_sem, RT_WAITING_FOREVER);
		SystemSettings.Is_arrival = 'F';		         //�رյ����־λ
		position.x = CarInfo.goal_y - orignal_pos.y; //λ�ÿ��Ƶ��趨ֵ
		position.y = CarInfo.goal_x - orignal_pos.x;
//		SystemSettings.Is_control = 'T';			
		if(CarInfo.num < CarInfo.dotnum) //Ѳ����
		{
			if(DetAbs(coordinate.x, CarInfo.goal_x * 200) < 10 && DetAbs(CarInfo.goal_y * 200, coordinate.y) < 10) //�жϵ����λ��
			{
				rt_mb_send(buzzer_mailbox, 500);
				stop_run();                 //ֹͣ���߿�ʼ����	
			}
		}
		else                                 //Ѳ����ɻع������
		{
			if(DetAbs(coordinate.x, CarInfo.goal_x * 200) == 0 && DetAbs(coordinate.y, CarInfo.goal_y * 200) == 0) //�жϻص�������
			{
				rt_mb_send(buzzer_mailbox, 500);
				stop_car();                      //ͣ��
				orignal_pos.x = CarInfo.goal_x;
				orignal_pos.y = CarInfo.goal_y;				
			}					
		}
	}
}

/***********************************************************
 * @brief �Ӿ�����
 * @param
 * @return
 ***********************************************************/
static inline void stop_correct()
{
	SystemSettings.Iscorrect = 'F';   //�ؽ���
	renew_Carpulse();
	position.x = position.y = 0;
  MotorStopped();                   //ֹͣ���	
	SystemSettings.IsAiOn = 'T';			//����ʶ��
}
static inline int status(coordinateTypedef pos, int x, int y)
{
	int s;
	if(pos.x <= x && pos.y <= y)  //Ŀ���ڳ������Ϸ�
	{
		s = 1;
	}
	if(pos.x <= x && pos.y > y)   //Ŀ���ڳ������·�
	{
		s = 2;
	}
	if(pos.x > x && pos.y <= y)  //Ŀ���ڳ������Ϸ�
	{
		s = 3;
	}
	if(pos.x > x && pos.y > y)   //Ŀ���ڳ������·�
	{
		s = 4;
	}
	return s;
}
void visual_pursit(void *parameter)
{
	rt_thread_mdelay(10); //�ӳٵȴ�Finsh�����ʼ��
	rt_uint32_t e;
	int condition = 4;
	while(1)
	{
		rt_sem_take(server3_sem, RT_WAITING_FOREVER);
		condition = status(orignal_pos, CarInfo.goal_x, CarInfo.goal_y);
		if(CarInfo.white_proportion <= 5)
		{
			switch(condition)
			{
				case 1:CarInfo.distance1 = -40;CarInfo.distance2 = 10;break;
				case 2:CarInfo.distance1 = -40;CarInfo.distance2 = 10;break;
				case 3:CarInfo.distance1 = -40;CarInfo.distance2 = -10;break;
				case 4:CarInfo.distance1 = -40;CarInfo.distance2 = -10;break;
			}
		}
		else
		{
			CarInfo.distance2 = barycentre[0] - 80;                    //x��ƫ��
			CarInfo.distance1 = 60 - barycentre[1];                     //y��ƫ��	
		}
		if (Abs(CarInfo.distance1) < 4 && Abs(CarInfo.distance2) < 4) //�����ɹ�
		{
			rt_mb_send(buzzer_mailbox, 500);
			orignal_pos.x = CarInfo.goal_x;                            //���³��ľ���λ������
			orignal_pos.y = CarInfo.goal_y;	
			stop_correct();                                            //ֹͣ����
		}				
	}
}
/***********************************************************
 * @brief ����ͼƬ
 * @param
 * @return
 ***********************************************************/
static inline int collision(int e, coordinateTypedef pos)
{
	int ready_dot[20];        //����׼��״̬�ĵ�
	int temp, j = 0;
	for(int i =0; i < 20; i++)//��ȡ����׼��״̬��Ŀ���
	{
		if(dot[i].flag == 1)
		{
			switch(e)
			{
				case 1:ready_dot[j] = dot[i].y; break;
				case 2:ready_dot[j] = dot[i].x; break;
				case 3:ready_dot[j] = dot[i].y; break;
			}
			j++;
		}
	}
	for(int i =0; i < 20; i++)//�ж��Ƿ��е�������·����
	{
		if((ready_dot[i] == pos.y && e == 1 | e==2) |(ready_dot[i] == pos.x && e == 2))
		{
			temp++;
		}
	}
	return 1;
}
void carry_image(void *parameter)
{
	rt_thread_mdelay(10); //�ӳٵȴ�Finsh�����ʼ��
	rt_uint32_t e;
  while (1)
  {
    //�ȴ������¼��Ľ���
    if(rt_event_recv(run_event, (1|2|3),
           RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
           RT_WAITING_FOREVER, &e) == RT_EOK)
     {
			 //���¶���ڱ�
			 smotor_down();
			 //����Ƿ�����
			 rt_thread_mdelay(500);
			 //�������ͨ��
			 gpio_set(DIAN_PIN, 1);		 
			 //������ͼƬ
			 smotor_up();
			 rt_thread_mdelay(500);
			 switch(e)
			 {
				 case 1: CarInfo.goal_x = -1; CarInfo.goal_y = orignal_pos.y;  position.x = 0; position.y = CarInfo.goal_x - orignal_pos.x; break;  //�����ڵ�ͼ�����
				 case 2: CarInfo.goal_x = orignal_pos.x; CarInfo.goal_y = 19; position.x = CarInfo.goal_y - orignal_pos.y; position.y = 0; break;  //��ͨ�����ڵ�ͼ���ϱ�
				 case 3: CarInfo.goal_x = 21; CarInfo.goal_y = orignal_pos.y; position.x = 0; position.y = CarInfo.goal_x - orignal_pos.x; break;  //ˮ���ڵ�ͼ���ұ�
			 } 
			 //��̼Ƴ�ʼ��
			 renew_Carpulse();                
			 //�������ƺͰ���
			 SystemSettings.IsCarry = 'T';
		 }
		 rt_sem_take(server4_sem, RT_WAITING_FOREVER);
		 //����ͼƬ
		 switch(DetectResult)
		 {	
			 case 1:smotor_down1(); rt_thread_mdelay(300); gpio_set(DIAN_PIN, 0); smotor_up();rt_thread_mdelay(500);break;
			 case 2:gpio_set(DIAN_PIN, 0);break;
			 case 3:smotor_down1(); rt_thread_mdelay(300); gpio_set(DIAN_PIN, 0); smotor_up();rt_thread_mdelay(500);break;
		 }
		 //��ʼ��һ��Ѳ��
		 SystemSettings.IsFound_dot = 'T';
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
  
  server1 = rt_thread_create("server1_thread", //�߳�����
                             server_find_dot,  //�߳���ں���
                             RT_NULL,          //��ڲ���
                             1024,             //�߳�ջ
                             11,               //���ȼ�
                             10);              //ʱ��Ƭ
  if (RT_NULL != server1)
  {
    rt_thread_startup(server1);
  }
}
/***********************************************************
 * @brief	�˶��̳߳�ʼ��
 * @param
 * @return
 ***********************************************************/
void server2_thread_init(void)
{
  rt_thread_t server2;
  
  server2 = rt_thread_create("server2_thread", //�߳�����
                             run_control,      //�߳���ں���
                             RT_NULL,          //��ڲ���
                             1024,             //�߳�ջ
                             11,               //���ȼ�
                             10);              //ʱ��Ƭ
  if (RT_NULL != server2)
  {
    rt_thread_startup(server2);
  }
}
/***********************************************************
 * @brief �Ӿ�׷���̳߳�ʼ��
 * @param
 * @return
 ***********************************************************/
void server3_thread_init(void)
{
  rt_thread_t server3;
  
  server3 = rt_thread_create("server3_thread", //�߳�����
                             carry_image,      //�߳���ں���
                             RT_NULL,          //��ڲ���
                             1024,             //�߳�ջ
                             11,               //���ȼ�
                             10);              //ʱ��Ƭ
  if (RT_NULL != server3)
  {
    rt_thread_startup(server3);
  }
}
/***********************************************************
 * @brief �����̳߳�ʼ��
 * @param
 * @return
 ***********************************************************/
void server4_thread_init(void)
{
  rt_thread_t server4;
  
  server4 = rt_thread_create("server4_thread", //�߳�����
                             visual_pursit,      //�߳���ں���
                             RT_NULL,          //��ڲ���
                             1024,             //�߳�ջ
                             11,               //���ȼ�
                             10);              //ʱ��Ƭ
  if (RT_NULL != server4)
  {
    rt_thread_startup(server4);
  }
}
