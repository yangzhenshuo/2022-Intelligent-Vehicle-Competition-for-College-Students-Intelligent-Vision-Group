#include "buzzer.h"
#define BUZZER_PIN B11 // ���������Ϸ�������Ӧ����

rt_mailbox_t buzzer_mailbox;

/***********************************************************
 * @brief �������̵߳���ں���
 * @param
 * @return
 ***********************************************************/
void buzzer_entry(void *parameter)
{
    uint32 mb_data;
    rt_thread_mdelay(10); // ��ʱ�ȴ�finsh��ʼ�����
    while (1)
    {
        //�����������ݣ����û������������ȴ����ͷ�CPU����Ȩ
        rt_mb_recv(buzzer_mailbox, &mb_data, RT_WAITING_FOREVER);
        gpio_set(BUZZER_PIN, 1);   //�򿪷�����
        rt_thread_mdelay(mb_data); //��ʱ
        gpio_set(BUZZER_PIN, 0);   //�رշ�����
    }
}
/***********************************************************
 * @brief �������̳߳�ʼ��
 * @param
 * @return
 ***********************************************************/
void buzzer_thread_init(void)
{
    rt_thread_t tid;

    //��ʼ����������ʹ�õ�GPIO
    gpio_init(BUZZER_PIN, GPO, 0, GPIO_PIN_CONFIG); // ��ʼ��ΪGPIO�������� Ĭ�������ߵ�ƽ

    //��������
    buzzer_mailbox = rt_mb_create("buzzer", 5, RT_IPC_FLAG_FIFO);

    //�������������߳�
    tid = rt_thread_create("buzzer",     //�߳�����
                           buzzer_entry, //�߳���ں���
                           RT_NULL,      //�߳���ڲ���
                           512,          //�߳�ջ��С1024�ֽ�
                           13,           //�߳����ȼ�
                           2);           //�߳�ʱ��Ƭ��ͬ���ȼ��߳�������

    //�����߳�
    if (RT_NULL != tid)
    {
        rt_thread_startup(tid);
    }
}