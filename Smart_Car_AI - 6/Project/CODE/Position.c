/*
 * Position.c
 * ��λ
 * Author: ����˶
 */
#include "Position.h"
#include "System.h"
/***********************************************************
 * @brief icm20602���й��Ե�������Զ�λ
 * @param
 * @return
***********************************************************/
//IMU��λ
void location(void)
{
	icm.acc_x = (9.8*icm_acc_x)/4096.0;
  icm.acc_y = (9.8*icm_acc_y)/4096.0;
	icm.acc_z = (9.8*icm_acc_z)/4096.0;
  icm.gyro_x = (icm_gyro_x-GyroOffset.x)/16.4;
  icm.gyro_y = (icm_gyro_y-GyroOffset.y)/16.4;
  icm.gyro_z = (icm_gyro_z-GyroOffset.z)/16.4;
	//��ú����yaw�뵱ǰλ��
	if(CarInfo.yaw>180||CarInfo.yaw<-180)
		CarInfo.yaw = 0;
	CarInfo.yaw += icm.gyro_z*dtt;
	//x������ٶ�
	speed.x += icm.acc_x*dtt;
	//y������ٶ�
	speed.y += icm.acc_y*dtt;
	//λ�ø���
	position.x += speed.x*dtt+0.5*icm.acc_x*dtt*dtt;
	position.y += speed.y*dtt+0.5*icm.acc_y*dtt*dtt;
}
/***********************************************************
 * @brief λ�õ����껯
 * @param
 * @return
***********************************************************/
void coordinatograph(void)
{
	coordinate.x = position.x/0.2;
	coordinate.y = position.y/0.2;
}


