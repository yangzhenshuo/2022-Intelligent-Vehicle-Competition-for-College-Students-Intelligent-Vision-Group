#ifndef CODE_TSP_H
#define CODE_TSP_H

#include "common.h"

#define Max_gen 200 //����������
#define Size_pop 100 //��Ⱥ��Ŀ
#define pcross 0.6 //�������
#define pmutation 0.1 //�������
#define lenchrom 14 // Ⱦɫ�峤��(���ＴΪ���и���)

extern int chrom[Size_pop][lenchrom];
extern int best_result[lenchrom]; // ���·��
extern double min_distance; // ���·������

// ��������
void init(void); // ��Ⱥ��ʼ������
double * min(double *); // ��������������Сֵ
double path_len(int *); // ����ĳһ��������·�����ȣ���Ӧ�Ⱥ���Ϊ·�߳��ȵĵ���
void Choice(int [Size_pop][lenchrom]); // ѡ�����
void Cross(int [Size_pop][lenchrom]); // �������
void Mutation(int [Size_pop][lenchrom]); // �������
void Reverse(int [Size_pop][lenchrom]); // ��ת����

void gen_tsp(void);          //�Ŵ��㷨���TSP
#endif