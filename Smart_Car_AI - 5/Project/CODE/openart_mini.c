#include "openart_mini.h"
#include "buzzer.h"
#include "System.h"

#define OPENART_MINI_UART USART_4

OpenArtMiniInfoTypedef OpenArtMiniInfo; // OpenArt Mini��Ϣ

uint8 openart_rx_buffer;
lpuart_transfer_t openart_receivexfer;
lpuart_handle_t openart_g_lpuartHandle;

char OpenArtCommandRecv[100]; //������յ�����Ϣ
char OpenArtByteRecv;         //�洢�յ����ֽ�
uint8 OpenArtCommandLength = 0;

char send[] = "complete\n";
/***********************************************************
 * @brief ����openart��������Ϣ
 * @param CommandRecv �洢ָ������ͷָ��
 * @param CommandLength ָ���
 * @return
 ***********************************************************/
void handle_OpenartMessage(char *CommandRecv, uint8 CommandLength)
{
  char *p = CommandRecv;
  while ((p - CommandRecv) < CommandLength)
  {
    if (strstr(p, "orange"))
    {
      rt_kprintf("orange\n");			
      rt_mb_send(buzzer_mailbox, 100);
    }
    else if (strstr(p, "durain"))
    {
      rt_kprintf("durain\n");
    }
    else if (strstr(p, "apple"))
    {
      rt_kprintf("apple\n");
    }
    else if (strstr(p, "grape"))
    {
      rt_kprintf("grape\n");
    }
    else if (strstr(p, "banana"))
    {
      rt_kprintf("banana\n");
    }
    p++;
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
	int dot_num = 0;
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
	uart_putchar(OPENART_MINI_UART, 0x02);
  CarInfo.dotnum++;
	SystemSettings.Complete = 'T';
}
/***********************************************************
 * @brief Openart Mini��Ϣ��ʼ��
 * @param
 * @return
 ***********************************************************/
void OpenArtMiniSoftInit(void)
{
  OpenArtMiniInfo.DetectResult = 0;
  OpenArtMiniInfo.NumberDetectResult = 0;
  OpenArtMiniInfo.AprilTagDetectResult = 0;
  uart_putchar(OPENART_MINI_UART, 0x0A);
}

/***********************************************************
 * @brief Openart Miniͨ��ָ��
 * @param type ָ������
 * @return
 ***********************************************************/
void SendCommand2OpenArtMini(uint8 type)
{
  if (type == 0) //����
  {
    uart_putchar(OPENART_MINI_UART, 0x0B);
  }
  if (type == 1) //�л�ʶ��ģʽ
  {
    uart_putchar(OPENART_MINI_UART, 0x0C);
  }
  if (type == 2) //֪ͨOpenArt Mini����ɶ���
  {
    if (OpenArtMiniInfo.DetectResult == 4) //��������ʾʶ�𼴽���ʼ
    {
      rt_mb_send(buzzer_mailbox, 500);
    }
    else
    {
      rt_mb_send(buzzer_mailbox, 20);
    }
    uart_putchar(OPENART_MINI_UART, 0x0D);
    OpenArtMiniInfo.DetectResult = 0; //�ָ�����ģʽ
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
			//seekfree_wireless_send_buff((uint8 *)OpenArtCommandRecv, strlen(OpenArtCommandRecv));
      //����openart�����Ŀ�����Ϣ��ÿ�������ַ���һ����x�ڶ�����y���ַ���������ǡ�#����
      receive_dot(OpenArtCommandRecv, OpenArtCommandLength);
      OpenArtCommandLength = 0;
      SystemSettings.IsFound_dot = 'T';
			seekfree_wireless_send_buff((uint8 *)send, strlen(send));
      //rt_mb_send(buzzer_mailbox, 500); //��������ʾ���Ѳ��
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