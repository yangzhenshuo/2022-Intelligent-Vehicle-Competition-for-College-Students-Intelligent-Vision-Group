#include "headfile.h"
#include "System.h"
#include "display.h"
#include "encoder.h"
/***********************************************************
 * @brief ��ʾ�߳���ں���
 * @param
 * @return
***********************************************************/
void display_entry(void *parameter)
{
	  //ips114_clear(WHITE);
	  rt_thread_mdelay(10);										// ��ʱ�ȴ�finsh��ʼ�����
    while(1)
    {
      ips114_displayimage032_zoom(BinaryImage[0], MT9V03X_CSI_W, MT9V03X_CSI_H, 240, 135);//��С��ʾ�������ͬ����ʾһЩ����
			rt_thread_mdelay(1000);
    }
    
}
/***********************************************************
 * @brief ��Ļ��ʼ��
 * @param
 * @return
***********************************************************/
void display_thread_init(void)
{
    rt_thread_t tid;
    
    //��ʼ����Ļ
    ips114_init();
	  ips114_showstr(0,0,"Initializing... ");
	  ips114_showstr(0,0,"      OK...     ");
    //������ʾ�߳� ���ȼ�����Ϊ31
    tid = rt_thread_create("display", //�߳�����
														display_entry, //�߳���ں���
														RT_NULL, //��ڲ���
														1024, //�߳�ջ
														31, //���ȼ�
														30);//ʱ��Ƭ
    
    //������ʾ�߳�
    if(RT_NULL != tid)
    {
        rt_thread_startup(tid);
    }
}