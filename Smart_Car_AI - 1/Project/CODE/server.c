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

/***********************************************
 * @brief ���ܳ���������Ŀ������Ѱ���滮·��
 * @param
 * @return
 * *********************************************/
void server_find_dot(void *parameter)
{
  rt_thread_mdelay(10); //�ӳٵȴ�Finsh�����ʼ��
  char test[] = "enter1\n";
	while(1)
	{
  rt_sem_take(server1_sem, RT_WAITING_FOREVER);
  // Found_dot_info();
  Scan_line();
  seekfree_wireless_send_buff((uint8 *)test, sizeof(test) - 1);
	}
}
/***********************************************************
 * @brief ��̬����
 * @param
 * @return
 ***********************************************************/
void posture_rectify(void *parameter)
{
  char test[] = "enter2\n";
  //Ѱ�Ҷ���
  Scan_line();
  seekfree_wireless_send_buff((uint8 *)test, sizeof(test) - 1);
  // find_top_angle();
}
/***********************************************************
 * @brief ·���滮�̳߳�ʼ��
 * @param
 * @return
 ***********************************************************/
void server_thread_init(void)
{
  rt_thread_t server1;
  //��������һ�ź���
  server1_sem = rt_sem_create("server1", 0, RT_IPC_FLAG_FIFO);
  server1 = rt_thread_create("server1_thread", //�߳�����
                             server_find_dot,  //�߳���ں���
                             RT_NULL,          //��ڲ���
                             1024,             //�߳�ջ
                             11,                //���ȼ�
                             30);              //ʱ��Ƭ
  if (RT_NULL != server1)
  {
    rt_thread_startup(server1);
  }
}