#ifndef __BSP_BMP_H
#define	__BSP_BMP_H


#include "stm32f10x.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bd_dt.h"
#include "ff.h"



// �ĸ��ֽڶ���  ��1�ƴ���
#define WIDTHBYTES(bits) (((bits)+31)/32*4)		//����24λ���ɫ ÿһ�е����ؿ�ȱ�����4�ı���  ����0����

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;


//λͼ�ļ�ͷ��Ϣ�ṹ����
//���в������ļ�������Ϣ�����ڽṹ����ڴ�ṹ������Ҫ�Ǽ��˵Ļ���������ȷ��ȡ�ļ���Ϣ��
typedef struct tagBITMAPFILEHEADER 
{   
    DWORD bfSize; 															//�ļ���С																				4
    WORD bfReserved1; 													//�����֣�������		 															2
    WORD bfReserved2; 													//�����֣�ͬ��			 															2
    DWORD bfOffBits; 														//ʵ��λͼ���ݵ�ƫ���ֽ�������ǰ�������ֳ���֮��	4
} BITMAPFILEHEADER,tagBITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize; 														//ָ���˽ṹ��ĳ��ȣ�Ϊ40		 												4
    LONG biWidth; 														//λͼ��											 												4
    LONG biHeight; 														//λͼ��											 												4
    WORD biPlanes; 														//ƽ������Ϊ1								 													2
    WORD biBitCount; 													//������ɫλ����������1��2��4��8��16��24�µĿ�����32	2
    DWORD biCompression; 											//ѹ����ʽ��������0��1��2������0��ʾ��ѹ��						4
    DWORD biSizeImage; 												//ʵ��λͼ����ռ�õ��ֽ���														4
    LONG biXPelsPerMeter; 										//X����ֱ���																					4
    LONG biYPelsPerMeter; 										//Y����ֱ���																					4
    DWORD biClrUsed; 													//ʹ�õ���ɫ�������Ϊ0�����ʾĬ��ֵ(2^��ɫλ��)			4
    DWORD biClrImportant; 										//��Ҫ��ɫ�������Ϊ0�����ʾ������ɫ������Ҫ��				4
} BITMAPINFOHEADER,tagBITMAPINFOHEADER;


typedef struct tagRGBQUAD 
{
    BYTE rgbBlue; 													//����ɫ����ɫ����
    BYTE rgbGreen; 													//����ɫ����ɫ����
    BYTE rgbRed; 														//����ɫ�ĺ�ɫ����
    BYTE rgbReserved;											 	//����ֵ
} RGBQUAD,tagRGBQUAD;


typedef struct RGB_PIXEL
{      //���ص���������
    unsigned char   rgbBlue;
    unsigned char   rgbGreen;
    unsigned char   rgbRed;
}RGB_PIXEL;


#define GETR_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )RGB565) >>11)<<3))		  			//����8λ R
#define GETG_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )(RGB565 & 0x7ff)) >>5)<<2)) 	//����8λ G
#define GETB_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )(RGB565 & 0x1f))<<3)))       	//����8λ B
#pragma diag_suppress 870 	//ʹ������֧�ֶ��ֽ��ַ�,�������invalid multibyte character sequence���� 



void  LCD_Show_BMP( uint16_t x, uint16_t y, char * pic_name );
int  Screen_Shot( uint16_t x, uint16_t y, uint16_t Width, uint16_t Height, char * filename );

void Frame_difference(char * pic1_name, char * pic2_name, char * filename);
void Fast_difference(char * pic1_name, char * pic2_name, char * filedst);

int Camera_Shot( uint16_t x, uint16_t y, uint16_t Width, uint16_t Height, char * filename);

extern uint8_t bwdiff[240][40];
extern uint8_t baseimg[240][40];
extern uint8_t mask[240][40];

extern uint8_t buffer[3][960];

extern double streak_list[40][2]; // a(1), b(1)
extern char index0;
extern char index0_past;
extern uint32_t white_cntr;
extern char cnvdetected;

extern int16_t list1[500][2];
extern int16_t list2[500][2];
extern short CpH[2];
extern short left;
extern short right;

extern uint16_t Hsize;
extern uint16_t Bsize;

void bwdisplay(void);
void bwdisplay0(void);
void bwdisplay1(void);

void bwfill(void);
void basevoid(void);
void maskvoid(void);

//char bw_extract_bool(char matrix, uint16_t h_coor, uint16_t b_coor);

void update_list1(uint16_t *list1, uint16_t *list2);

void findstreaks(void);
void clearstreaks(void);
void drawstreaks(void);

void intersections(void);
void cnvyrbase(void);
void whitensector(double Xa, double Ya, double Xb, double Yb);

void border_det(void);
void prepare_bislot(char slot, uint8_t sprout_b, uint8_t merge_b1, uint8_t merge_b2, uint8_t l_height, uint8_t l_start, uint8_t l_finish);

void prepare_slot1(char slot, uint8_t sprout_b, uint8_t merge_b1, uint8_t merge_b2, uint8_t s_height, uint8_t s_start, uint8_t s_finish);
void store_body0(char stored, char slot);
void store_node0(char storedN, char Nsprout, char i, char horizontal_pos);

//void draw_segments000(void);

//extern char connect, sprout, merge, height;

void border_det1(void);
void border_det2(void);

extern BYTE pColorData[960],pColorData1[960],pColorData2[960];
//extern char pColorData1[960],pColorData2[960];

extern FIL bmpfsrc, bmpfsrc1, bmpfsrc2, bmpfdst; 
extern FRESULT bmpres;

#endif /* __BSP_BMP_H */

