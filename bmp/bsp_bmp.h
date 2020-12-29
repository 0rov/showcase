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



// 四个字节对齐  进1制处理
#define WIDTHBYTES(bits) (((bits)+31)/32*4)		//对于24位真彩色 每一行的像素宽度必须是4的倍数  否则补0补齐

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;


//位图文件头信息结构定义
//其中不包含文件类型信息（由于结构体的内存结构决定，要是加了的话将不能正确读取文件信息）
typedef struct tagBITMAPFILEHEADER 
{   
    DWORD bfSize; 															//文件大小																				4
    WORD bfReserved1; 													//保留字，不考虑		 															2
    WORD bfReserved2; 													//保留字，同上			 															2
    DWORD bfOffBits; 														//实际位图数据的偏移字节数，即前三个部分长度之和	4
} BITMAPFILEHEADER,tagBITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize; 														//指定此结构体的长度，为40		 												4
    LONG biWidth; 														//位图宽											 												4
    LONG biHeight; 														//位图高											 												4
    WORD biPlanes; 														//平面数，为1								 													2
    WORD biBitCount; 													//采用颜色位数，可以是1，2，4，8，16，24新的可以是32	2
    DWORD biCompression; 											//压缩方式，可以是0，1，2，其中0表示不压缩						4
    DWORD biSizeImage; 												//实际位图数据占用的字节数														4
    LONG biXPelsPerMeter; 										//X方向分辨率																					4
    LONG biYPelsPerMeter; 										//Y方向分辨率																					4
    DWORD biClrUsed; 													//使用的颜色数，如果为0，则表示默认值(2^颜色位数)			4
    DWORD biClrImportant; 										//重要颜色数，如果为0，则表示所有颜色都是重要的				4
} BITMAPINFOHEADER,tagBITMAPINFOHEADER;


typedef struct tagRGBQUAD 
{
    BYTE rgbBlue; 													//该颜色的蓝色分量
    BYTE rgbGreen; 													//该颜色的绿色分量
    BYTE rgbRed; 														//该颜色的红色分量
    BYTE rgbReserved;											 	//保留值
} RGBQUAD,tagRGBQUAD;


typedef struct RGB_PIXEL
{      //像素的数据类型
    unsigned char   rgbBlue;
    unsigned char   rgbGreen;
    unsigned char   rgbRed;
}RGB_PIXEL;


#define GETR_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )RGB565) >>11)<<3))		  			//返回8位 R
#define GETG_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )(RGB565 & 0x7ff)) >>5)<<2)) 	//返回8位 G
#define GETB_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )(RGB565 & 0x1f))<<3)))       	//返回8位 B
#pragma diag_suppress 870 	//使编译器支持多字节字符,否则会有invalid multibyte character sequence警告 



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

