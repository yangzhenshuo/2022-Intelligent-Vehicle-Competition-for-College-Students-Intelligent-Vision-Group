
#include "buzzer.h"
#include "button.h"
#include "System.h"
#include "image.h"
#include "openart_mini.h"

#define KEY_1 C31 // 定义主板上按键对应引脚
#define KEY_2 C27 // 定义主板上按键对应引脚
#define KEY_3 C26 // 定义主板上按键对应引脚
#define KEY_4 C4  // 定义主板上按键对应引脚

extern float Threshold;
//开关状态变量
uint8 key1_status = 1;
uint8 key2_status = 1;
uint8 key3_status = 1;
uint8 key4_status = 1;

//上一次开关状态变量
uint8 key1_last_status;
uint8 key2_last_status;
uint8 key3_last_status;
uint8 key4_last_status;

void button_entry(void *parameter)
{
  // rt_thread_mdelay(10);										// 延时等待finsh初始化完毕
  //保存按键状态
  key1_last_status = key1_status;
  key2_last_status = key2_status;
  key3_last_status = key3_status;
  key4_last_status = key4_status;

  //读取当前按键状态
  key1_status = gpio_get(KEY_1);
  key2_status = gpio_get(KEY_2);
  key3_status = gpio_get(KEY_3);
  key4_status = gpio_get(KEY_4);

  //检测到按键按下之后并放开 释放一次信号量
  if (key1_status && !key1_last_status)
  {
    rt_sem_release(key1_sem);
    rt_mb_send(buzzer_mailbox, 100);
  }
  if (key2_status && !key2_last_status)
  {
    rt_sem_release(key2_sem);
    SystemSettings.IsFound_dot = 'T';
    rt_mb_send(buzzer_mailbox, 100);
  }
  if (key3_status && !key3_last_status)
  {
    rt_sem_release(key3_sem);
		uart_putchar(USART_4, 0x01);
    rt_mb_send(buzzer_mailbox, 100);
  }
  if (key4_status && !key4_last_status)
  {
    rt_sem_release(key4_sem);
    rt_mb_send(buzzer_mailbox, 100);
  }
}
void button_init(void)
{
  rt_timer_t timer1;

  gpio_init(KEY_1, GPI, GPIO_HIGH, GPIO_PIN_CONFIG); // 初始化为GPIO浮空输入 默认上拉高电平
  gpio_init(KEY_2, GPI, GPIO_HIGH, GPIO_PIN_CONFIG);
  gpio_init(KEY_3, GPI, GPIO_HIGH, GPIO_PIN_CONFIG);
  gpio_init(KEY_4, GPI, GPIO_HIGH, GPIO_PIN_CONFIG);

  timer1 = rt_timer_create("button",                //定时器名称
                           button_entry,            //超时函数
                           RT_NULL,                 //入口参数0
                           20,                      //超时时间20个OStick
                           RT_TIMER_FLAG_PERIODIC); //周期性

  if (RT_NULL != timer1)
  {
    rt_timer_start(timer1);
  }
}