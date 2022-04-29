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

/***********************************************************
 * @brief ͼ������
 * @param
 * @return
***********************************************************/
void ReportImageStatus(void)
{
    char temp[100];
     sprintf(temp, "%d,%d;\n", CarInfo.BinaryThreshold, SystemSettings.BinaryMethod);
     seekfree_wireless_send_buff((uint8 *)temp, strlen(temp));
}
