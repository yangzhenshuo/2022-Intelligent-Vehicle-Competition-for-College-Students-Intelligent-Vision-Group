/*
 * Image.c
 * ͼ������ʶ����
 * Author:����˶
 */
#include "image.h"
#include "math.h"
#include "System.h"
#include "ProjectMath.h"
#include "communicate.h"
#include "SEEKFREE_MT9V03X_CSI.h"

uint8 BinaryImage[IMAGE_H][IMAGE_W];   //��Ŷ�ֵͼ
uint8 hist_eq_image[IMAGE_H][IMAGE_W]; //���ֱ��ͼ���⻯ͼƬ

static int Ysite = 0, Xsite = 0; // X��Y����
static uint8 *PicTemp;           //���浥��ͼ��

BoxDataTypedef BoxData;         //������Ϣ
ImageStatusTypedef ImageStatus; //ͼ��״̬��Ϣ
BoxTypedef Box;                 //����ĳ���
DotTypedef Dot[20];             //��ŵ��x��y����
uint16 Num = 0;                 //Ŀ��������

///***********************************************************
// * @brief ����ͷ�̵߳���ں���
// * @param
// * @return
// ***********************************************************/
// void camera_entry()
//{
//  rt_thread_mdelay(10); // ��ʱ�ȴ�finsh��ʼ�����
//  while (1)
//  {
//    if (mt9v03x_csi_finish_flag)
//    {
//      //����ͼƬ���ݵ���λ��
//      csi_seekfree_sendimg_03x(USART_1, BinaryImage[0], MT9V03X_CSI_W, MT9V03X_CSI_H);
//			//seekfree_wireless_send_buff(BinaryImage[0],sizeof(BinaryImage[0])-1);
//      mt9v03x_csi_finish_flag = 0;
//    }
//  }
//}
///***********************************************************
// * @brief ����ͷͼ����ʾ�̳߳�ʼ��
// * @param
// * @return
// ***********************************************************/
// void camera_thread_init()
//{
//  rt_thread_t camera;
//  camera = rt_thread_create("camera",     //�߳�����
//                            camera_entry, //�߳���ں���
//                            RT_NULL,      //�߳���ڲ���
//                            1024,         //�߳�ջ��С1024�ֽ�
//                            31,           //�߳����ȼ�
//                            30);          //�߳�ʱ��Ƭ��ͬ���ȼ��߳�������

//  //�����߳�
//  if (RT_NULL != camera)
//  {
//    rt_thread_startup(camera);
//  }
//}
/***********************************************************
 * @brief ����㷨
 * @param image ͼ���ַ
 * @param col ͼ���ȣ���Χ1~IMAGE_W��
 * @param row ͼ��߶ȣ���Χ1~IMAGE_H��
 * @return ��ֵ
 ***********************************************************/
static inline uint8 OtsuThreshold(uint8 *image, uint16 col, uint16 row)
{
#define GrayScale 256
#define StartGray 90
  uint16 width = col;
  uint16 height = row;
  int pixelCount[GrayScale]; //ÿ���Ҷȼ������ظ���
  float pixelPro[GrayScale]; //ÿ���Ҷȼ������ظ���
  int16 i, j, pixelSum = width * height;
  uint8 threshold = 0;
  uint8 *data = image; //ָ���������ݵ�ָ��
  for (i = 0; i < GrayScale; i++)
  {
    pixelCount[i] = 0;
    pixelPro[i] = 0.0;
  }
  //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
  int grayTmp = 0;
  uint32 graySum = 0; //�Ҷ�ֵ��
  for (i = 0; i < height; i += 2)
  {
    for (j = 0; j < width; j += 2)
    {
      grayTmp = (int)data[i * IMAGE_W + j];
      pixelCount[grayTmp]++; //������ֵ��Ϊ����������±�
      graySum += grayTmp;    //����ͼƬ�ܻҶ�ֵ��
    }
  }
  float grayEverage = (float)graySum / pixelSum; //����ͼ��ƽ���Ҷ�

  //����ÿ������������ͼ���еı���
  for (i = 0; i < GrayScale / 2; i++)
  {
    pixelPro[i] = (float)pixelCount[i] / pixelSum;
  }

  //�����Ҷȼ�[0,255]
  float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0;
  w1 = u1tmp = 0;
  for (i = GrayScale - 1; i >= StartGray; i--) // i��Ϊ��ֵ���Ӻ���ǰ��ǰ��
  {
    w0 += pixelPro[i];        //��������ÿ���Ҷ�ֵ�����ص���ռ����֮�ͣ����������ֵı���
    u0tmp += i * pixelPro[i]; //�������֣�ÿ���Ҷ�ֵ��ı���*�Ҷ�ֵ

    w1 = 1 - w0; //����
    u1tmp = grayEverage - u1tmp;

    u0 = u0tmp / w0; //����ƽ���Ҷ�
    u1 = u1tmp / w1; //ǰ��ƽ���Ҷ�

    deltaTmp = w0 * w1 * pow((u0 - u1), 2); //���㷽��
    if (deltaTmp > deltaMax)
    {
      deltaMax = deltaTmp;
      threshold = i;
    }
  }
  return threshold;
}
/***********************************************************
 * @brief ����������ֵ
 * @param image ͼ���ַ
 * @param col ͼ���ȣ���Χ1~IMAGE_W��
 * @param row ͼ��߶ȣ���Χ1~IMAGE_H��
 * @return ��ֵ
 ***********************************************************/
#define Thres 128 //��ֵ
#define ERROR 2   //���

int16 EdgeThres = 18; //����20  ����25 18

float Iteration_Threshould(uint8 *image, uint16 col_w, uint16 row_h)
{
  uint16_t i = 0, j = 0, N0 = 0, N1 = 0, flag = 0;
  float T0, T1, T2, T_center;
  uint32_t S0 = 0, S1 = 0;
  //��ʼԤ��ֵ
  float threshold = 70.0;
  T2 = threshold;

  uint8 *data = image; //ָ���������ݵ�ָ��
  do
  {
    //�ָ�ͼ��Ϊ������
    for (i = 0; i < row_h; i++)
    {
      for (j = 0; j < col_w; j++)
      {
        if (image[i * col_w + j] < T2)
        {
          S0 += data[i * col_w + j]; // G2���ֵ����ػҶ�ֵ֮��
          N0++;                      // G2���ֵ����ص����
        }
        else
        {
          S1 += data[i * col_w + j]; // G1���ֵ����ػҶ�ֵ֮��
          N1++;                      // G1���ֵ����ص����
        }
      }
    }
    T0 = S0 / N0;             // G2���ֵ�ƽ���Ҷ�ֵ
    T1 = S1 / N1;             // G1���ֵ�ƽ���Ҷ�ֵ
    T_center = (T0 + T1) / 2; //�µ���ֵ
    if (T2 < T_center)
    {
      if ((T_center - T2) > ERROR)
      {
        flag = 1;
      }
      else
      {
        flag = 0;
      }
    }
    else
    {
      if ((T2 - T_center) > ERROR)
      {
        flag = 1;
      }
      else
      {
        flag = 0;
      }
    }
    T2 = T_center;
    threshold = T2;
  } while (flag);
  return threshold;
}
/***********************************************************
 * @brief ͼ��ֱ��ͼ���⻯��Ч�����ѣ�
 ***********************************************************/
void HistEqImageConvert(uint8 *RawImagePtr, uint8 *HistEqImagePtr)
{
  int gray[256] = {0};                //��¼ÿ���Ҷȼ����µ����ظ���
  float gray_prob[256] = {0};         //��¼�Ҷȷֲ�Ƶ��
  float gray_distribution[256] = {0}; //��¼ԭʼͼ����ۼ��ܶ�
  int gray_equal[256] = {0};          //���⻯��ĻҶ�ֵ
  int gray_sum = 0;                   //��������
  gray_sum = IMAGE_H * IMAGE_W;

  //ͳ��ÿ���Ҷ��µ����ظ���
  for (int i = 0; i < gray_sum; i++)
  {
    gray[RawImagePtr[i]]++;
  }

  //ͳ��ԭʼͼ��ĻҶȷֲ�Ƶ��
  for (int i = 0; i < 256; i++)
  {
    gray_prob[i] = (float)gray[i] / gray_sum;
  }
  //�����ۼ��ܶ�
  gray_distribution[0] = gray_prob[0];
  for (int i = 1; i < 256; i++)
  {
    gray_distribution[i] = gray_distribution[i - 1] + gray_prob[i];
  }
  //���¼�����⻯��ĻҶ�ֵ���������롣�ο���ʽ��(N-1)*T+0.5
  for (int i = 0; i < 256; i++)
  {
    gray_equal[i] = (uint8)(255 * gray_distribution[i] + 0.5); //(int)(a+0.5)
  }
  //ֱ��ͼ���⻯,����ԭͼÿ���������ֵ
  for (int i = 0; i < gray_sum; i++)
  {
    HistEqImagePtr[i] = gray_equal[RawImagePtr[i]];
  }
}
/***********************************************************
 * @brief ��ȡ��ֵ����ֵ
 * @param type 0:��򷨣�1����������2�������㷨;3:ֱ��ͼ����Ӵ��
 * @return
 ***********************************************************/
void Binary_threshold(uint8 type)
{
  if (type == 0)
  {
    CarInfo.BinaryThreshold = OtsuThreshold(mt9v03x_csi_image[0], IMAGE_W, IMAGE_H);
  }
  if (type == 1)
  {
    CarInfo.BinaryThreshold = Iteration_Threshould(mt9v03x_csi_image[0], IMAGE_W, IMAGE_H);
  }
  if (type == 2)
  {
    uint8 ImageThreshold1 = OtsuThreshold(mt9v03x_csi_image[0], IMAGE_W, IMAGE_H); //�����һ����ֵ
    for (int i = 0; i < IMAGE_H; i++)
      for (int j = 0; j < IMAGE_W; j++)
      {
        if (mt9v03x_csi_image[i][j] > ImageThreshold1)
        {
          BinaryImage[i][j] = mt9v03x_csi_image[i][j];
        }
        else
        {
          BinaryImage[i][j] = ImageThreshold1 - 10;
        }
      }
    uint8 ImageThreshold2 = OtsuThreshold(BinaryImage[0], IMAGE_W, IMAGE_H); //����ڶ�����ֵ
    CarInfo.BinaryThreshold = ImageThreshold2;
  }
  if (type == 3)
  {
    uint8 *HistImagePtr = hist_eq_image[0];
    HistEqImageConvert(mt9v03x_csi_image[0], HistImagePtr);
    CarInfo.BinaryThreshold = OtsuThreshold(HistImagePtr, IMAGE_W, IMAGE_H);
  }
}
/***********************************************************
 * @brief ��ֵ��ͼ��
 * @param
 * @return
 ***********************************************************/
void Binary_image(void)
{
  for (int i = 0; i < IMAGE_H; i++)
    for (int j = 0; j < IMAGE_W; j++)
    {
      if (mt9v03x_csi_image[i][j] > CarInfo.BinaryThreshold)
      {
        BinaryImage[i][j] = White;
      }
      else
      {
        BinaryImage[i][j] = Black;
      }
    }
}
/***********************************************************
 * @brief �跽���ıߵ���;Ѱ�Ҿ������µ�����ϵ��������ĵ�����
 * @return
 ***********************************************************/
static inline void Big_Scan(void)
{
  for (Ysite = 119; Ysite >= 60; Ysite--) //�������Ҵ��ϵ��£��ұ߿�
  {
    PicTemp = BinaryImage[Ysite];
    if (*(PicTemp + CENTER_POINT) == Black) //Ѱ�����µ�
    {
      //Ѱ�ҵױߵ������
      for (int i = 1; i < 60; i++)
      {
        PicTemp = BinaryImage[Ysite - i];
        if (*(PicTemp + CENTER_POINT) == White)
        {
          Ysite = Ysite - i + 1;
          break;
        }
      }
      for (Xsite = CENTER_POINT; Xsite > 0; Xsite--) //�����ұ���
      {
        if (*(PicTemp + CENTER_POINT - Xsite) == Black) //һ���ҵ����µ��break
        {
          //Ѱ����ߵ������
          for (int j = 1; j < 60; j++)
          {
            if (*(PicTemp + CENTER_POINT - Xsite + j) == White)
            {
              Xsite = CENTER_POINT - Xsite + j - 1;
              break;
            }
          }
          break;
        }
      }
      BoxData.LeftBorder = Xsite;
      BoxData.HighBorder = Ysite;
      break;
    }
  }
  for (Ysite = 0; Ysite < 60; Ysite++)
  {
    PicTemp = BinaryImage[Ysite];
    if (*(PicTemp + CENTER_POINT) == Black) //����ױ�ͼ���е�Ϊ�ڣ�Ѱ�����ϵ�
    {
      //Ѱ�Ҷ��ߵ������
      for (int i = 1; i < 60; i++)
      {
        PicTemp = BinaryImage[Ysite + i];
        if (*(PicTemp + CENTER_POINT) == White)
        {
          Ysite = Ysite + i - 1;
          break;
        }
      }
      for (Xsite = 0; Xsite < CENTER_POINT; Xsite--) //�����ұ���
      {
        if (*(PicTemp + CENTER_POINT + Xsite) == Black)
          break;
      }
      BoxData.RightBorder = CENTER_POINT + Xsite;
      BoxData.LowBorder = Ysite;
      break;
    }
  }
  Box.High = BoxData.HighBorder - BoxData.LowBorder;
  Box.Wide = BoxData.RightBorder - BoxData.LeftBorder;
}
/***********************************************************
 * @brief Ѱ��Ŀ��㲢�ұ任�����Ҹ�����������ľ���
 * @return
 ***********************************************************/
void Found_dot_info()
{
  float k0 = 0.0, k1 = 0.0; //�任ϵ����k0Ϊ�ߵı任ϵ����k1Ϊ��ı任ϵ��
  Big_Scan();
  k0 = 500 / Box.High; // 500��700�ֱ�Ϊʵ�ʳ��صĳ���
  k1 = 700 / Box.Wide;
  for (int i = BoxData.LeftBorder + 1; i < BoxData.RightBorder; i++)
    for (int j = BoxData.LowBorder + 1; j < BoxData.HighBorder; j++)
    {
      if (BinaryImage[j][i] == Black && BinaryImage[j][i + 1] == Black)
      {
        Dot[Num].x = i;
        Dot[Num].y = j;
        Num++;
      }
    }
  for (int i = 0; i < Num; i++)
  {
    Dot[i].x = (int)(Dot[i].x - BoxData.LeftBorder) * k1 / 20;
    Dot[i].y = (int)(BoxData.HighBorder - Dot[i].y) * k0 / 20;
  }
}
/*************************************************************
 * @brief ���㵱ǰ����ͼƬ���ڵĽǶ�
 *
 * @return ������С�Ƕ�
 *************************************************************/
uint8 angle_dot[2];        //������Ϣ
uint8 Left_border[20][2];  //�����Ϣ
uint8 Right_border[20][2]; //�ұ���Ϣ

uint8 test1[] = "start find dot\n";
uint8 test2[] = "find dot\n";

void Computing_angle()
{
  int i, j;
  int Flag1 = 0;      //�ҵ������־
  double left_angle;  //��߼н�
  double right_angle; //�ұ߼н�
  uint8 *row_pic;     //���浥��ͼƬ
	
  for (j = 119; j > 0; j--)
  {
		
    row_pic = BinaryImage[j];
    for (i = 1; i < 187; i++)
    {
//			seekfree_wireless_send_buff(test1,sizeof(test1)-1);
      if (*(row_pic + i) == White && *(row_pic + i - 1) == Black ) //�ҵ�����(�ڰ�)
      {
        angle_dot[0] = i; //���������
        angle_dot[1] = j; //����������
        Flag1 = 1;
			  seekfree_wireless_send_buff(test2,sizeof(test2)-1);
        break;
      }
    }
    if (Flag1 == 1)
		{
			ReportDot();
      break;
		}
  }
  //��ͼ����ߣ��Ӷ�������
  int left_flag = 0;
  for (j = angle_dot[1]; j > 0; j--)
  {
    row_pic = BinaryImage[j];
    for (i = angle_dot[0]; i >= 0; i--)
    {
      if (*(row_pic + i) == White && *(row_pic + i - 1) == Black && *(row_pic + i - 2) == Black) //�ҵ��߽��(�׺ں�)
      {
        Left_border[left_flag][0] = i; //������
        Left_border[left_flag][1] = j;
        left_flag++;
        break;
      }
    }
    if (left_flag > 20)
      break;
  }
  //��ͼ���ұߣ��Ӷ������ң�
  int right_flag = 0;
  for (j = angle_dot[1]; j > 0; j--)
  {
    row_pic = BinaryImage[j];
    for (i = angle_dot[0]; i < 188; i++)
    {
      if (*(row_pic + i) == White && *(row_pic + i + 1) == Black && *(row_pic + i + 2) == Black) //�ҵ��߽��(�׺ں�)
      {
        Right_border[right_flag][0] = i; //������
        Right_border[right_flag][1] = j;
        right_flag++;
        break;
      }
    }
    if (right_flag > 20)
      break;
  }
  //�����������ߵĽǶȴ�С
  left_angle = atan2(angle_dot[1] - Left_border[10][1], angle_dot[0] - Left_border[10][0]);
  right_angle = atan2(angle_dot[1] - Right_border[10][1], Right_border[10][0] - angle_dot[0]);
  if (left_angle > right_angle)
    CarInfo.current_angle = right_angle;
  else
    CarInfo.current_angle = left_angle;
}