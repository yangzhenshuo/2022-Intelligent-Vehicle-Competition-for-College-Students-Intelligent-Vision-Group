/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2019,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		IAR 8.3 or MDK 5.28
 * @Target core		NXP RT1064DVL6A
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2019-04-30
 ********************************************************************************************************************/

//整套推荐IO查看Projecct文件夹下的TXT文本

//打开新的工程或者工程移动了位置务必执行以下操作
//第一步 关闭上面所有打开的文件
//第二步 project  clean  等待下方进度条走完

//下载代码前请根据自己使用的下载器在工程里设置下载器为自己所使用的

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

rt_sem_t camera_sem; //摄像头信号量
INIT_APP_EXPORT(communication_init);
INIT_APP_EXPORT(thread_init);
INIT_APP_EXPORT(hardware_init);
INIT_APP_EXPORT(timer_init);

char test0[] = "server1\n";
int flag=0;
int main(void)
{
  //创建信号量
  camera_sem = rt_sem_create("camera", 0, RT_IPC_FLAG_FIFO);
  //初始化pit定时器用来计时
  pit_init();
  //车信息初始化
  CarInformation_init();
  //全局中断使能
  EnableGlobalIRQ(0);
  while (1)
  { 		
    rt_sem_take(camera_sem, RT_WAITING_FOREVER); //等待摄像头采集完毕（18ms）采集一副图片
    if (SystemSettings.Binary_start == 'T')      //阈值更新
    {
      Binary_renew(CarInfo.BinaryMethod);
      SystemSettings.Binary_start = 'F';
    }
    Binary_image();    		//二值化图像		
    if (SystemSettings.IsFound_dot == 'T') //开始第一个服务程序（寻点）
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
			//寻找图片的质心
			find_barycen(BinaryImage);
			rt_sem_release(server3_sem);
		}
		if (SystemSettings.IsAiOn == 'T')
		{
			//发送识别信号
			SendCommand(3);
			SystemSettings.IsAiOn = 'F';
			CarInfo.num++;
		}
//**转角方案**//
		if (SystemSettings.IsCarry == 'T' )
		{
			 if(DetAbs(CarInfo.AngleSet, CarInfo.yaw)<=1) //判断转到对应角度
			 {
				 position.x = CarInfo.s;
				 if(DetAbs(coordinate.y, CarInfo.s * 200) < 15 ) //判断到达点位置
			   {
			     rt_mb_send(buzzer_mailbox, 500);
			     SystemSettings.IsCarry = 'F';   //关闭搬运标志位
			     MotorStopped();
			     orignal_pos.x = CarInfo.goal_x;
			     orignal_pos.y = CarInfo.goal_y;
				   rt_sem_release(server4_sem);	  //发送放下图片信号量
		      }
				}
			}
  }
}
