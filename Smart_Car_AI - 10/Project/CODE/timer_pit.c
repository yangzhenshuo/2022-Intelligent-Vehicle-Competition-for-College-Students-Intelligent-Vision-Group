/*
 * timer_pit.c
 * 软件定时器
 * Author: 杨镇硕
 */
#include "timer_pit.h"
#include "control.h"
#include "System.h"
#include "communicate.h"
#include "ProjectMath.h"
#include "buzzer.h"

rt_timer_t timer1, timer2;

void timer1_pit_entry(void *parameter)
{
  // rt_thread_mdelay(10);										// 延时等待finsh初始化完毕
  static uint32 time;
  time++;

  if (0 == (time % 5) && SystemSettings.Is_control == 'T')
  {
    //速度控制（控制周期5ms		
		SpeedControl();
  }
  if (0 == (time % 20))
  {
    //位置控制与角度控制（控制周期20ms）
		if(SystemSettings.Is_control == 'T')
		{
			AngleControl();
		}
		if(SystemSettings.Is_control == 'T')
		{
			if(SystemSettings.Iscorrect == 'F') 
				PositionControl();
			else 
			{
				CorrectControl();
			}
		}			
  }
	if (0 == time % 500)
	{
		//Report_info(); //向上位机发送信息
		Report();
		time = 0;
	}
}

void timer2_pit_entry(void *parameter)
{
	static uint32 time;
	time++;
	//0.5s判断一次是否旋转完成
	if(0 == time % 5)
	{
		time = 0;
	}
	if(0 == time % 2)
	{
		
	}
}
void timer1_pit_init(void)
{
  //创建定时器1 周期运行
  timer1 = rt_timer_create("timer1",                //定时器名称
                           timer1_pit_entry,        //超时函数入口指针
                           RT_NULL,                 //入口参数
                           1,                       //超时时间为0.001s
                           RT_TIMER_FLAG_PERIODIC); //周期性

  //启动定时器
  if (RT_NULL != timer1)
  {
    rt_timer_start(timer1);
  }
}
void timer2_pit_init(void)
{
	timer2 =	rt_timer_create("timer2",                //定时器名称
                           timer2_pit_entry,        //超时函数入口指针
                           RT_NULL,                 //入口参数
                           50,                       //超时时间为0.05s
                           RT_TIMER_FLAG_PERIODIC); //周期性
	if (RT_NULL != timer2)
  {
    rt_timer_start(timer2);
  }
	
}

