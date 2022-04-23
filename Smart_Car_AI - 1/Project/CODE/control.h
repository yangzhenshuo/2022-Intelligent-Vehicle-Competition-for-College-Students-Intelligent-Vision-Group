#ifndef _control_h
#define _control_h

#include "common.h"

void ResetControlArgs(void);
void Speedmath(float x, float y, float z);
void GetMotorSpeed(void);
void AngleControl(void);
void SpeedControl(void);
void PositionControl(void);
#endif