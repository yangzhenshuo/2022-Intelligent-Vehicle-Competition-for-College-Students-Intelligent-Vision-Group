#ifndef _openartart_mini_h
#define _openartart_mini_h

#include "headfile.h"

extern uint8 DetectResult;           //��ǰʶ����0:δʶ��,1:����,2:��ͨ����,3:ˮ��
void SendCommand(uint8 type);
void OpenArtMiniSoftInit(void);
void openart_mini_init(void);

#endif
