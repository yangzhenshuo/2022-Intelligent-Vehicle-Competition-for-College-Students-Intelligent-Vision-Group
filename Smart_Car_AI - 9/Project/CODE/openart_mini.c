#include "openart_mini.h"
#include "buzzer.h"
#include "System.h"

#define OPENART_MINI_UART USART_4

uint8 openart_rx_buffer;
lpuart_transfer_t openart_receivexfer;
lpuart_handle_t openart_g_lpuartHandle;

char OpenArtCommandRecv[100]; //储存接收到的信息
char OpenArtByteRecv;         //存储收到的字节
uint8 OpenArtCommandLength = 0;
uint8 DetectResult;           //当前识别结果0:未识别,1:动物,2:交通工具,3:水果

/***********************************************************
 * @brief Openart Mini通信指令
 * @param type 指令种类
 * @return
 ***********************************************************/
void SendCommand(uint8 type)
{
	switch(type)
	{
		case 0: uart_putchar(OPENART_MINI_UART, 0x00); break; //发送命令0x00
		case 1: uart_putchar(OPENART_MINI_UART, 0x01); break; //发送巡点命令
		case 2: uart_putchar(OPENART_MINI_UART, 0x02); break; //发送完成命令
		case 3: uart_putchar(OPENART_MINI_UART, 0x03); break; //发送开始识别的命令
	}
}
/***********************************************************
 * @brief 处理openart传来的信息
 * @param CommandRecv 存储指令数组头指针
 * @param CommandLength 指令长度
 * @return
 ***********************************************************/
void handle_OpenartMessage(char *CommandRecv, uint8 CommandLength)
{
  char *p = CommandRecv;
	char temp[20] = "animal_cat\n";
	SendCommand(2);          //发送完成命令
  while ((p - CommandRecv) < CommandLength)
  {
    if (strstr(p, "animal_cat"))
    {
			strcpy(temp, "animal_cat\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
      DetectResult = 1;			     
    }
		else if (strstr(p, "animal_cattle"))
    {
      strcpy(temp, "animal_cattle\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
      DetectResult = 1;		
    }
		else if (strstr(p, "animal_dog"))
    {
      strcpy(temp, "animal_dog\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
      DetectResult = 1;		
    }
		else if (strstr(p, "animal_horse"))
    {
      strcpy(temp, "animal_horse\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
      DetectResult = 1;		
    }
		else if (strstr(p, "animal_pig"))
    {
      strcpy(temp, "animal_pig\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
      DetectResult = 1;		
    }
    else if (strstr(p, "vehicle_bus"))
    {
			strcpy(temp, "vehicle_bus\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
			DetectResult = 2;	
    }
		else if (strstr(p, "vehicle_car"))
    {
			strcpy(temp, "vehicle_car\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
			DetectResult = 2;	
    }
		else if (strstr(p, "vehicle_plane"))
    {
			strcpy(temp, "vehicle_plane\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
			DetectResult = 2;	
    }
		else if (strstr(p, "vehicle_ship"))
    {
			strcpy(temp, "vehicle_ship\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
			DetectResult = 2;	
    }
		else if (strstr(p, "vehicle_train"))
    {
			strcpy(temp, "vehicle_train\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
			DetectResult = 2;	
    }
    else if (strstr(p, "fruit_apple"))
    {
			strcpy(temp, "fruit_apple\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
			DetectResult = 3;
    }
    else if (strstr(p, "fruit_banana"))
    {
			strcpy(temp, "fruit_banana\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
			DetectResult = 3;
    }
    else if (strstr(p, "fruit_durian"))
    {
			strcpy(temp, "fruit_durian\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
			DetectResult = 3;
    }
		else if (strstr(p, "fruit_grape"))
    {
			strcpy(temp, "fruit_grape\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
			DetectResult = 3;
    }
		else if (strstr(p, "fruit_orange"))
    {
			strcpy(temp, "fruit_orange\n");
      seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
			DetectResult = 3;
    }
    p++;
  }
	switch(DetectResult)
	{
		rt_mb_send(buzzer_mailbox, 500);
		case 1: rt_event_send(run_event, 1); break;//发送事件1，表明要搬运的图片为动物
		case 2: rt_event_send(run_event, 2); break;//发送事件1，表明要搬运的图片为动物
		case 3: rt_event_send(run_event, 3); break;//发送事件3，表明要搬运的图片为水果
	}
}
/***********************************************************
 * @brief 接收openart传来的目标点信息
 * @param CommandRecv 存储指令数组头指针
 * @param CommandLength 指令长度
 * @return
 ***********************************************************/
void receive_dot(char *CommandRecv, uint8 CommandLength)
{
	char p[20];
	int i = 0;
	char delims[] = ",";
	char *result;
	result = strtok(CommandRecv, delims);
	while(result != NULL)
  {
		p[i] = atoi(result);
		result = strtok(NULL, delims);
		i++;
	}
	for(int j = 0; j < i - 1; j = j+2)
	{
		dot[CarInfo.dotnum].x = p[j];
		dot[CarInfo.dotnum].y = p[j + 1];
		dot[CarInfo.dotnum].flag = 1;
		CarInfo.dotnum++;
	}
}
/***********************************************************
 * @brief 接收openart传来的目标点信息
 * @param CommandRecv 存储指令数组头指针
 * @param CommandLength 指令长度
 * @return
 ***********************************************************/
void draw_rec(char *CommandRecv, uint8 CommandLength)
{
	char p[20];
	int i, n = 0;
	char delims[] = ",";
	char *result;
	result = strtok(CommandRecv, delims);
	while(result != NULL)
  {
		p[i] = atoi(result);
		result = strtok(NULL, delims);
		i++;
	}
	for(int j = 0; j < i - 1; j = j + 2)
	{
		rec[n][0] = p[j] * 0.5875;
		rec[n][1] = p[j + 1] * 0.5;
		n++;
		if(n >= 4)
			break;
	}
}
/***********************************************************
 * @brief Openart通信处理函数
 * @param
 * @return
 ***********************************************************/
void openart_uart1_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
  if (kStatus_LPUART_RxIdle == status)
  {
    //串口收到数据后会自动进入到这里，然后读取openart_rx_buffer变量即可读取串口收到的数据
    OpenArtByteRecv = (char)openart_rx_buffer;
    OpenArtCommandRecv[OpenArtCommandLength++] = OpenArtByteRecv;
    if (OpenArtByteRecv == ';')
    {
      handle_OpenartMessage(OpenArtCommandRecv, OpenArtCommandLength);
      OpenArtCommandLength = 0;
    }
		if (OpenArtByteRecv == '#')
    {
			SendCommand(2);  //发送完成命令
      //接收openart传输的目标点信息（每两个个字符第一个是x第二个是y，字符数组最后是‘#’）
      receive_dot(OpenArtCommandRecv, OpenArtCommandLength);			
      OpenArtCommandLength = 0;
    }
		if (OpenArtByteRecv == '?')
    {
      draw_rec(OpenArtCommandRecv, OpenArtCommandLength);
      OpenArtCommandLength = 0;
			rt_mb_send(buzzer_mailbox, 100);
    }
  }
  handle->rxDataSize = openart_receivexfer.dataSize; //还原缓冲区长度
  handle->rxData = openart_receivexfer.data;         //还原缓冲区地址
}
/***********************************************************
 * @brief Openart通信串口初始化
 * @param
 * @return
 ***********************************************************/
void openart_mini_init(void)
{
  uart_init(USART_4, 115200, UART4_TX_C16, UART4_RX_C17);
  //配置串口接收的缓冲区及缓冲区长度
  openart_receivexfer.dataSize = 1;
  openart_receivexfer.data = &openart_rx_buffer;

  //设置中断函数及其参数
  uart_set_handle(USART_4, &openart_g_lpuartHandle, openart_uart1_callback, NULL, 0, openart_receivexfer.data, 1);

  NVIC_SetPriority(LPUART4_IRQn, 14); //设置串口中断优先级 范围0-15 越小优先级越高
  uart_rx_irq(USART_4, 1);            //串口接收中断设置：0：关闭；1：打开
}