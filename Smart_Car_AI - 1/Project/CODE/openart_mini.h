#ifndef _openartart_mini_h
#define _openartart_mini_h

#include "headfile.h"

typedef struct
{
  uint8 NumberDetectResult;   //����ʶ����
  uint8 AprilTagDetectResult; //AprilTagʶ����
  uint8 DetectResult;           //��ǰʶ����0:δʶ��,1:ˮ��,2:���3����ͨ����
} OpenArtMiniInfoTypedef;     //OpenArt Mini��Ϣ

extern OpenArtMiniInfoTypedef OpenArtMiniInfo;

void SendCommand2OpenArtMini(uint8 type);
void OpenArtMiniSoftInit(void);
void openart_mini_init(void);

#endif
