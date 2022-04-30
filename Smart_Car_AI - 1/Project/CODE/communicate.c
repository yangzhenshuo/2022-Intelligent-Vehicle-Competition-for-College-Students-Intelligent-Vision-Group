/*
 * Communicate.c
 * ��λ��ͨ��Э��
 * Author: han
 */
#include "System.h"
#include "Communicate.h"
#include "SEEKFREE_WIRELESS.h"
#include "stdio.h"
#include "stdlib.h"
#include "image.h"

/***********************************************************
 * @brief ͼ������
 * @param
 * @return
***********************************************************/
void ReportImageStatus(void)
{
     char temp[100];
     sprintf(temp, "%d,%d,%f;\n", CarInfo.BinaryThreshold, SystemSettings.BinaryMethod, CarInfo.current_angle);
     seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));	 
}

void ReportDot(void)
{
	char temp1[100];
	sprintf(temp1, "%d,%d;\n", angle_dot[0], angle_dot[1]);
  seekfree_wireless_send_buff((uint8 *)temp1, strlen(temp1));
}