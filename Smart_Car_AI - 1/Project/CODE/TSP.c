/*
 * TSP.c
 * �Ŵ��㷨��TSP����
 * Author:����˶
 */
#include "TSP.h"
#include "image.h"


int chrom[Size_pop][lenchrom];
int best_result[lenchrom]; // ���·��
double min_distance; // ���·������
//��Ⱥ��ʼ��
void init(void)
{
    int num = 0;
    while(num < Size_pop)
    {
        for(int i=0;i<Size_pop;i++)
            for(int j=0;j<lenchrom;j++)
                chrom[i][j] = j+1;
        num++;
        for(int i=0;i<lenchrom-1;i++)
        {
            for(int j=i+1;j<lenchrom;j++)
            {
                int temp = chrom[num][i];
                chrom[num][i] = chrom[num][j];
                chrom[num][j] = temp; // ������num������ĵ�i��Ԫ�غ͵�j��Ԫ��
                num++;
                if(num >= Size_pop)
                    break;
            }
            if(num >= Size_pop)
                break;
        }
        // ������������ѭ�������޷������㹻�ĳ�ʼ���壬������ٲ���һ����
        // ���巽ʽ����ѡ����������λ�ã�Ȼ�󽻻�
        while(num < Size_pop)
        {
            double r1 = ((double)rand())/(RAND_MAX+1.0);
            double r2 = ((double)rand())/(RAND_MAX+1.0);
            int p1 = (int)(lenchrom*r1); // λ��1
            int p2 = (int)(lenchrom*r2); // λ��2
            int temp = chrom[num][p1];
            chrom[num][p1] = chrom[num][p2];
            chrom[num][p2] = temp;    // ��������λ��
            num++;
        }
    }
}

//���뺯��
double distance(DotTypedef d1,DotTypedef d2)
{
    double x1 = d1.x;
    double y1 = d1.y;
    double x2 = d2.x;
    double y2 = d2.y;
    double dis = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
    return dis;
}
// min()����
double * min(double * arr)
{
    static double best_index[2];
    double min_dis = *arr;
    double min_index = 0;
    for(int i=1;i<Size_pop;i++)
    {
        double dis = *(arr+i);
        if(dis < min_dis)
        {
            min_dis = dis;
            min_index = i;
        }
    }
    best_index[0] = min_index;
    best_index[1] = min_dis;
    return best_index;
}
// ����·������
double path_len(int * arr)
{
    double path = 0; // ��ʼ��·������
    int index = *arr; // ��λ����һ������(�������)
    for(int i=0;i<lenchrom-1;i++)
    {
        int index1 = *(arr+i);
        int index2 = *(arr+i+1);
        double dis = distance(Dot[index1-1],Dot[index2-1]);
        path += dis;
    }
    int last_index = *(arr+lenchrom-1); // ���һ���������
    int first_index = *arr; // ��һ���������
    double last_dis = distance(Dot[last_index-1],Dot[first_index-1]);
    path = path + last_dis;
    return path; // �����ܵ�·������
}
// ѡ�����
void Choice(int chrom[Size_pop][lenchrom])
{
    double pick;
    double choice_arr[Size_pop][lenchrom];
    double fit_pro[Size_pop];
    double sum = 0;
    double fit[Size_pop]; // ��Ӧ�Ⱥ�������(����ĵ���)
    for(int j=0;j<Size_pop;j++)
    {
        double path = path_len(chrom[j]);
        double fitness = 1/path;
        fit[j] = fitness;
        sum += fitness;
    }
    for(int j=0;j<Size_pop;j++)
    {
        fit_pro[j] = fit[j]/sum; // ��������
    }
    // ��ʼ���̶�
    for(int i=0;i<Size_pop;i++)
    {
        pick = ((double)rand())/RAND_MAX; // 0��1֮��������  
        for(int j=0;j<Size_pop;j++)
        {
            pick = pick - fit_pro[j];
            if(pick<=0)
            {
                for(int k=0;k<lenchrom;k++)
                    choice_arr[i][k] = chrom[j][k]; // ѡ��һ������
                break;    
            }
        }

    }
    for(int i=0;i<Size_pop;i++)
    {
        for(int j=0;j<lenchrom;j++)
            chrom[i][j] = choice_arr[i][j];
    }
}
//�������
void Cross(int chrom[Size_pop][lenchrom])
{
    double pick;
    double pick1,pick2;
    int choice1,choice2;
    int pos1,pos2;
    int temp;
    int conflict1[lenchrom]; // ��ͻλ��
    int conflict2[lenchrom];
    int num1,num2;
    int index1,index2;
    int move = 0; // ��ǰ�ƶ���λ��
    while(move<lenchrom-1)
    {
        pick = ((double)rand())/RAND_MAX; // ���ھ����Ƿ���н������
        if(pick > pcross)
        {
            move += 2;
            continue; // ���β����н���
        }
        // ���ò���ӳ���ӽ�
        choice1 = move; // ����ѡȡ�ӽ�����������
        choice2 = move+1; // ע������±�Խ��
        pick1 = ((double)rand())/(RAND_MAX+1.0);
        pick2 = ((double)rand())/(RAND_MAX+1.0);
        pos1 = (int)(pick1*lenchrom); // ����ȷ�������ӽ����λ��
        pos2 = (int)(pick2*lenchrom); 
        while(pos1 > lenchrom -2 || pos1 < 1)
        {
            pick1 = ((double)rand())/(RAND_MAX+1.0);
            pos1 = (int)(pick1*lenchrom);
        }
        while(pos2 > lenchrom -2 || pos2 < 1)
        {
            pick2 = ((double)rand())/(RAND_MAX+1.0);
            pos2 = (int)(pick2*lenchrom);
        }
        if(pos1 > pos2)
        {
            temp = pos1;
            pos1 = pos2;
            pos2 = temp; // ����pos1��pos2��λ��
        }
        for(int j=pos1;j<=pos2;j++)
        {
            temp = chrom[choice1][j];
            chrom[choice1][j] = chrom[choice2][j];
            chrom[choice2][j] = temp; // �������˳��
        }
        num1 = 0;
        num2 = 0;
        if(pos1 > 0 && pos2 < lenchrom-1)
        {
            for(int j =0;j<=pos1-1;j++)
            {
                for(int k=pos1;k<=pos2;k++)
                {
                    if(chrom[choice1][j] == chrom[choice1][k])
                    {
                        conflict1[num1] = j;
                        num1++;
                    }
                    if(chrom[choice2][j] == chrom[choice2][k])
                    {
                        conflict2[num2] = j;
                        num2++;
                    }
                }
            }
            for(int j=pos2+1;j<lenchrom;j++)
            {
                for(int k=pos1;k<=pos2;k++)
                {
                    if(chrom[choice1][j] == chrom[choice1][k])
                    {
                        conflict1[num1] = j;
                        num1++;
                    }
                    if(chrom[choice2][j] == chrom[choice2][k])
                    {
                        conflict2[num2] = j;
                        num2++;                     
                    }                 
                }

            }
        }
        if((num1 == num2) && num1 > 0)
        {
            for(int j=0;j<num1;j++)
            {
                index1 = conflict1[j];
                index2 = conflict2[j];
                temp = chrom[choice1][index1]; // ������ͻ��λ��
                chrom[choice1][index1] = chrom[choice2][index2];
                chrom[choice2][index2] = temp;
            }
        }
        move += 2;
    }
}
// �������
// ������Բ�ȡ���ѡȡ�����㣬����Ի�λ��
void Mutation(int chrom[Size_pop][lenchrom])
{
    double pick,pick1,pick2;
    int pos1,pos2,temp;
    for(int i=0;i<Size_pop;i++)
    {
         pick = ((double)rand())/RAND_MAX; // �����ж��Ƿ���б������
        if(pick > pmutation)
            continue;
        pick1 = ((double)rand())/(RAND_MAX+1.0);
        pick2 = ((double)rand())/(RAND_MAX+1.0);
        pos1 = (int)(pick1*lenchrom); // ѡȡ���б����λ��
        pos2 = (int)(pick2*lenchrom);
        while(pos1 > lenchrom-1)
        {
            pick1 = ((double)rand())/(RAND_MAX+1.0);
            pos1 = (int)(pick1*lenchrom);
        }
        while(pos2 > lenchrom-1)
        {
            pick2 = ((double)rand())/(RAND_MAX+1.0);
            pos2 = (int)(pick2*lenchrom);
        }
        temp = chrom[i][pos1];
        chrom[i][pos1] = chrom[i][pos2];
        chrom[i][pos2] = temp;
    }
}

// ������ת����
void Reverse(int chrom[Size_pop][lenchrom])
{
    double pick1,pick2;
    double dis,reverse_dis;
    int n;
    int flag,pos1,pos2,temp;
    int reverse_arr[lenchrom];
    
    for(int i=0;i<Size_pop;i++)
    {
        flag = 0; // ���ڿ��Ʊ�����ת�Ƿ���Ч
        while(flag == 0)
        {
            pick1 = ((double)rand())/(RAND_MAX+1.0);
            pick2 = ((double)rand())/(RAND_MAX+1.0);
            pos1 = (int)(pick1*lenchrom); // ѡȡ������ת������λ��
            pos2 = (int)(pick2*lenchrom);
            while(pos1 > lenchrom-1)
            {
                pick1 = ((double)rand())/(RAND_MAX+1.0);
                pos1 = (int)(pick1*lenchrom);
            }
            while(pos2 > lenchrom -1)
            {
                pick2 = ((double)rand())/(RAND_MAX+1.0);
                pos2 = (int)(pick2*lenchrom);
            }
            if(pos1 > pos2)
            {
                temp = pos1;
                pos1 = pos2;
                pos2 = temp; // ����ʹ��pos1 <= pos2
            }
            if(pos1 < pos2)
            {
                for(int j=0;j<lenchrom;j++)
                    reverse_arr[j] = chrom[i][j]; // ��������
                n = 0; // ��ת��Ŀ
                for(int j=pos1;j<=pos2;j++)
                {
                    reverse_arr[j] = chrom[i][pos2-n]; // ��ת����
                    n++; 
                }
                reverse_dis = path_len(reverse_arr); // ��ת֮��ľ���
                dis = path_len(chrom[i]); // ԭʼ����
                if(reverse_dis < dis)
                {
                    for(int j=0;j<lenchrom;j++)
                        chrom[i][j] = reverse_arr[j]; // ���¸���
                }
            }
            flag = 1;
        }    

    }   
}
void gen_tsp()
{
	init();// ��ʼ����Ⱥ
	int best_fit_index = 0; //���·�����ִ���
	double distance_arr[Size_pop];
	double dis;
	for(int j=0;j<Size_pop;j++)
    {
        dis = path_len(chrom[j]);
        distance_arr[j] = dis;
    }
    double * best_index = min(distance_arr); // �������·�������
    min_distance = *(best_index+1); // ���·��
    int index = (int)(*best_index); // ���·�����
    for(int j=0;j<lenchrom;j++)
        best_result[j] = chrom[index][j]; // ���·������
		// ��ʼ����
    double * new_arr;
    double new_min_dis;
    int new_index;
    for(int i=0;i<Max_gen;i++) 
    {
        Choice(chrom); // ѡ��
        Cross(chrom); //����
        Mutation(chrom); //����
        Reverse(chrom); // ��ת����
        for(int j=0;j<Size_pop;j++)
            distance_arr[j] = path_len(chrom[j]); // ��������
        new_arr = min(distance_arr);
        new_min_dis = *(new_arr+1); //�µ����·��
        if(new_min_dis < min_distance)
        {
            min_distance = new_min_dis; // �������·��
            new_index =(int)(*new_arr);
            for(int j=0;j<lenchrom;j++)
                best_result[j] = chrom[new_index][j]; // �������·������
            best_fit_index = i+1; // ���·������
        }
    }
}