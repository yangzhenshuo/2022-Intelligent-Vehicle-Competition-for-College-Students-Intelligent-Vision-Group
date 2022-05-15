/*
 * serve.c
 * �������
 * Author: ����˶
 */
#include "System.h"
#include "server.h"
#include "image.h"
#include "TSP.h"

rt_sem_t server1_sem;
rt_sem_t server2_sem;

/***********************************************
 * @brief ���ܳ���������Ŀ������Ѱ���滮·��
 * @param
 * @return
 * *********************************************/
void server_find_dot(void *parameter)
{
  rt_thread_mdelay(10); //�ӳٵȴ�Finsh�����ʼ��
  char test[] = "enter1\n";
  while (1)
  {
    rt_sem_take(server1_sem, RT_WAITING_FOREVER);
    // Found_dot_info();
    Scan_line();
    seekfree_wireless_send_buff((uint8 *)test, sizeof(test) - 1);
  }
}
/***********************************************************
 * @brief ·���滮�̳߳�ʼ��
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
    //    Computing_angle();
    //    //�������ýǶ�
    //    CarInfo.AngleSet = CarInfo.current_angle;
    //�ж��Ƿ���ת���
    //    if (CarInfo.current_angle <= 2)
    //    {
    find_midpoint();
    //�����е�x���������꣨���Ƶ�94��
    CarInfo.distance2 = midpoint[0] - 94; //x��ƫ��
    CarInfo.distance1 = midpoint[1] - 119;//y��ƫ��
    if (CarInfo.distance1 == 0 && CarInfo.distance2 == 0)
    {
      CarInfo.Iscorrect = 'F';
    }
    //    }
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