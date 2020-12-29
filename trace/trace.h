#ifndef __COLOR_H
#define __COLOR_H	 
//#include "sys.h"
#include "stm32f10x.h"

#define IMG_X 0							  //ͼƬx����
#define IMG_Y 0               //ͼƬy����
#define IMG_W 320             //ͼƬ���
#define IMG_H 240             //ͼƬ�߶�

#define ALLOW_FAIL_PER 3    //�ݴ���
#define ITERATER_NUM   8    //��������

#define minOf3Values( v1, v2, v3 )			( (v1<v2) ? ( (v1<v3) ? (v1) : (v3) ) \
																									: ( (v2<v3) ? (v2) : (v3) ) )

#define maxOf3Values( v1, v2, v3 )			( (v1>v2) ? ( (v1>v3) ? (v1) : (v3) ) \
																									: ( (v2>v3) ? (v2) : (v3) ) )
typedef struct							//�ж�ΪĿ�������
{
	/*
	unsigned char H_MIN;			//Ŀ����Сɫ��
	unsigned char H_MAX;			//Ŀ�����ɫ��
	
	unsigned char S_MIN;			//Ŀ����С���Ͷ�
	unsigned char S_MAX;			//Ŀ����󱥺Ͷ�
	
	unsigned char L_MIN;			//Ŀ����С����
	unsigned char L_MAX;			//Ŀ���������*/
	
	unsigned int WIDTH_MIN;		//Ŀ����С���
	unsigned int HEIGHT_MIN;	//Ŀ����С�߶�
	
	unsigned int WIDTH_MAX;		//Ŀ�������
	unsigned int HEIGHT_MAX;	//Ŀ�����߶�
}TARGET_CONDITION;

typedef struct						//��Ѱ����
{
	unsigned int X_Start;
	unsigned int X_End;
	unsigned int Y_Start;
	unsigned int Y_End;
}SEARCH_AREA;


typedef struct				//���
{
	unsigned int x;			//Ŀ��x����
	unsigned int y;			//Ŀ��y����
	unsigned int w;			//Ŀ����
	unsigned int h;			//Ŀ��߶�
}RESULT;

extern RESULT result;
extern TARGET_CONDITION condition;


int Trace(const TARGET_CONDITION* condition, RESULT* result_final, SEARCH_AREA area, unsigned int isBlack );	

#endif
