#ifndef CODE_IMAGE_H
#define CODE_IMAGE_H

#include "common.h"
#include "SEEKFREE_MT9V03X_CSI.h"

#define IMAGE_H (MT9V03X_CSI_H)  //ͼ��߶�
#define IMAGE_W (MT9V03X_CSI_W)  //ͼ����
#define CENTER_POINT (IMAGE_W/2) //ͼ�����ĺ�����
#define Black 0x00               //��ɫ����ֵ
#define White 0xff               //��ɫ����ֵ
//ͼ��״̬��Ϣ
typedef struct
{
	int OFFLine; /////ͼ�񶥱�
  int Flag;    ////0��ʾ���޺����򡱣�1Ϊ����ͨ���ߡ���2Ϊ��������ߡ���3Ϊ��˫��˫�ߡ���4Ϊ�����˫�ߡ���5Ϊ���Ҳ�˫�ߡ�
}ImageStatusTypedef;
//������Ϣ
typedef struct
{ 
  int LeftBorder;    ////��߽��
  int RightBorder;   ////�ұ߽��
  int HighBorder;    ////�߽߱��
  int LowBorder;     ////�ͱ߽��
} BoxDataTypedef;
//����ľ�����Ϣ
typedef struct
{
	int Wide;          ////���
	int High;          ////�߶�
} BoxTypedef;
//Ŀ������Ϣ
typedef struct
{
	int x;
	int y;
} DotTypedef;

extern DotTypedef Dot[20];
extern BoxDataTypedef BoxData;
extern BoxTypedef Box;
extern ImageStatusTypedef ImageStatus;

void camera_DMA(void);
void BinaryImageConvert(uint8 type);
void Found_dot_info(void);
#endif