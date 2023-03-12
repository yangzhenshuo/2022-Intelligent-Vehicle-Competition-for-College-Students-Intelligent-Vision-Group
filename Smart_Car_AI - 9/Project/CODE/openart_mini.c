#include "openart_mini.h"
#include "buzzer.h"
#include "System.h"

#define OPENART_MINI_UART USART_4

uint8 openart_rx_buffer;
lpuart_transfer_t openart_receivexfer;
lpuart_handle_t openart_g_lpuartHandle;

char OpenArtCommandRecv[100]; //������յ�����Ϣ
char OpenArtByteRecv;         //�洢�յ����ֽ�
uint8 OpenArtCommandLength = 0;
uint8 DetectResult;           //��ǰʶ����0:δʶ��,1:����,2:��ͨ����,3:ˮ��

/***********************************************************
 * @brief Openart Miniͨ��ָ��
 * @param type ָ������
 * @return
 ***********************************************************/
void SendCommand(uint8 type)
{
	switch(type)
	{
		case 0: uart_putchar(OPENART_MINI_UART, 0x00); break; //��������0x00
		case 1: uart_putchar(OPENART_MINI_UART, 0x01); break; //����Ѳ������
		case 2: uart_putchar(OPENART_MINI_UART, 0x02); break; //�����������
		case 3: uart_putchar(OPENART_MINI_UART, 0x03); break; //���Ϳ�ʼʶ�������
	}
}
/***********************************************************
 * @brief ����openart��������Ϣ
 * @param CommandRecv �洢ָ������ͷָ��
 * @param CommandLength ָ���
 * @return
 ***********************************************************/
void handle_OpenartMessage(char *CommandRecv, uint8 CommandLength)
{
  char *p = CommandRecv;
	char temp[20] = "animal_cat\n";
	SendCommand(2);          //�����������
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
		case 1: rt_event_send(run_event, 1); break;//�����¼�1������Ҫ���˵�ͼƬΪ����
		case 2: rt_event_send(run_event, 2); break;//�����¼�1������Ҫ���˵�ͼƬΪ����
		case 3: rt_event_send(run_event, 3); break;//�����¼�3������Ҫ���˵�ͼƬΪˮ��
	}
}
/***********************************************************
 * @brief ����openart������Ŀ�����Ϣ
 * @param CommandRecv �洢ָ������ͷָ��
 * @param CommandLength ָ���
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
 * @brief ����openart������Ŀ�����Ϣ
 * @param CommandRecv �洢ָ������ͷָ��
 * @param CommandLength ָ���
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
 * @brief Openartͨ�Ŵ�����
 * @param
 * @return
 ***********************************************************/
void openart_uart1_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
  if (kStatus_LPUART_RxIdle == status)
  {
    //�����յ����ݺ���Զ����뵽���Ȼ���ȡopenart_rx_buffer�������ɶ�ȡ�����յ�������
    OpenArtByteRecv = (char)openart_rx_buffer;
    OpenArtCommandRecv[OpenArtCommandLength++] = OpenArtByteRecv;
    if (OpenArtByteRecv == ';')
    {
      handle_OpenartMessage(OpenArtCommandRecv, OpenArtCommandLength);
      OpenArtCommandLength = 0;
    }
		if (OpenArtByteRecv == '#')
    {
			SendCommand(2);  //�����������
      //����openart�����Ŀ�����Ϣ��ÿ�������ַ���һ����x�ڶ�����y���ַ���������ǡ�#����
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
  handle->rxDataSize = openart_receivexfer.dataSize; //��ԭ����������
  handle->rxData = openart_receivexfer.data;         //��ԭ��������ַ
}
/***********************************************************
 * @brief Openartͨ�Ŵ��ڳ�ʼ��
 * @param
 * @return
 ***********************************************************/
void openart_mini_init(void)
{
  uart_init(USART_4, 115200, UART4_TX_C16, UART4_RX_C17);
  //���ô��ڽ��յĻ�����������������
  openart_receivexfer.dataSize = 1;
  openart_receivexfer.data = &openart_rx_buffer;

  //�����жϺ����������
  uart_set_handle(USART_4, &openart_g_lpuartHandle, openart_uart1_callback, NULL, 0, openart_receivexfer.data, 1);

  NVIC_SetPriority(LPUART4_IRQn, 14); //���ô����ж����ȼ� ��Χ0-15 ԽС���ȼ�Խ��
  uart_rx_irq(USART_4, 1);            //���ڽ����ж����ã�0���رգ�1����
}