#ifndef CODE_IMAGE_H
#define CODE_IMAGE_H

#include "SEEKFREE_MT9V03X_CSI.h"
#include "headfile.h"

#define IMAGE_H (MT9V03X_CSI_H)    //ͼ��߶�
#define IMAGE_W (MT9V03X_CSI_W)    //ͼ����
#define CENTER_POINT (IMAGE_W / 2) //ͼ�����ĺ�����
#define Black 0x00                 //��ɫ����ֵ
#define White 0xff                 //��ɫ����ֵ

//������Ϣ
typedef struct
{
  int LeftBorder;  ////��߽��
  int RightBorder; ////�ұ߽��
  int HighBorder;  ////�߽߱��
  int LowBorder;   ////�ͱ߽��
} BoxDataTypedef;
//����ľ�����Ϣ
typedef struct
{
  int Wide; ////���
  int High; ////�߶�
} BoxTypedef;

extern BoxDataTypedef BoxData;
extern BoxTypedef Box;
extern uint8 BinaryImage[IMAGE_H][IMAGE_W];   //��Ŷ�ֵͼ
extern uint8 hist_eq_image[IMAGE_H][IMAGE_W]; //���ֱ��ͼ���⻯ͼƬ
extern uint8 angle_dot[2];
extern uint8 midpoint[2];

void BinaryRenew_thread_init(void);
void Binary_renew(uint8 type);
void Found_dot_info(void);
void Binary_image(void);
void find_top_angle(void);
void Scan_line(void);
void find_midpoint(void);
double Computing_angle(uint8 (*p)[IMAGE_W]);

#endif