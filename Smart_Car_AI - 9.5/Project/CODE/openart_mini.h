#ifndef _openartart_mini_h
#define _openartart_mini_h

#include "headfile.h"

void SendCommand(uint8 type);
void OpenArtMiniSoftInit(void);
void openart_mini_init(void);

extern uint8 DetectResult;           //��ǰʶ����0:δʶ��,1:����,2:��ͨ����,3:ˮ��

#endif
