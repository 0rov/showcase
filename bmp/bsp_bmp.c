/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   bmp文件驱动，显示bmp图片和给液晶截图
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "ff.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./bmp/bsp_bmp.h"
#include "./ov7725/bsp_ov7725.h"
#include "./led/bsp_led.h"

#include <math.h>

#include "./BD_subfct.h" 

#define RGB24TORGB16(R,G,B) ((unsigned short int)((((R)>>3)<<11) | (((G)>>2)<<5)	| ((B)>>3)))

BYTE pColorData[960];			// 一行真彩色数据缓存 320 * 3 = 960 ///
//me tweaking
BYTE pColorData1[960],pColorData2[960];			
//char pColorData1[960],pColorData2[960];			
FIL bmpfsrc1,bmpfsrc2;
// end me tweaking
FIL bmpfsrc, bmpfdst; 
FRESULT bmpres;

/* 如果不需要打印bmp相关的提示信息,将printf注释掉即可
 * 如要用printf()，需将串口驱动文件包含进来
 */
#define BMP_DEBUG_PRINTF(FORMAT,...)  printf(FORMAT,##__VA_ARGS__)	 

/* 打印BMP文件的头信息，用于调试 */
static void showBmpHead(BITMAPFILEHEADER* pBmpHead)
{
    BMP_DEBUG_PRINTF("位图文件头:\r\n");
    BMP_DEBUG_PRINTF("文件大小:%ld\r\n",(*pBmpHead).bfSize);
    BMP_DEBUG_PRINTF("保留字:%d\r\n",(*pBmpHead).bfReserved1);
    BMP_DEBUG_PRINTF("保留字:%d\r\n",(*pBmpHead).bfReserved2);
    BMP_DEBUG_PRINTF("实际位图数据的偏移字节数:%ld\r\n",(*pBmpHead).bfOffBits);
		BMP_DEBUG_PRINTF("\r\n");	
}

/* 打印BMP文件的头信息，用于调试 */
static void showBmpInforHead(tagBITMAPINFOHEADER* pBmpInforHead)
{
    BMP_DEBUG_PRINTF("位图信息头:\r\n");
    BMP_DEBUG_PRINTF("结构体的长度:%ld\r\n",(*pBmpInforHead).biSize);
    BMP_DEBUG_PRINTF("位图宽:%ld\r\n",(*pBmpInforHead).biWidth);
    BMP_DEBUG_PRINTF("位图高:%ld\r\n",(*pBmpInforHead).biHeight);
    BMP_DEBUG_PRINTF("biPlanes平面数:%d\r\n",(*pBmpInforHead).biPlanes);
    BMP_DEBUG_PRINTF("biBitCount采用颜色位数:%d\r\n",(*pBmpInforHead).biBitCount);
    BMP_DEBUG_PRINTF("压缩方式:%ld\r\n",(*pBmpInforHead).biCompression);
    BMP_DEBUG_PRINTF("biSizeImage实际位图数据占用的字节数:%ld\r\n",(*pBmpInforHead).biSizeImage);
    BMP_DEBUG_PRINTF("X方向分辨率:%ld\r\n",(*pBmpInforHead).biXPelsPerMeter);
    BMP_DEBUG_PRINTF("Y方向分辨率:%ld\r\n",(*pBmpInforHead).biYPelsPerMeter);
    BMP_DEBUG_PRINTF("使用的颜色数:%ld\r\n",(*pBmpInforHead).biClrUsed);
    BMP_DEBUG_PRINTF("重要颜色数:%ld\r\n",(*pBmpInforHead).biClrImportant);
		BMP_DEBUG_PRINTF("\r\n");
}





/**
 * @brief  设置ILI9341的截取BMP图片
 * @param  x ：在扫描模式1下截取区域的起点X坐标 
 * @param  y ：在扫描模式1下截取区域的起点Y坐标 
 * @param  pic_name ：BMP存放的全路径
 * @retval 无
 */

//uint16_t Hsize=240;
//uint16_t Bsize=320;


uint8_t bwdiff[240][40]={0};
uint8_t baseimg[240][40]={0};
uint8_t mask[240][40]={0};

uint8_t buffer[3][960]={0};

double streak_list[40][2]={0};
char index0;
char index0_past=0;
uint32_t white_cntr;
char cnvdetected=0;

int16_t list1[500][2]={0};
int16_t list2[500][2]={0};
short CpH[2]={160,240};
short left;
short right;

	char connect;
//	char sprout;
	char merge;
	char height;
/*
	char stud;
	char stag;

	char detect0;
	char detect1;
	char alive;
	char succumb;
	char byte4;
	char byte5;
	char byte6;
	char sprout;
	char stored_alrd;
	char merge0;
	char merge1;
	char root;
	char tip;

	uint8_t stored;
	uint8_t storedN;
*/
void LCD_Show_BMP ( uint16_t x, uint16_t y, char * pic_name )
{
	int i, j, k;
	int width, height, l_width;

	BYTE red,green,blue;
	BITMAPFILEHEADER bitHead;
	BITMAPINFOHEADER bitInfoHead;
	WORD fileType;

	unsigned int read_num;

	bmpres = f_open( &bmpfsrc , (char *)pic_name, FA_OPEN_EXISTING | FA_READ);	
/*-------------------------------------------------------------------------------------------------------*/
	if(bmpres == FR_OK)
	{
		BMP_DEBUG_PRINTF("打开文件成功\r\n");

		/* 读取文件头信息  两个字节*/         
		f_read(&bmpfsrc,&fileType,sizeof(WORD),&read_num);     

		/* 判断是不是bmp文件 "BM"*/
		if(fileType != 0x4d42)
		{
			BMP_DEBUG_PRINTF("这不是一个 .bmp 文件!\r\n");
			return;
		}
		else
		{
			BMP_DEBUG_PRINTF("这是一个 .bmp 文件\r\n");	
		}        

		/* 读取BMP文件头信息*/
		f_read(&bmpfsrc,&bitHead,sizeof(tagBITMAPFILEHEADER),&read_num);        
		showBmpHead(&bitHead);

		/* 读取位图信息头信息 */
		f_read(&bmpfsrc,&bitInfoHead,sizeof(BITMAPINFOHEADER),&read_num);        
		showBmpInforHead(&bitInfoHead);
	}    
	else
	{
		BMP_DEBUG_PRINTF("打开文件失败!错误代码：bmpres = %d \r\n",bmpres);
		return;
	}    
/*-------------------------------------------------------------------------------------------------------*/
	width = bitInfoHead.biWidth;
	height = bitInfoHead.biHeight;

	/* 计算位图的实际宽度并确保它为32的倍数	*/
	l_width = WIDTHBYTES(width* bitInfoHead.biBitCount);	

	if(l_width > 960)
	{
		BMP_DEBUG_PRINTF("\n 本图片太大，无法在液晶屏上显示 (<=320)\n");
		return;
	}
	
	
	/* 开一个图片大小的窗口*/
	ILI9341_OpenWindow(x, y, width, height);
	ILI9341_Write_Cmd (CMD_SetPixel ); 

	
	/* 判断是否是24bit真彩色图 */
	if( bitInfoHead.biBitCount >= 24 )
	{
		for ( i = 0; i < height; i ++ )
		{
			/*从文件的后面读起，BMP文件的原始图像方向为右下角到左上角*/
      f_lseek ( & bmpfsrc, bitHead .bfOffBits + ( height - i - 1 ) * l_width );	
			
			/* 读取一行bmp的数据到数组pColorData里面 */
			f_read ( & bmpfsrc, pColorData, l_width, & read_num );				

			for(j=0; j<width; j++) 											   //一行有效信息
			{
				k = j*3;																	 //一行中第K个像素的起点
				red = pColorData[k+2];
				green = pColorData[k+1];
				blue = 	pColorData[k];
				ILI9341_Write_Data ( RGB24TORGB16 ( red, green, blue ) ); //写入LCD-GRAM
			}            			
		}        		
	}    	
	else 
	{        
		BMP_DEBUG_PRINTF("这不是一个24位真彩色BMP文件！");
		return ;
	}
	
	f_close(&bmpfsrc);  
  
}




/**
 * @brief  设置ILI9341的截取BMP图片
 * @param  x ：截取区域的起点X坐标 
 * @param  y ：截取区域的起点Y坐标 
 * @param  Width ：区域宽度
 * @param  Height ：区域高度 
 * @retval 无
  *   该参数为以下值之一：
  *     @arg 0 :截图成功
  *     @arg -1 :截图失败
 */
int Screen_Shot( uint16_t x, uint16_t y, uint16_t Width, uint16_t Height, char * filename)
{
	/* bmp  文件头 54个字节 */
	unsigned char header[54] =
	{
		0x42, 0x4d, 0, 0, 0, 0, 
		0, 0, 0, 0, 54, 0, 
		0, 0, 40,0, 0, 0, 
		0, 0, 0, 0, 0, 0, 
		0, 0, 1, 0, 24, 0, 
		0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 
		0, 0, 0
	};
	
	int i;
	int j;
	long file_size;     
	long width;
	long height;
	unsigned char r,g,b;	
	unsigned int mybw;
	unsigned int read_data;
	char kk[4]={0,0,0,0};
	
	uint8_t ucAlign;//
	
	
	/* 宽*高 +补充的字节 + 头部信息 */
	file_size = (long)Width * (long)Height * 3 + Height*(Width%4) + 54;		

	/* 文件大小 4个字节 */
	header[2] = (unsigned char)(file_size &0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;
	
	/* 位图宽 4个字节 */
	width=Width;	
	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) &0x000000ff;
	header[20] = (width >> 16) &0x000000ff;
	header[21] = (width >> 24) &0x000000ff;
	
	/* 位图高 4个字节 */
	height = Height;
	header[22] = height &0x000000ff;
	header[23] = (height >> 8) &0x000000ff;
	header[24] = (height >> 16) &0x000000ff;
	header[25] = (height >> 24) &0x000000ff;
		
	/* 新建一个文件 */
	bmpres = f_open( &bmpfsrc , (char*)filename, FA_CREATE_ALWAYS | FA_WRITE );
	
	/* 新建文件之后要先关闭再打开才能写入 */
	f_close(&bmpfsrc);
		
	bmpres = f_open( &bmpfsrc , (char*)filename,  FA_OPEN_EXISTING | FA_WRITE);

	if ( bmpres == FR_OK )
	{    
		/* 将预先定义好的bmp头部信息写进文件里面 */
		bmpres = f_write(&bmpfsrc, header,sizeof(unsigned char)*54, &mybw);		
			
		ucAlign = Width % 4;
		
		for(i=0; i<Height; i++)					
		{
			for(j=0; j<Width; j++)  
			{					
				read_data = ILI9341_GetPointPixel ( x + j, y + Height - 1 - i );
				//read_data = ILI9341_GetPointPixel ( x + j, y + i );
				
				r =  GETR_FROM_RGB16(read_data);
				g =  GETG_FROM_RGB16(read_data);
				b =  GETB_FROM_RGB16(read_data);

				bmpres = f_write(&bmpfsrc, &b,sizeof(unsigned char), &mybw);
				bmpres = f_write(&bmpfsrc, &g,sizeof(unsigned char), &mybw);
				bmpres = f_write(&bmpfsrc, &r,sizeof(unsigned char), &mybw);
			}
				
			if( ucAlign )				/* 如果不是4字节对齐 */
				bmpres = f_write ( & bmpfsrc, kk, sizeof(unsigned char) * ( ucAlign ), & mybw );

		}/* 截屏完毕 */

		f_close(&bmpfsrc); 
		
		return 0;
		
	}	
	else/* 截屏失败 */
		return -1;

}


/*-------------------------------------
Manfredi
*/



int Camera_Shot( uint16_t x, uint16_t y, uint16_t Width, uint16_t Height, char * filename)
{
	/* bmp  文件头 54个字节 */
	unsigned char header[54] =
	{
		0x42, 0x4d, 0, 0, 0, 0, 
		0, 0, 0, 0, 54, 0, 
		0, 0, 40,0, 0, 0, 
		0, 0, 0, 0, 0, 0, 
		0, 0, 1, 0, 24, 0, 
		0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 
		0, 0, 0
	};
	
	int i;
	int j;
	long file_size;     
	long width;
	long height;
	unsigned char r,g,b;	
	unsigned int mybw;
	unsigned int read_data;
	char kk[4]={0,0,0,0};
	
	uint8_t ucAlign;//
	
	
	uint16_t Camera_Data;
	
	
	/* 宽*高 +补充的字节 + 头部信息 */
	file_size = (long)Width * (long)Height * 3 + Height*(Width%4) + 54;		

	/* 文件大小 4个字节 */
	header[2] = (unsigned char)(file_size &0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;
	
	/* 位图宽 4个字节 */
	width=Width;	
	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) &0x000000ff;
	header[20] = (width >> 16) &0x000000ff;
	header[21] = (width >> 24) &0x000000ff;
	
	/* 位图高 4个字节 */
	height = Height;
	header[22] = height &0x000000ff;
	header[23] = (height >> 8) &0x000000ff;
	header[24] = (height >> 16) &0x000000ff;
	header[25] = (height >> 24) &0x000000ff;
		
	/* 新建一个文件 */
	bmpres = f_open( &bmpfsrc , (char*)filename, FA_CREATE_ALWAYS | FA_WRITE );
	
	/* 新建文件之后要先关闭再打开才能写入 */
	f_close(&bmpfsrc);
		
	bmpres = f_open( &bmpfsrc , (char*)filename,  FA_OPEN_EXISTING | FA_WRITE);

	if ( bmpres == FR_OK )
	{    
		/* 将预先定义好的bmp头部信息写进文件里面 */
		bmpres = f_write(&bmpfsrc, header,sizeof(unsigned char)*54, &mybw);		
			
		ucAlign = Width % 4;
		
		for(i=0; i<Height; i++)					
		{
			for(j=0; j<Width; j++)  
			{					
				//read_data = ILI9341_GetPointPixel ( x + j, y + Height - 1 - i );					
				
				READ_FIFO_PIXEL(Camera_Data);
				
				r =  GETR_FROM_RGB16(Camera_Data);
				g =  GETG_FROM_RGB16(Camera_Data);
				b =  GETB_FROM_RGB16(Camera_Data);

				bmpres = f_write(&bmpfsrc, &b,sizeof(unsigned char), &mybw);
				bmpres = f_write(&bmpfsrc, &g,sizeof(unsigned char), &mybw);
				bmpres = f_write(&bmpfsrc, &r,sizeof(unsigned char), &mybw);
			}
				
			if( ucAlign )				/* 如果不是4字节对齐 */
				bmpres = f_write ( & bmpfsrc, kk, sizeof(unsigned char) * ( ucAlign ), & mybw );

		}/* 截屏完毕 */

		f_close(&bmpfsrc); 
		
		return 0;
		
	}	
	else/* 截屏失败 */
		return -1;

}


void Fast_difference(char * pic1_name, char * pic2_name, char * filedst)
{
int i, j, k;
	int width, height, l_width;

	BYTE red1,red2,green1,green2,blue1,blue2;
	BITMAPFILEHEADER bitHead1;
	BITMAPINFOHEADER bitInfoHead1;
	WORD fileType;
	
	

	unsigned int read_num;
	
	uint16_t Camera_Data;
	

	/*
	first pixel
	*/

	bmpres = f_open( &bmpfsrc1 , (char *)pic1_name, FA_OPEN_EXISTING | FA_READ);	
/*-------------------------------------------------------------------------------------------------------*/
	if(bmpres == FR_OK)
	{
		BMP_DEBUG_PRINTF("打开文件成功\r\n");
      
		f_read(&bmpfsrc1,&fileType,sizeof(WORD),&read_num);     

		if(fileType != 0x4d42)
		{
			BMP_DEBUG_PRINTF("这不是一个 .bmp 文件!\r\n");
			return;
		}
		else
		{
			BMP_DEBUG_PRINTF("这是一个 .bmp 文件\r\n");	
		}        

		f_read(&bmpfsrc1,&bitHead1,sizeof(tagBITMAPFILEHEADER),&read_num);        
		showBmpHead(&bitHead1);

		f_read(&bmpfsrc1,&bitInfoHead1,sizeof(BITMAPINFOHEADER),&read_num);        
		showBmpInforHead(&bitInfoHead1);
	}    
	else
	{
		BMP_DEBUG_PRINTF("打开文件失败!错误代码：bmpres = %d \r\n",bmpres);
		return;
	}    
	
/*-------------------------------------------------------------------------------------------------------*/
	width = bitInfoHead1.biWidth;
	height = bitInfoHead1.biHeight;

	l_width = WIDTHBYTES(width* bitInfoHead1.biBitCount);	

	if(l_width > 960)
	{
		BMP_DEBUG_PRINTF("\n 本图片太大，无法在液晶屏上显示 (<=320)\n");
		return;
	}
	
	/*
	destination file
	*/
	
	unsigned char header[54] =
	{
		0x42, 0x4d, 0, 0, 0, 0, 
		0, 0, 0, 0, 54, 0, 
		0, 0, 40,0, 0, 0, 
		0, 0, 0, 0, 0, 0, 
		0, 0, 1, 0, 24, 0, 
		0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 
		0, 0, 0
	};
	
	long file_size;     
	BYTE r,g,b,average;	
	unsigned int /*mybw,*/mybw2;
	//unsigned int read_data;
	char kk[4]={0,0,0,0};
	
	uint8_t ucAlign;//
	
	//uint32_t writedata;
	char dataline[1024]={0};
	uint8_t bwbyte;
	
	
	file_size = (long)width * (long)height * 3 + height*(width%4) + 54;		

	header[2] = (unsigned char)(file_size &0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;
		
	header[18] = (long)width & 0x000000ff;
	header[19] = ((long)width >> 8) &0x000000ff;
	header[20] = ((long)width >> 16) &0x000000ff;
	header[21] = ((long)width >> 24) &0x000000ff;
	
	header[22] = (long)height &0x000000ff;
	header[23] = ((long)height >> 8) &0x000000ff;
	header[24] = ((long)height >> 16) &0x000000ff;
	header[25] = ((long)height >> 24) &0x000000ff;
	
	/*
	bmpres = f_open( &bmpfsrc , (char*)filedst, FA_CREATE_ALWAYS | FA_WRITE );
	
	f_close(&bmpfsrc);
		
	bmpres = f_open( &bmpfsrc , (char*)filedst,  FA_OPEN_EXISTING | FA_WRITE);

	if ( bmpres == FR_OK )
	{    
		bmpres = f_write(&bmpfsrc, header,sizeof(unsigned char)*54, &mybw);		
			
		ucAlign = width % 4;
		
	}	
	else  // 截屏失败
		return;
	*/
	
	bmpres = f_open( &bmpfsrc2 , (char*)pic2_name, FA_CREATE_ALWAYS | FA_WRITE );
	
	f_close(&bmpfsrc2);
		
	bmpres = f_open( &bmpfsrc2 , (char*)pic2_name,  FA_OPEN_EXISTING | FA_WRITE);

	if ( bmpres == FR_OK )
	{    
		bmpres = f_write(&bmpfsrc2, header,sizeof(unsigned char)*54, &mybw2);		
			
		ucAlign = width % 4;
		
	}	
	else/* 截屏失败 */
		return;
	
	/* 开一个图片大小的窗口*/
//	ILI9341_OpenWindow(x, y, width, height);
//	ILI9341_Write_Cmd (CMD_SetPixel ); 

	bwbyte=0;
	char rest;
	
	/* 判断是否是24bit真彩色图 */
	if (bitInfoHead1.biBitCount >= 24)
	{
		ILI9341_OpenWindow(0, 0, width, height);
		ILI9341_Write_Cmd (CMD_SetPixel ); 
		
		white_cntr=0;
		
		for ( i = 0; i < height; i ++ )
		{
      //f_lseek ( & bmpfsrc1, bitHead1 .bfOffBits + ( height - i - 1 ) * l_width );
			f_lseek ( & bmpfsrc1, bitHead1 .bfOffBits + ( i + 1 ) * l_width );
			
			f_read ( & bmpfsrc1, pColorData1, l_width, & read_num );					

			for(j=0; j<width; j++) 											   //一行有效信息
			{
				rest = j%8;
				
				if (rest==0) {bwbyte=0;}
				
				k = j*3;																	 //一行中第K个像素的起点
				red1 = pColorData1[k+2];
				green1 = pColorData1[k+1];
				blue1 = 	pColorData1[k];
				
				READ_FIFO_PIXEL(Camera_Data);
				
				red2 =  GETR_FROM_RGB16(Camera_Data);
				green2 =  GETG_FROM_RGB16(Camera_Data);
				blue2 =  GETB_FROM_RGB16(Camera_Data);
				
				r =  abs(red1-red2);
				g =  abs(green1-green2);
				b =  abs(blue1-blue2);
				
				average = (r+g+b)/3;
				
				if ((average>40)&&((cnvdetected==0)||((mask[i][(j-rest)/8]&(128>>rest)))))
				//if (average>35)
				{
					//bwbit=1;
					
					bwbyte = bwbyte|(128>>rest);
					white_cntr++;

				}
				//else
					//bwbit=0;
				
				if (rest==7)
				{
					bwdiff[i][(j-7)/8]=bwbyte;   //baseimg[i][(j-7)/8]=130;	
					//bwdiff[i][(j-7)/8]|=bwbyte; 				
				}
				
				/*
				bmpres = f_write(&bmpfsrc, &b,sizeof(unsigned char), &mybw);
				bmpres = f_write(&bmpfsrc, &g,sizeof(unsigned char), &mybw);
				bmpres = f_write(&bmpfsrc, &r,sizeof(unsigned char), &mybw);
				*/
				
				//writedata = (uint32_t)red2*65536 | (uint32_t)green2*256 | (uint32_t)blue2;
				dataline[j*3]=blue2;
				dataline[j*3+1]=green2;
				dataline[j*3+2]=red2;
				
				/*
				bmpres = f_write(&bmpfsrc2, &blue2,sizeof(uint8_t), &mybw2);
				bmpres = f_write(&bmpfsrc2, &green2,sizeof(unsigned char), &mybw2);
				bmpres = f_write(&bmpfsrc2, &red2,sizeof(unsigned char), &mybw2);
				*/
				
				//bmpres = f_write(&bmpfsrc2, &writedata,3, &mybw2);
							
				
				//ILI9341_Write_Data ( RGB24TORGB16 ( average, average, average ) );   //ILI9341_Write_Data ( RGB24TORGB16 ( r, g, b ) );
				
			} 

				bmpres = f_write(&bmpfsrc2, &dataline,960, &mybw2);
				
			
			if( ucAlign )				/* 如果不是4字节对齐 */
				bmpres = f_write ( & bmpfsrc2, kk, sizeof(unsigned char) * ( ucAlign ), & mybw2 );
		}        		
	}    	
	else 
	{        
		BMP_DEBUG_PRINTF("这不是一个24位真彩色BMP文件！");
		return ;
	}
	
	f_close(&bmpfsrc1);  
	f_close(&bmpfsrc2);
	//f_close(&bmpfsrc); 
	
}



/*
char bw_extract_bool(char matrix, uint16_t h_coor, uint16_t b_coor)
{
	// 1 bwdiff, 2 baseimg, 3 mask
	
	char rest = b_coor%8;
	char byte;

	if (matrix==1)
	{
		byte=bwdiff[h_coor][(b_coor-rest)/8];
		if (byte&(128>>rest))
			return 1;
		else
			return 0;
	}
	else if (matrix==2)
	{
		byte=baseimg[h_coor][(b_coor-rest)/8];
		if (byte&(128>>rest))
			return 1;
		else
			return 0;
	}
	else if (matrix==3)
	{
		byte=mask[h_coor][(b_coor-rest)/8];
		if (byte&(128>>rest))
			return 1;
		else
			return 0;
	}
}
*/

static short Bbox[4];

void findstreaks(void)
{
	
	white_cntr=0;
	char rem;
	for (short i=0; i<Hsize; i++)
	{
		for (short j=0; j<Bsize/8; j++)
		{
			rem=j%8;
			if (bwdiff[i][(j-rem)/8]&(128>>rem))
				white_cntr++;
		}
	}
	
	char phrase[40];
	uint32_t tempo;
	tempo=white_cntr;
	
	sprintf(phrase,"value: %d",tempo);
	
	ILI9341_DispStringLine_EN(LINE(0),phrase);
	
	if (white_cntr<1500)
	{
		
	clearstreaks();
	
	for (int j=0; j<(Bsize/8); j++)
	{
		for (int i=0; i<Hsize; i++)
		{
			mask[i][j]=baseimg[i][j]|bwdiff[i][j];                      //BBBBBBBBBBBBBBBBBBBBBBBBBb
		}
	}
	mask[0][0]&=127;
	
	//char pixel;
	
	//booleans
	char repeat=0;
	char connex=1;
	
	//tmp
	int a1,b1,a2,b2;
	int amin,amax,bmin,bmax;
	short length1, index1, index2, maxindex;//, index0;
	length1=1;
	index0=0;

	
	//raw info
	short start[2];
	//short fnish[2];
	//short Bbox[4];
	long centroid[2];
	short area;
	
	//refined info
	short diagonal;
	short geocenter[2];
	short centerdif[2];
	char direct;
	double cosa,sina;
	double axedistance[2];
	
	char rest;
	
	int seeker[8][2]={{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1}};
	
	bwdisplay1();
	
	for (short i=0; i<Hsize; i++)
	{
		for (short j=0; j<Bsize; j++)
		{			
			//pixel = bw_extract_bool(3, i, j);
			
			rest=j%8;
			
			//(mask[i][(j-rest)/8]&(128>>rest))
			char byte;
			byte=mask[i][(j-rest)/8];
			
			if (byte&(128>>rest))
			{			
				//LED_BLUE;
				
				//initialize raw info
				start[0]=i;
				start[1]=j;
				
				amin=240;
				amax=0;
				bmin=320;
				bmax=0;
				/*
				Bbox[0]=i;
				Bbox[1]=j;
				Bbox[2]=0;
				Bbox[3]=0;
				*/
				centroid[0]=i;
				centroid[1]=j;
				
				area=1;
				
				list1[0][0]=i;
				list1[0][1]=j;
				mask[i][(j-rest)/8]&=(~(128>>rest));
				
				length1=1;
				index1=0;
				index2=0;

				//bwdisplay1();
				
				connex=1;
				
				while (connex)              
				{
					//bwdisplay1();
					connex=0;
					//while (((list1[index1][0]>0)||(list1[index1][1]>0))&&(index1<500)&&(index2<500))
					for (int ll=0; ll<fmin(length1,500); ll++)
					{					
						//bwdisplay1();
						
						a1 = list1[index1][0];
						b1 = list1[index1][1];
						for (char kk=0; kk<8; kk++)
						{
							a2=a1+seeker[kk][0];
							b2=b1+seeker[kk][1];
							if ((a2>=0)&&(b2>=0)&&(a2<Hsize)&&(b2<Bsize))
							{
								
								char byte2;
								char rest2;
								rest2=b2%8;
								byte2=mask[a2][(b2-rest2)/8];
								//if (mask[a2][b2])
								//if(bw_extract_bool(3, b2, a2))
								if (byte2&(128>>rest2))
								{								
									connex=1;
									
									maxindex=fmax(length1,index2);
									for (int qq=0; qq<maxindex; qq++)      //check if non-null neighbour is already in present(1) or future(2) list
									{
										if (((list1[qq][0]==a2)&&(list1[qq][1]==b2))||((list2[qq][0]==a2)&&(list2[qq][1]==b2)))
										{
											repeat=1;
											LED_YELLOW;
											break;
										}
										
										if (qq==maxindex-1)
										{
											LED_BLUE;
										}
									}

									if (repeat==0)
									{
										list2[index2][0]=a2;
										list2[index2][1]=b2;
										
										index2++;
										
										// info mining
										if (a2<amin)
											{amin=a2;}
										if (b2<bmin)
											{bmin=b2;}
										if (a2>amax)
											{amax=a2;}
										if (b2>bmax)
											{bmax=b2;}
										centroid[0]+=a2;
										centroid[1]+=b2;
										area++;
										
									}
									repeat=0;							
								}	
							}
						}
						char rest1;
						rest1=b1%8;
						
						mask[a1][(b1-rest1)/8]&=(~(128>>rest1));
						index1++;
					}
					index1=0;
					
					//update_list1(&(*(*list1)), &(*(*list2)));
					//LED_CYAN;
					for (short cc=0; cc<500; cc++)              //present list(1) updated with future(2) values, future list is reset
					{
						list1[cc][0]=list2[cc][0];
						list1[cc][1]=list2[cc][1];
						list2[cc][0]=0;
						list2[cc][1]=0;
					}
					length1=index2;
					index2=0;
					
					LED_GREEN;
				}
				
				LED_RED;
				
				
				uint32_t time=1;//2000000;
				while(time--)
					LED_RED;
				
				Bbox[0]=bmin;
				Bbox[1]=amin;
				Bbox[2]=bmax-bmin+1;
				Bbox[3]=amax-amin+1;
				
				diagonal=sqrt(Bbox[2]*Bbox[2]+Bbox[3]*Bbox[3]);
				
				if (abs(start[1]-bmin)>abs(start[1]-bmax))
					{
					direct=2;
					Bbox[0]+=area/diagonal;
					Bbox[2]-=area/diagonal;
					}
				else
					{
					direct=0;
					Bbox[2]-=area/diagonal;
					}
				
				if (area>40)//((area>40)&&(Bbox[3]/Bbox[2]>1.732051))
				{
					//info processing
				
					/*
					if (Bbox[3]/Bbox[2]>20){}
					*/
					
					
						
					centroid[0]/=area;
					centroid[1]/=area;
					
					geocenter[1]=Bbox[0]+Bbox[2]/2;
					geocenter[0]=Bbox[1]+Bbox[3]/2;
					centerdif[0]=centroid[0]-geocenter[0];
					centerdif[1]=centroid[1]-geocenter[1];
					cosa=(double)(direct-1)*Bbox[2]/diagonal;
					sina=(double)Bbox[3]/diagonal;
					axedistance[0]=cosa*centerdif[0]+sina*centerdif[1];
					axedistance[1]=(-1)*sina*centerdif[0]+sina*centerdif[1];
					
					if ((index0<39)&&(area/diagonal<(diagonal/20))&&(fabs(axedistance[1])<(area/diagonal))&&(fabs(axedistance[0])<(diagonal/10)))
					{
						double tempo1, tempo2;
						tempo1=(double)Bbox[3];
						tempo2=(double)Bbox[2];
						streak_list[index0][0]=tempo1/tempo2*(1-direct);
						streak_list[index0][1]=(geocenter[0]-streak_list[index0][0]*geocenter[1]);
						
						ILI9341_DrawRectangle ( (uint16_t) centroid[1]-1, (uint16_t) centroid[0]-1, 3, 3, 1 );						
						ILI9341_DrawRectangle ( (uint16_t) geocenter[1]-3, (uint16_t) geocenter[0]-3, 5, 5, 0 );
						
						index0++;
					}
				}
			}
		}
		if ((index0>=index0_past))
		{
			index0_past=index0;
			
			for (int j=0; j<(Bsize/8); j++)
			{
				for (int i=0; i<Hsize; i++)
				{
					baseimg[i][j]|=bwdiff[i][j];                    //BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
				}
			}
			
		}
		
		drawstreaks();
	}
	
	}
	/*
	bwdisplay0();
	
	uint32_t time=2000000;
	while(time--)
		{LED_PURPLE;}
	*/	
}

void clearstreaks(void)
{
	for (int i=0; i<40; i++)
	{
		/*
		if ((streak_list[i][0]==0)&&(streak_list[i][1]==0))
			{break;}
		*/
		streak_list[i][0]=0;
		streak_list[i][1]=0;
	}
}

void drawstreaks(void)
{
	//ILI9341_DrawLine( 50, 100, 20, 130);
	
	uint16_t usX1;
	uint16_t usY1;
	uint16_t usX2;
	uint16_t usY2;
	double tmp1, tmp2;
	char phrase[40];
	
	uint32_t tempo;
	tempo=white_cntr;
	
	sprintf(phrase,"value: %d",tempo);
	
	//ILI9341_DispStringLine_EN(LINE(0),phrase);
	
	/*
	if (index0)
	{
		ILI9341_DispStringLine_EN(LINE(0),phrase);
		ILI9341_DrawLine( 0, 0, 320, 240 );
		ILI9341_DrawLine( 0, 0, 220, 240 );
		ILI9341_DrawLine( 0, 0, 120, 240 );
	}
	*/
	
	for (int i=0; i<40; i++)
	{
		/*
		if ((streak_list[i][0]==0)&&(streak_list[i][1]==0))  // no use to read and draw null lines
		{
			uint32_t time=2000000;
			while(time--)
			{
				LED_BLUE;
			}
			break;
		}
		*/
		
		usY1 = 0;
		usY2 = Hsize;
		
		tmp1 = ((usY1-streak_list[i][1])/streak_list[i][0])/1;
		tmp2 = ((usY2-streak_list[i][1])/streak_list[i][0])/1;
		
		// point1
		
		if ((tmp1>=0)&&(tmp1<320))
			usX1 = (uint16_t)tmp1;
		else if (tmp1<0)
		{
			usX1=0;
			usY1=(uint16_t)streak_list[i][1];
		}
		else if (tmp1>=320)
		{
			usX1=319;
			usY1=(uint16_t)(usX1*streak_list[i][0]+streak_list[i][1]);
		}
		
		// point2
		
		if ((tmp2>=0)&&(tmp2<320))
			usX2 = (uint16_t)tmp2;
		else if (tmp2<0)
		{
			usX2=0;
			usY2=(uint16_t)streak_list[i][1];
		}
		else if (tmp2>=320)
		{
			usX2=319;
			usY2=(uint16_t)(usX2*streak_list[i][0]+streak_list[i][1]);
		}
		
		// draw
		
		ILI9341_DrawLine( usX1, usY1, usX2, usY2 );
	}
	
	sprintf(phrase,"0:gray_%d.bmp",usX2);

	//ILI9341_DispStringLine_EN(LINE(0),phrase);
}

void intersections(void) //computes the value of the CpH
{
	short int_n = (index0_past-1)*index0_past/2;
	short intersect[int_n][2]; //to be pre-assigned (38*39/2 = 741x2)if possible
	short X, Y;
	short counter=0;
	for (char i=0; i<index0_past; i++)   //index0_past is the (last-1) index
	{
		for (char j=i+1; j<index0; j++)    //index0 = index0_past+1
		{		
			X = (streak_list[j][1]-streak_list[i][1])/(streak_list[i][0]-streak_list[j][0]);
			Y = streak_list[i][0]*X + streak_list[i][1];
			intersect[counter][0]=X;
			intersect[counter][1]=Y;
			
			counter++;
		}
	}

	double minD = 2000000000;//__DBL_MAX__;
	short minDindex=counter;
	double distance=0;
	//minD|=(4294967295|(4294967295<<32)); //infinity
	
	short Xdif;
	short Ydif;
	
	for (short i=0; i<counter; i++)
	{
		for (short j=0; j<counter; j++)
		{
			if (i-j)
			{
				Xdif=intersect[i][0]-intersect[j][0];
				Ydif=intersect[i][1]-intersect[j][1];
				distance+= sqrt(sqrt(sqrt(sqrt(sqrt(Xdif*Xdif+Ydif*Ydif)))));
			}
		}
		if (distance<minD)
		{
			minD=distance;
			minDindex=i;
		}
		distance=0;
	}
	CpH[0]=intersect[minDindex][0];
	CpH[1]=intersect[minDindex][1];
	
	//ILI9341_DrawRectangle ( (uint16_t) CpH[0]-2, (uint16_t) CpH[1]-2, 5, 5, 1 );
}

void cnvyrbase(void)
{
	left=Bsize;
	right=0;
	char hstart;
	hstart=9*(Hsize-Hsize%10)/10;
	char rem;
	
	for (short i=hstart;i<Hsize;i++)
	{
		for (short j=0;j<Bsize;j++)
		{
			rem=j%8;
			if ((baseimg[i][(j-rem)/8]&(128>>rem)))                       //BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
			{
				if (j<left)
					left=j;
				if (j>right)
					right=j;
			}
				
		}
	}
}

void whitensector(double Xa, double Ya, double Xb, double Yb)
{
	char rem;
	double Aa, Ab, Ba, Bb;
	double Aaplus, Abplus;
	Aa=((double)(Hsize-CpH[1]-Ya))/((double)CpH[0]-Xa);
	Ba=Hsize-CpH[1]-CpH[0]*Aa;
	Ab=((double)(Hsize-CpH[1]-Yb))/((double)CpH[0]-Xb);
	Bb=Hsize-CpH[1]-CpH[0]*Ab;
	Aaplus=fabs(Aa);
	Abplus=fabs(Ab);
	
	for (short i=0; i<Hsize; i++)
	{
		for (short j=0; j<Bsize; j++)
		{
			if ((Aa/Aaplus*((Hsize-i)-(Aa*j+Ba))<=0)&&(Ab/Abplus*((Hsize-i)-(Ab*j+Bb))>=0))
			{
				rem=j%8;
				mask[i][(j-rem)/8]|=(128>>rem);
			}
			
		}
	}
}


void update_list1(uint16_t*list1, uint16_t *list2)
	{
  uint16_t *temp = list1;
  *list1 = *list2;
  list2 = temp;
	}
	

void bwdisplay(void)
{
	uint8_t bytedisplay;
	
	ILI9341_OpenWindow(0, 0, 320, 240);
	ILI9341_Write_Cmd (CMD_SetPixel ); 
	for (int i=0; i<240; i++)
	{
		for (int j=0; j<40; j++)
		{		
			bytedisplay = (uint32_t)bwdiff[i][j];
			for (int k=0; k<8; k++)
			{
				if (bytedisplay&0x80)
					ILI9341_Write_Data ( RGB24TORGB16 ( 255, 255, 255 ) );
				else
					//ILI9341_Write_Data ( RGB24TORGB16 ( 0, 0, 50 ) );
					ILI9341_Write_Data ( RGB24TORGB16 ( 0, 0, 0 ) );
				
			  bytedisplay=(uint32_t)(bytedisplay<<1);
			}
			
		}
	}
}


void bwdisplay0(void)
{
	uint8_t bytedisplay;
	
	ILI9341_OpenWindow(0, 0, 320, 240);
	ILI9341_Write_Cmd (CMD_SetPixel ); 
	for (int i=0; i<240; i++)
	{
		for (int j=0; j<40; j++)
		{		
			bytedisplay = (uint32_t)baseimg[i][j];                 // BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
			for (int k=0; k<8; k++)
			{
				if (bytedisplay&0x80)
					ILI9341_Write_Data ( RGB24TORGB16 ( 255, 255, 255 ) );
				else
					ILI9341_Write_Data ( RGB24TORGB16 ( 0, 0, 50 ) );
				
			  bytedisplay=(uint32_t)(bytedisplay<<1);
			}
			
		}
	}
}

void bwdisplay1(void)
{
	uint8_t bytedisplay;
	
	ILI9341_OpenWindow(0, 0, 320, 240);
	ILI9341_Write_Cmd (CMD_SetPixel ); 
	for (int i=0; i<240; i++)
	{
		for (int j=0; j<40; j++)
		{		
			bytedisplay = (uint32_t)mask[i][j];
			for (int k=0; k<8; k++)
			{
				if (bytedisplay&0x80)
					ILI9341_Write_Data ( RGB24TORGB16 ( 255, 255, 255 ) );
				else
					ILI9341_Write_Data ( RGB24TORGB16 ( 0, 0, 50 ) );
				
			  bytedisplay=(uint32_t)(bytedisplay<<1);
			}
			
		}
	}
}

void bwfill(void)
{
	char rem;
	char byte_n;
	for (int i=0; i<240; i++)
	{
		for (int j=0; j<320; j++)
		{
			rem=j%8;
			byte_n=(j-rem)/8;
			
			if((fabs(2.0*j+i-250)<2)||((2.0*j-i<300)&&(2.0*j-i>297)))
				bwdiff[i][byte_n]|=(128>>rem);
			else
				bwdiff[i][byte_n]&=(~(128>>rem));
		}
	}
}


void basevoid(void)
{
	for (int i=0; i<240; i++)
	{
		for (int j=0; j<40; j++)
		{
			baseimg[i][j]=0;                    // BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
		}
	}
}

void maskvoid(void)
{
	for (int i=0; i<240; i++)
	{
		for (int j=0; j<40; j++)
		{
			mask[i][j]=0;                    // BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
		}
	}
}


void border_det(void)
{
	int i, j, kk, kkk, ii, iii, JJ, jj, jjj;
	short indi, indj;
	short a1, a2, counter, doub;
	
	int boole;

	BYTE red,green,blue;	
	
	uint16_t Camera_Data;
	
	char phrase[10];
	
	maskvoid();
	
	//int8_t index_i[8]={-1,-1,-1,0,1,1, 1, 0};
	//int8_t index_j[8]={-1, 0, 1,1,1,0,-1,-1};
	
	char lele=2;

	//int8_t index_i[4]={-1,0,1, 0};
	//int8_t index_j[4]={ 0,1,0,-1};
	
	//int8_t index_i[4]={-1,-1,0,1};
	//int8_t index_j[4]={ 0, 1,1,1};
	
	//int8_t index_i[2]={-1,0};
	//int8_t index_j[2]={ 0,1};

	int8_t index_i[2]={-2,0};
	int8_t index_j[2]={ 0,2};
	
	uint8_t bwbyte;
	bwbyte=0;
	char rest;
	
	ILI9341_OpenWindow(0, 0, 320, 240);
	ILI9341_Write_Cmd (CMD_SetPixel ); 
	
	white_cntr=0;
	

	
	for (i=0; i<3; i++)
	{
		for (j=0; j<Bsize; j++)
		{
			READ_FIFO_PIXEL(Camera_Data);
		
			red =  GETR_FROM_RGB16(Camera_Data);
			green =  GETG_FROM_RGB16(Camera_Data);
			blue =  GETB_FROM_RGB16(Camera_Data);
			
			jj=j*3;
			
			buffer[i][jj] = red;
			buffer[i][jj+1] = green;
			buffer[i][jj+2] = blue;
		}
	}
	
	JJ=0;
	for (j=0; j<Bsize; j++)
	{
		rest=j%8;
		a1=0;
		counter=0;
		
		jj=j*3;
		
		//for (kk=3; kk<lele; kk++)
		
	
		for (kk=1; kk<lele; kk++)
		{
			indi=index_i[kk];
			indj=j+index_j[kk];
			jjj=indj*3;
			boole=(indi+1)*(indj+1)*(Hsize-indi)*(Bsize-indj);
			if (boole)
			{
				a2=0;
				for (kkk=0; kkk<3; kkk++)
				{
					doub=buffer[0][jj+kkk]-buffer[indi][jjj+kkk];
					a2+=abs(doub); 
				}
				//a1+=(uint8_t)(a2/3);
				a1+=a2;
				counter=counter+1;
			}
		}
		counter*=3;
		a1/=counter;
		
		if (a1>40)
		{					
			bwbyte = bwbyte|(128>>rest);
			white_cntr++;
		}
		
		if (rest==7)
		{
			//bwdiff[0][(j-7)/8]=bwbyte;
			bwdiff[0][JJ]=bwbyte;
			bwbyte=0;
			JJ++;
		}
	}
	
	int number;
	int maxnumber=0;
	uint8_t lng;
	char whiteon;
	uint8_t pos;
	char empty_slot=8;//was 40
	char activ_slot;
	uint8_t partners;
	uint8_t last_partner;
	char left_merger;
	uint8_t stored=1;
	
	uint8_t line_start;
	uint8_t q, qq, tmp;
	
	connect=2;
	sprout=5;
	merge=6;
	height=7;
	
	char start=4;
	
	char see_matrix=0;
	char draw=1;
	char block=1;
	
	ii=0;
	
	//for (i=1; i<240-2+3; i++)
	//	{mask[stored][10]=0;}
	
	for (i=1; i<240-2+3; i++)
	{	
		
		//mask[stored][10]=0;
		
		ii++;
		iii=ii;
		if (ii==3)
			ii=0;
		
		JJ=0;
		
		number=0;
		whiteon=0;
		
		lng=0;
		
		for(j=0; j<320; j++) 										
		{
			rest = j%8;
			
			jj=j*3;
		
			a1=0;
			counter=0;
			
			last_partner=0;
			
			for (kk=0; kk<lele; kk++)
			{
				indi=(iii+1+index_i[kk])%3;    //
				//indi=iii+index_i[kk];  if(indi==3){indi=0;}
				indj=j+index_j[kk];
				jjj=3*indj;
				boole=(indi+1)*(indj+1)*(Hsize-indi)*(Bsize-indj);
				if (boole)
				{
					a2=0;
					for (kkk=0; kkk<3; kkk++)
					{
						doub=buffer[ii][jj+kkk]-buffer[indi][jjj+kkk];
						a2+=abs(doub); 
					}
					//a1+=(uint8_t)(a2/3);
					a1+=a2;
					counter=counter+1;
				}
			}
			//counter*=3;
			a1/=counter;
			
			if ((a1>20)&&(j<255)&&(i<239))//(Camera_Data%7==1)//
			{					
				bwbyte = bwbyte|(128>>rest);
				white_cntr++;
				ILI9341_Write_Data ( RGB24TORGB16 ( 255, 255, 255 ) );
				lng++;
				
				if (whiteon==0)
				{
					//number++;
					whiteon=1;
					line_start=j;					
				}
			}
			else
			{
				ILI9341_Write_Data ( RGB24TORGB16 ( 0,0,0 ));
				
				if (whiteon==1)
				{
					whiteon=0;
					
					partners=0;
					///*
					if (lng>=2)
					{
						//bwdisplay1();
						for (q=start; q<20; q++)
						{
							qq=q<<1;
							pos=mask[height][qq+1];
							if ((mask[0][qq])&&(line_start<=mask[pos][qq+1]-1)&&(j>=mask[pos][qq]+1))
							{
								partners++;
								/*
								if (mask[connect][qq])
								{																
									if (mask[sprout][qq+1]==0)
									{
										store_body(stored, qq);
										
										mask[sprout][qq+1]=255;
										
										pos=mask[height][qq+1];

										//prepare_bislot(empty_slot, stored, 0, 0, mask[height][qq]+pos-7, mask[pos][qq], mask[pos][qq+1]);
										prepare_bislot(empty_slot, stored, 0, 0, i, mask[pos][qq], mask[pos][qq+1]);
										
										mask[pos][qq]=0; 
										mask[pos][qq+1]=0;
										mask[height][qq+1]--;
										
										for (p=start; p<20; p++)
										{
											pp=p<<1;
											if (mask[0][pp]==0)
											{
												empty_slot=pp;
												break;
											}
										}
										
										stored++;
										
										//body qq is diverging and needs to be erased
										mask[connect][qq+1]=0;	
										
									}
									
									prepare_bislot(empty_slot, stored-1, 0, 0, i, line_start, j);
									left_merger=empty_slot;
									empty_slot=40;
									
									
								}
								*/
//								else
//								{									
									mask[height][qq+1]++;
									pos=mask[height][qq+1];
									mask[pos][qq]=line_start;
									mask[pos][qq+1]=j;
									
									if ((partners==2)&&(empty_slot<40))
									{									
										if (mask[merge][last_partner])
										{
											//merging left and right mergers when both are present... bad idea
										//	mask[merge][qq]=mask[merge][last_partner];
										//	mask[mask[height][qq+1]][qq]=	mask[mask[height][last_partner+1]][last_partner];  //current merge extended on the left
										//	mask[mask[height][last_partner+1]][last_partner+1]=j;  //previous merge extended on the right
											
											//making the two merges of last partner 'aware' of each other's existence...unnecessary
										//	mask[mask[merge][last_partner]][6]=stored;
										//	mask[stored][5]=mask[merge][last_partner];
										}
										//else
										//{
										
										mask[stored][0]=0;
										mask[stored][1]=0;
										mask[stored][2]=Hsize-i;
										mask[stored][3]=(mask[mask[height][last_partner+1]][last_partner+1]+mask[pos][qq])>>1;		//mask[stored][3]=(line_start+j)>>1;															
										
										//in case this was also a diverging point, 'sprout' value doesn't get stored
										
										if (mask[sprout][last_partner+1]==255)
										{
											mask[stored-1][6]=stored;
										}
										mask[merge][last_partner+1]=stored;   
										mask[merge][qq]=stored;
										
										if (mask[sprout][last_partner+1]==0)
										{
											activ_slot=empty_slot;
											
											//prepare_bislot(activ_slot, stored, 0, 0, i, mask[pos][qq], mask[pos][qq+1]);
											prepare_bislot(activ_slot, stored, 0, 0, i, line_start, j);
											/*
											mask[0][activ_slot]=127;
											mask[0][activ_slot+1]=254;
											mask[1][activ_slot]=127;
											mask[1][activ_slot+1]=254;
											
											mask[connect][activ_slot]=14;
											mask[connect][activ_slot+1]=112;
											
											mask[3][activ_slot]=127;
											mask[3][activ_slot+1]=254;
											mask[4][activ_slot]=127;
											mask[4][activ_slot+1]=254;
											
											mask[sprout][activ_slot]=stored;  // what body do I derive from
											
											mask[merge][activ_slot]=0;  // what body do I merge into (on the left)
											mask[merge][activ_slot+1]=0;  // what body do I merge into (on the right)
											
											mask[height][activ_slot]=Hsize-i;
											mask[height][activ_slot+1]=height+1;
											
											mask[height+1][activ_slot]=line_start;
											mask[height+1][activ_slot+1]=j;
											*/
											empty_slot=40;
										}
										else
										{
											mask[sprout][left_merger]=stored;
										}
										
										stored++;
										
										// last and current partner now need to be erased
										mask[connect][last_partner+1]=0;
										mask[connect][qq+1]=0;
										
										if (see_matrix)
											bwdisplay1();
										//}
									}
									else if (partners>2)
									{
										mask[merge][qq]=stored;  //?//yes
										mask[stored][3]=(mask[mask[height][last_partner+1]][last_partner+1]+mask[pos][qq])>>1;
										mask[stored][3]=((mask[stored][3]<<1)-mask[mask[height][last_partner+1]][last_partner]+mask[pos][qq])>>1; // shifting merging point to the right         
										//possibility of creating a flag to compensate for integer approximation
										
										//current partner needs to be erased
										mask[connect][qq+1]=0;								
									}
									
									mask[connect][qq]=14;
									if ((mask[connect][qq+1]==0)&&(partners==1))
										mask[connect][qq+1]=112;
									//else
									//	mask[2][qq+1]=14; //needed?
//								}
								

								last_partner=qq;
							}
							else if ((mask[0][qq]==0)&&(j<empty_slot))
							{
								empty_slot=qq;
							}
						}
						if ((partners==0)&&(empty_slot<40))
						{
							activ_slot=empty_slot;
							
							prepare_bislot(activ_slot, stored, 0, 0, i, line_start, j);
							/*
							mask[0][activ_slot]=127;
							mask[0][activ_slot+1]=254;
							mask[1][activ_slot]=127;
							mask[1][activ_slot+1]=254;
							
							mask[connect][activ_slot]=14;
							mask[connect][activ_slot+1]=112;
							
							mask[3][activ_slot]=127;
							mask[3][activ_slot+1]=254;
							mask[4][activ_slot]=127;
							mask[4][activ_slot+1]=254;
							
							mask[sprout][activ_slot]=0;  // what body do I derive from
							
							mask[merge][activ_slot]=0;  // what body do I merge into (on the left)
							mask[merge][activ_slot+1]=0;  // what body do I merge into (on the right)
							
							mask[height][activ_slot]=Hsize-i;
							mask[height][activ_slot+1]=height+1;
							
							mask[height+1][activ_slot]=line_start;
							mask[height+1][activ_slot+1]=j;
							*/
							empty_slot=40;
							
							if (see_matrix)
								bwdisplay1();
							
						}
						//bwdisplay1();
					}
					//*/
					if (lng>4)
						number++;
					lng=0;
					///*
					if (empty_slot==40)
					{							
						for (q=start; q<20; q++)
						{
							qq=q<<1;
							if (mask[0][qq]==0)
							{
								empty_slot=qq;
								break;
							}
						}
						if (empty_slot==40)
							break;
						/*
						{
							while (1)
							{
								LED_PURPLE;
								bwdisplay1();
								while(1){}
							}
						}
						*/
					}
					if (stored==230)
						while (1)
						{LED_YELLOW;
							bwdisplay1();
							while(1){}}
					//*/
				}
			}
			
			if (rest==7)
			{
				bwdiff[i][JJ]=bwbyte;
				bwbyte=0;
				JJ++;
			}
		
			//ILI9341_Write_Data ( RGB24TORGB16 ( average, average, average ) );   //ILI9341_Write_Data ( RGB24TORGB16 ( r, g, b ) );
			
			READ_FIFO_PIXEL(Camera_Data);
			
			red =  GETR_FROM_RGB16(Camera_Data);
			green =  GETG_FROM_RGB16(Camera_Data);
			blue =  GETB_FROM_RGB16(Camera_Data);
			
			//buffer[(i-1)%3][j*3]=red;
			buffer[iii-1][jj]=red;
			buffer[iii-1][jj+1]=green;
			buffer[iii-1][jj+2]=blue;
			
		} 
		
		uint8_t col;
		
		//bwdisplay1();
		
		for (q=start; q<20; q++)
		{
			qq=q<<1;
			
			if ((i<250)&&(mask[0][qq+1])&&((mask[connect][qq+1]==0) )) // ||(mask[merge][qq])||(mask[merge][qq+1])))
			{
				///*
				if ((mask[sprout][qq+1]==0)&&((mask[height][qq+1]>12)||(mask[connect][qq])))
				{
					mask[stored][0]=mask[height][qq];
					mask[stored][1]=(mask[height+1][qq]+mask[height+1][qq+1])>>1;
					mask[stored][2]=mask[height][qq]-mask[height][qq+1]+height;
					mask[stored][3]=(mask[mask[height][qq+1]][qq]+mask[mask[height][qq+1]][qq+1])>>1;
					
					mask[stored][4]=mask[sprout][qq];
					mask[stored][5]=mask[merge][qq];
					mask[stored][6]=mask[merge][qq+1];
					
					stored++;
				}
				//*/
				
				col=mask[height][qq+1]; //hoping no conflict for 'pos'

				for (tmp=0; tmp<=col; tmp++)  //not really necessary to erase everything, 'occupy' byte(s) suffice #optimize
				{
					mask[tmp][qq]=0;
					mask[tmp][qq+1]=0;
				}
			}
			mask[connect][qq]=0;
			mask[connect][qq+1]=0;
		}
		
		if (number>maxnumber)
			maxnumber=number;
		
	} 
	
	//bwdisplay1();
	//while(1){}
	
	//ILI9341_DrawLine( 0, 0, 320, 240 );
	
	sprintf(phrase,"objects %d akk",maxnumber);
	
	//ILI9341_DispStringLine_EN(LINE(0),phrase);
	ILI9341_DispStringLine_EN(220,phrase);
	
	if (draw)
	{
		for (tmp=1; tmp<stored; tmp++)
		{
			if (mask[tmp][0])
				ILI9341_DrawLine( mask[tmp][1], Hsize-mask[tmp][0]-1, mask[tmp][3], Hsize-mask[tmp][2]-1 );
				//ILI9341_DrawLine( mask[tmp][1], Hsize-mask[tmp][0]-1, 0, 0 );
		}
	}
	/*
	ILI9341_DrawLine( 255, 0, 255, 240 );
	ILI9341_DrawLine( 0, 113, 320, 113 );
	
	ILI9341_DrawLine( 112, 0, 112, 240 );
	ILI9341_DrawLine( 0, 240-14, 320, 240-14 );
	*/
	if (block)
		while(1){}
}

void prepare_bislot(char slot, uint8_t sprout_b, uint8_t merge_b1, uint8_t merge_b2, uint8_t l_height, uint8_t l_start, uint8_t l_finish)
{
	mask[0][slot]=127;
	mask[0][slot+1]=254;
	mask[1][slot]=127;
	mask[1][slot+1]=254;
	
	mask[connect][slot]=14;
	mask[connect][slot+1]=112;
	
	mask[3][slot]=127;
	mask[3][slot+1]=254;
	mask[4][slot]=127;
	mask[4][slot+1]=254;
	
	mask[sprout][slot]=sprout_b;  // what body do I derive from
	mask[sprout][slot]=0;
	
	mask[merge][slot]=merge_b1;  // what body do I merge into (on the left)
	mask[merge][slot+1]=merge_b2;  // what body do I merge into (on the right)
	
	mask[height][slot]=Hsize-l_height;
	mask[height][slot+1]=height+1;
	
	mask[height+1][slot]=l_start;
	mask[height+1][slot+1]=l_finish;
}



void border_det1(void)
{
	int i, j, k, kk, kkk, ii, iii, JJ, jj, jjj;
	short indi, indj;
	short a1, a2, counter, doub;
	
	int boole;

	BYTE red,green,blue;	
	
	uint16_t Camera_Data;
	
	char phrase[10];
	
	maskvoid();
	
	//int8_t index_i[8]={-1,-1,-1,0,1,1, 1, 0};
	//int8_t index_j[8]={-1, 0, 1,1,1,0,-1,-1};
	
	char lele=2;

	//int8_t index_i[4]={-1,0,1, 0};
	//int8_t index_j[4]={ 0,1,0,-1};
	
	//int8_t index_i[4]={-1,-1,0,1};
	//int8_t index_j[4]={ 0, 1,1,1};
	
	//int8_t index_i[2]={-1,0};
	//int8_t index_j[2]={ 0,1};

	int8_t index_i[2]={-2,0};
	int8_t index_j[2]={ 0,2};
	
	uint8_t bwbyte;
	bwbyte=0;
	char rest;
	
	ILI9341_OpenWindow(0, 0, 320, 240);
	ILI9341_Write_Cmd (CMD_SetPixel ); 
	
	white_cntr=0;
	

	
	for (i=0; i<3; i++)
	{
		for (j=0; j<Bsize; j++)
		{
			READ_FIFO_PIXEL(Camera_Data);
		
			red =  GETR_FROM_RGB16(Camera_Data);
			green =  GETG_FROM_RGB16(Camera_Data);
			blue =  GETB_FROM_RGB16(Camera_Data);
			
			jj=j*3;
			
			buffer[i][jj] = red;
			buffer[i][jj+1] = green;
			buffer[i][jj+2] = blue;
		}
	}
	
	JJ=0;
	for (j=0; j<Bsize; j++)
	{
		rest=j%8;
		a1=0;
		counter=0;
		
		jj=j*3;
		
		//for (kk=3; kk<lele; kk++)
		
	
		for (kk=1; kk<lele; kk++)
		{
			indi=index_i[kk];
			indj=j+index_j[kk];
			jjj=indj*3;
			boole=(indi+1)*(indj+1)*(Hsize-indi)*(Bsize-indj);
			if (boole)
			{
				a2=0;
				for (kkk=0; kkk<3; kkk++)
				{
					doub=buffer[0][jj+kkk]-buffer[indi][jjj+kkk];
					a2+=abs(doub); 
				}
				//a1+=(uint8_t)(a2/3);
				a1+=a2;
				counter=counter+1;
			}
		}
		counter*=3;
		a1/=counter;
		
		if (a1>40)
		{					
			bwbyte = bwbyte|(128>>rest);
			white_cntr++;
		}
		
		if (rest==7)
		{
			//bwdiff[0][(j-7)/8]=bwbyte;
			bwdiff[0][JJ]=bwbyte;
			bwbyte=0;
			JJ++;
		}
	}
	
	int number;
	int maxnumber=0;
	uint8_t lng;
	char whiteon;
	short black=0;
	
	//uint8_t pos;
	uint8_t posQ;
	uint8_t posES;
	uint8_t posLP;
	
	char empty_slot=8;//was 40
	char activ_slot;
	uint8_t partners;
	uint8_t last_partner;
	char left_merger;
	uint8_t stored=8;//1;
	
	mask[0][0]=126; mask[0][1]=126; mask[1][0]=126; mask[1][1]=126;
	
	uint8_t line_start;
	uint8_t q, p, tmp;
	
	connect=2;                            // #0 ON    segment is being continued     #1 OFF   slot was touched by multiple strips (needs to be ERASED)
	sprout=6;                             // #0 VALUE originate from...              #1 ON    slot content / body must not be stored
	merge=8;                              // #0 VALUE back/left merger               #1 VALUE right/front merger
	height=10;                            // #0 VALUE starting ordinate              #1 VALUE position (start) of last added strip
	
	char start=8;
	
	char see_matrix=0;
	char draw=1;
	char block=0;
	
	ii=0;
	
	//for (i=1; i<240-2+3; i++)
	//	{mask[stored][10]=0;}
	
	for (i=1; i<240-2+3; i++)
	{	
		
		//mask[stored][10]=0;
		
		ii++;
		iii=ii;
		if (ii==3)
			ii=0;
		
		JJ=0;
		
		number=0;
		whiteon=0;
		
		lng=0;
		
		//
		//  BEGIN READING THROUGH A LINE OF PIXELS
		//
		
		for(j=0; j<320; j++) 										
		{
			rest = j%8;
			
			jj=j*3;
		
			a1=0;
			counter=0;
			
			last_partner=0;
			
			for (kk=0; kk<lele; kk++)
			{
				indi=(iii+1+index_i[kk])%3;    //
				//indi=iii+index_i[kk];  if(indi==3){indi=0;}
				indj=j+index_j[kk];
				jjj=3*indj;
				boole=(indi+1)*(indj+1)*(Hsize-indi)*(Bsize-indj);
				if (boole)
				{
					a2=0;
					for (kkk=0; kkk<3; kkk++)
					{
						doub=buffer[ii][jj+kkk]-buffer[indi][jjj+kkk];
						a2+=abs(doub); 
					}
					//a1+=(uint8_t)(a2/3);
					a1+=a2;
					counter=counter+1;
				}
			}
			//counter*=3;
			a1/=counter;
			
			if ((a1>20)&&(j<255)&&(i<239))//(Camera_Data%7==1)//
			{					
				bwbyte = bwbyte|(128>>rest);
				white_cntr++;
				ILI9341_Write_Data ( RGB24TORGB16 ( 255, 255, 255 ) );
				lng++;
				
				if (whiteon==0)
				{
					//number++;
					whiteon=1;
					line_start=j;					
				}
			}
			else
			{
				ILI9341_Write_Data ( RGB24TORGB16 ( 0,0,0 ));
				black++;
				
				if (whiteon==1)
				{
					whiteon=0;
					
					partners=0;
					///*
					if (lng>=3)
					{
						//bwdisplay1();
						//my_slot=40;
						for (q=start; q<40; q++)
						{
							posQ=mask[height+1][q];   // NO NEED for a +2 (or whatever) here, because we're reading last turn's strip values (turn i-1) 
							
							// CONDITION: strip overlaps with last stored,
							//            strip's start is not far beyond and stip's end is not far back
							
							if ((mask[0][q])&&(line_start<=mask[posQ+1][q]-1)&&(j>=mask[posQ][q]+1))
							{
								partners++;
//							/*
								
								// DIVERGENCE
								
								// / CONDITION: connect#0 is ON,
								// /            slot was (already) continued this turn
								// /            --> divergence
								
								if (mask[connect][q])
								{
//									if (black>2)
//									{
									
										// / refresh empty slot's value // necessary?
									
										if (empty_slot==40)
										{
											for (p=start; p<40; p++)
											{
												if (mask[1][p]==0)
												{
													empty_slot=p;
													break;
												}
											}								
										}
										
										// / CONDITION: sprout#1 is not ON, 
										// /            (slot q IS NOT going to be erased without transcription) // irrelevant
										// /            sprout#1 flag is activated for diverging bodies, because they have to be stored on the spot and must mot be stored multiple times
										// /            connect#0 ON --> divergence, yet sprout#1 OFF --> first divergence
										// / TRANSLATION: first divergence
										
										if (mask[sprout+1][q]==0)
										{
											
											// / In this IF loop, line_start and j are not used,
											// / we're working on previously stored values to store the divergence correctly:
											
											// / We're just discovering that slotq's body is diverging,
											// / before we thought it was just being added to as a segment,
											// / yet it was not the active tip of the current body, rather the leftmost origin of a divergence
											// / this needs to be corrected
											// / therefore slotq's body tip needs to be removed (?)
											// / and a new slot needs to be created, with that tip's value as a starting point
											
											// / my previous intent was to actually create a slot beginning with the ACTUAL tip (end in this case) of the body
											// / this means creating a slot with the 'true tip's' value (not from j-1, from j-2)
											// / and then adding the 'false tip's' value (from j-2 ; the value that has been (?) removed from slotq before storing it
											
											//store_body(stored, q); // this line can and NEEDS to be moved down,
																						 // slotq's last value needs to be erased
										
											
									
											posQ=mask[height+1][q];  // need to add +2 to go with the centralizing
																						 		 // yet would be smarter to remove both the +2 here and the -2 on the next line // ok let's do this
																								 // increment pos to store new values // WHAT?? no, if anything values need be REMOVED from slotq																								 
											
											
											//prepare_bislot(empty_slot, stored, 0, 0, mask[height][qq]+pos-7, mask[pos][qq], mask[pos][qq+1]);
											//if (empty_slot==40) {while (1) {LED_GREEN;}}               //RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
											//prepare_slot1(empty_slot, stored, 0, 0, i, mask[pos][q], mask[pos+1][q]);
											//prepare_slot1(empty_slot, stored, 0, 0, i-1, mask[posQ-2][q], mask[posQ-1][q]);   // have added a -2 here...
											prepare_slot1(empty_slot, stored, 0, 0, i-1, mask[posQ][q], mask[posQ+1][q]);   // have removed -2
											
											posES=mask[height+1][empty_slot]; // is this the PREVIOUS strip? // yes, slot was just created
											
											//mask[height+4][empty_slot]= mask[pos][q];
											//mask[height+5][empty_slot]= mask[pos+1][q];
											
											//mask[posES+0][empty_slot]= mask[posQ][q];
											//mask[posES+1][empty_slot]= mask[posQ+1][q];
											
											//mask[posES+0][empty_slot]= mask[posQ-2][q]; // readapted to the 'centralizing'//see right below
											//mask[posES+1][empty_slot]= mask[posQ-1][q];
											
											/// copy last value of slot q into newly created slot
											mask[posES+0][empty_slot]= mask[posQ][q]; // moved the +2, therefore readapted
											mask[posES+1][empty_slot]= mask[posQ+1][q];
											
											//mask[height+1][empty_slot]++;                   //      centralizing this process
											//mask[height+1][empty_slot]++;
											
											/*                          not so important
											mask[posQ][q]=0; 
											mask[posQ+1][q]=0;
											*/
											
											//mask[height+1][q]--;                             //     centralizing this process
											//mask[height+1][q]--;
											
											
											// / refresh empty_slot value  //necessary?
											
											for (p=start; p<40; p++)
											{
												if (mask[1][p]==0)
												{
													empty_slot=p;
													break;
												}
											}
											
											// / ok so let's remove this famous 'false tip'
											// / we'll then readjust height#1 value
											
											mask[posQ][q]=0;
											mask[posQ+1][q]=0;
											
											mask[height+1][q]--;    // only exception to centralizing, because we're correcting an error // NO!!! comment out now!
											mask[height+1][q]--;	  // NO, otherwise after centralized process, pos will be different from (greater than) the location of the body's tip
																							// and pos must be equal to the tip of the body at the beginning of next line (i+1, j=0)
																							// OK, so this is probably going to be uncommented, BUT prepare_slot1 must be adapted to centralized process // NO, NO
																							//                                                      and the rest of the code must be adapted accordingly // and NO
																							// TRUE REASON: new slots are not subject to centralizing because of byte#0 being set OFF										
											
											//posQ=mask[height+1][q]; // may be useless
																							// posQ is not used anymore in this IF loop
																							// and as soon as we get out of it we MUST set its value in any case for the times this IF loop isn't entered
																							// we might as well call posQ BEFORE this IF loop is entered...?,
																						  // this would allow to remove both THIS posQ assignment ans THE ONE at the beginning of this IF loop //??? NO!
																							// NO, just the one at the beginning: otherwise, after exiting this IF loop, posQ would be wrong
																							// therefore, best solution is removing THIS posQ assignment and keeping the NEXT ONE // let's do it then
											
											// / slot q's body is diverging and needs to be erased when j=320 (connect#1 set to OFF)
											
											mask[connect+1][q]=0;	
											
											
											// / setting sprout#1 ON on slot q // NO!
											// / slot q will not be stored // wtf? probably confused 'erasing' and 'not storing'
											// / if you don't store slot q's body, there will be a diverging point coming from nowhere...
										  //mask[sprout+1][q]=255;
											
											
											store_body(stored, q); // moved down from top of the loop, otherwise could not remove 'false tip'
											
											stored++;
											
											//posQ=posQ+2; // Cf centralising
																	 // now posQ refers to an EMPTY CURRENT location //why? // no use apparently... will comment out then
											             // (?) HERE what has been refered to as 'centralizing' up to now 
																	 // (?) accounts for the fact that this IF loop is 'OPEN' (no ELSE condition)
											
										}
										
										// / at this stage, wether the preceding IF loop has been entered or not,
										// / the body in slot q has been stored and slot q is doomed to be erased when j comes to 320
										
										// / as a note, if it HAS been entered, it means we've moved to a NEW STRIP and to A value of q that touched LAST STRIP
										// /                                                                            it's the very last (and most often, the sole) value of q met by LAST strip,
										// /																																						so the same q.
										// /            if it HASN'T, then q hasn't changed, we're still on the same strip
										// /                          AND we are now actually working on the CURRENT strip's line_start and j
										// / this structure allows to account for more than 2 ramifications for a divergence
										
										// / so q (the value) is 'the same'
										// / BUT, the body's 'fake tip' has been removed when the divergence was first detected
										// / good, because we don't need it anymore, it has been placed in an empty slot and a new body will emerge from it
										// / 																				 as a note, WHEREVER that empty slot is with respect to q (><) it cannot INTERFERE with future operations 
										// /                                         the current and future strips are DISTINCT from (and 'GREATER' than) the one of the former 'fake tip', 
										// /                                         in fact, we had to extract this latter 'from the past' in the preceding IF loop
										// /																				 and distinct strips of a same line do not overlap by definition
										// /            														 so there is no risk of storing anything else in this new slot before j comes to 320
										// / We still need the 'true tip' though, for connectivity reasons
										// / we will get it from the un-erased SLOT q, for code-hygiene reasons
										// / more specifically, at this stage it could also be retrieved from the STORED body
										// / BUT it's projected to NOT be the case in the future, namely as line thickness is accounted for
										
										posQ=mask[height+1][q];       // need to add +2 to go with the centralizing 
																										// therefore posQ is a location from the CURRENT turn
																										// seems to be redundant...? // yes, commenting it out// NO, see above, we'll just remove the +2
										
									
										
										//if (empty_slot==40) {while (1) {LED_CYAN;}}               //RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
										
										
										
										
										//prepare_slot1(empty_slot, stored-1, 0, 0, i-1, mask[posQ][q], mask[posQ+1][q]);
										//prepare_slot1(empty_slot, stored-1, 0, 0, i-1, mask[posQ-2][q], mask[posQ-1][q]);  // -2 to compensate preceding +2 // commented out the +2
										
										// / Retrieving node from soon-to-be-erased q slot	
										// / Preparing a slot with that value as first value,
										// / THEN, adding line_start and j
										
										prepare_slot1(empty_slot, stored-1, 0, 0, i-1, mask[posQ][q], mask[posQ+1][q]);
																				
										//mask[height+4][empty_slot]= line_start;
										//mask[height+5][empty_slot]= j;
										
										posES=mask[height+1][empty_slot];       // NO NEED to add +2 HERE because slot was prepared just before
										
										// / Following 4 lines are old comments, but still hold
										// (slot ES has connect0 byte set to ON, it will not be erased at the end of this line) // / so what?
										// thus, its height1 byte will be incremented by 2 // / WILL, as in NOT NOW
										// thus, the next 2 lines (+0, +1) need to be changed (to +2, +3) // / but its height#1 NEEDN'T be incremented
										// this way the current strip will become the last value of the slot for next turn
										
										//mask[posES+0][empty_slot]= line_start;
										//mask[posES+1][empty_slot]= j;
										mask[posES+2][empty_slot]= line_start;
										mask[posES+3][empty_slot]= j;
										
										//mask[height+1][empty_slot]++;                    //   centralizing this process
										//mask[height+1][empty_slot]++;
										
										left_merger=empty_slot;   // why?// no idea, seems half-baked --> comment out// wait...
																							// seems like we're keeping a reference to the recently prepared slot,
																							// which is actually a RIGHT_DIVerger, but we're labelling it as a left_merger // to wathever expected merger node, this body can only be a LEFT merger as it PRECEDES it
																							// so it could suppose that this branch is expected to merge to the right (becoming a left merger)
										
										empty_slot=40;
										
//									}
//									else
//									{
//										pos=mask[height+1][q];
//										mask[pos+1][q]=j;
//									}
									/*
									while (1) 
									{
										LED_CYAN;
									}
									*/
								}
								
								// CONVERGENCE
								
								
								// / things to check: - posQ is used consistently 
								// /                  - case of non convergence (continuing a segment) is consistent with centralizing
								// /                  - sprout#1 255 flag is used consistently 
								// /                  - is it replaceable? don't think so 
								// /                  - can flags be rearranged? probably
								
								else    //ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
								{	
									//mask[height+1][q]++;               //     centralizing this process
									
									//mask[height+1][q]++;
									
									mask[connect][q]=24;
									
									posQ=mask[height+1][q];						 // need to add +2 to go with the centralizing // Yes, BUT
																											 // for code hygiene, the +2 gets added when needed...
									mask[posQ+2][q]=line_start;					 // ...like here
									mask[posQ+3][q]=j;
									
									posLP=mask[height+1][last_partner];   // need to add +2 to go with the centralizing // same, +2 added each time, not in pos assignment

									if ((partners==2)&&(empty_slot<40))
									{	
										
										//convergence: if one of the two branches is too short, just delete it and go on with the longer one
																				
										char small = 2*10;
										
										// / DEAD BRANCH ROUTINE
										// / CONDITION: if any of the following is true:
										// /            - current slot is short - last partner's slot is short : either of the two bodies is short
										// /            - current slot's sprout#0 is EMPTY - last partner's sprout#0 is EMPTY : either of the two bodies descends from no other body
										// /                                                                                    for this last one the equivalent should exist for divergence
										// /            Problem: this setup allows for small YET CONNECTED segments to be erased, creating 'holes'
										
										// / modified to: if either is short AND descends from no other body...
										// /              yet, would be wise to FIRST check for unconnectedness, THEN (if needed) compare tallnesses //done
										
										//if ((0)&&((posQ<3)||(posLP<3)||(mask[sprout][q]==0)||(mask[sprout][last_partner]==0)))
										
										if ((0)&&(((posQ<small)&&(mask[sprout][q]==0))||((posLP<small)&&(mask[sprout][last_partner]==0)))) 
											  // 'posQ' needs to be replaced with the true tallness measure
										{
											if ((posLP<small)&&(mask[sprout][q])&&(mask[sprout][last_partner]==0))  // if LP small + unconnected and Q connected
											{
												mask[sprout+1][last_partner]=255;
												mask[connect+1][last_partner]=0;
											}
											else if ((posQ<small)&&(mask[sprout][q]==0)&&(mask[sprout][last_partner]))  // if Q small + unconnected and LP connected
											{
												mask[sprout+1][q]=255;
												mask[connect+1][q]=0;
												
												//extending last_partner on the right (for eventual divergence next turn)
												mask[posLP+1][last_partner]=mask[posQ+1][q];
											}
											else if ((mask[sprout][q]==0)&&(mask[sprout][last_partner]==0)&&((posQ<small)||(posLP<small)))  // if  LP and Q unconnected and one is small
											{
												if (posQ>posLP)
												{
													mask[sprout+1][last_partner]=255;
													mask[connect+1][last_partner]=0;
												}
												else
												{
													mask[sprout+1][q]=255;
													mask[connect+1][q]=0;
												}
											}							
										}
										else  // / IF BOTH BRANCHES ARE KEPT --> ACTUAL CONVERGENCE
										{
											// / Store a merging point:
											// / col#0 and col#1 are blank
											// / col#2 is the merging point's ordinate
											// / col#3 is the middle of the gap dividing the 2 merging bodies
											
											mask[stored][0]=0;
											mask[stored][1]=0;
											mask[stored][2]=Hsize-i;
											mask[stored][3]=(mask[posLP+1][last_partner]+mask[posQ][q])>>1;		//mask[stored][3]=(line_start_LP+j_Q)>>1;															
											
											//in case this was also a diverging point, 'sprout' value doesn't get stored
											
											
											// / Condition: LP's sprout#1 flag is turned ON (--> need not be stored?)//that's just another (irrelevant) consequence of the cause we're focusing on
											// / "in case LP has diverged"? and/or "LP is a dead branch"? // CANNOT be a dead branch, as LP would not be assigned with a dead branch value (Cf dead branch routine)
											// / so LP has previously diverged, therefore it has already been stored, and adding to the doomed slot is pointless
											// / how would the opposite be possible? does it mean that this is a 3+branches convergence? so the alternative is a 2branches convergence?
											
											if (mask[sprout+1][last_partner])//255 is the only non-zero value
																											 // (this means the slot is doomed and we need to add the information directly into the storage)consequence
																											 // 3+BRANCHES CONVERGENCE
											{
												mask[stored-1][6]=stored;   // last partner's right merger is the newly created-and-stored node
												mask[merge][q]=stored;      // current partner's left merger is the ncas node
											}
											else // 0 value
												   // (the slot has not been doomed yet)consequence
											     // 2BRANCHES CONVERGENCE
											{
												mask[merge+1][last_partner]=stored;   // idem
												mask[merge][q]=stored;                // current partner's left merger is the ncas node
												
												activ_slot=empty_slot;
												
												prepare_slot1(activ_slot, stored, 0, 0, i, line_start, j);

												empty_slot=40;
											}
							
	///YYYYYYY										
											if (mask[sprout+1][last_partner]==0) //ELSE
											{
												
												
											}
											else
											{
												mask[sprout][left_merger]=stored;
											}
											
											stored++;
											
											// last and current partner now need to be erased
											mask[connect+1][last_partner]=0;
											mask[connect+1][q]=0;
										}
										
										if (see_matrix)
											bwdisplay1();
										//}
									}
									else if (partners>2)
									{
										mask[merge][q]=stored;  //?//yes
										mask[stored][3]=(mask[posLP+1][last_partner]+mask[posQ][q])>>1;
										mask[stored][3]=((mask[stored][3]<<1)-mask[posLP][last_partner]+mask[posQ][q])>>1; // shifting merging point to the right         
										//possibility of creating a flag to compensate for integer approximation                                 // and YES, no +1 between ] and ] on this line
										
										//current partner needs to be erased
										mask[connect+1][q]=0;								
									}
									
									if ((mask[connect+1][q]==0)&&(partners==1))
										mask[connect+1][q]=24;

								}   
								///  END CONVERGENCE ZONE
								
								//mask[connect][q]=24;
								last_partner=q;
							}    
							else if ((mask[1][q]==0)&&(q<empty_slot))
							{
								empty_slot=q;
							}
						}
						
						// / Minor optimisation:
						// / if ... (need to understand)
						
						if ((partners==0)&&(empty_slot<40))
						{
							activ_slot=empty_slot;
							
							prepare_slot1(activ_slot, stored, 0, 0, i, line_start, j);
							
							empty_slot=40;
							
							if (see_matrix)
								bwdisplay1();
							
						}
						//bwdisplay1();
						
						black=0;
					}
					//*/
					if (lng>4)
						number++;
					lng=0;
					///*
					
					// / Refresh empty slot
					// / if all slots are busy --> BLOCK, PURPLE
					
					if (empty_slot==40)
					{							
						for (q=start; q<40; q++)
						{
							if (mask[1][q]==0)
							{
								empty_slot=q;
								break;
							}
						}
						if (empty_slot==40)
							while (1)
							{
								LED_PURPLE;
								bwdisplay1();
								while(1){}
							}
					}
					
					// / If storage is full --> BLOCK, YELLOW
					
					if (stored==230)
						while (1)
						{LED_YELLOW;
							bwdisplay1();
							while(1){}}
								
					//*/
					
					
				}
			}
			
			if (rest==7)
			{
				bwdiff[i][JJ]=bwbyte;
				bwbyte=0;
				JJ++;
			}
		
			//ILI9341_Write_Data ( RGB24TORGB16 ( average, average, average ) );   //ILI9341_Write_Data ( RGB24TORGB16 ( r, g, b ) );
			
			READ_FIFO_PIXEL(Camera_Data);
			
			red =  GETR_FROM_RGB16(Camera_Data);
			green =  GETG_FROM_RGB16(Camera_Data);
			blue =  GETB_FROM_RGB16(Camera_Data);
			
			//buffer[(i-1)%3][j*3]=red;
			buffer[iii-1][jj]=red;
			buffer[iii-1][jj+1]=green;
			buffer[iii-1][jj+2]=blue;
			
		} 
		
		//
		// GENERAL CLEAN-UP AFTER READING THROUGH A LINE OF PIXELS
		//
		
		//bwdisplay1();
		
		for (q=start; q<40; q++)
		{
			
			// / CONDITION: byte#0 and connect#1 in slot q are ON
			// /            byte#0 : slot is not empty
			// /            connect#1 : slot DOES NOT need to be erased (no convergence, no divergence)
			
			if ((mask[0][q])&&((mask[connect+1][q])))     //  centralized process	
			{  
				mask[height+1][q]++;                 
				mask[height+1][q]++;
			}
			
			
			// / Store bodies that are too long and start a new one at their tips
			
			posQ=mask[height+1][q];
			if (posQ>220)
			{
				mask[connect+1][q]=0;
				
				activ_slot=empty_slot;
				prepare_slot1(activ_slot, stored, 0, 0, i, mask[posQ][q], mask[posQ+1][q]);
				empty_slot=40;			
			}
			
			/// DELETION ZONE
			
			// / CONDITION: byte#0 ON and connect#1 OFF     // could use an ELSE in the future... // ... or not, just rearranged the code
			// /            slot is not empty AND needs to be erased because convergence/divergence has been detected
			
			// /            connect#1 OFF: this byte's default value is ON
      // /                           for it to be OFF, the slot must have diverged or converged
			// /                           divergence: somewhere along the way, a strip touched a body whose state was connect#0 ON and connect#1 OFF
			// /                           convergence: a strip has 2 'partners' (to be reviewed)
			
			if ((mask[0][q])&&((mask[connect+1][q]==0)))
			{
				///*
				
				// / CONDITION: sprout#1 OFF and (body in slot q is tall or connect#0 is ON)
				// /            sprout#1 OFF:  when ON this flag signals not to store a body that is going to be deleted  // ATM, only used for dead ends resulting from mergers
				// /                           it is required to be OFF here, for the body to be stored before deletion
				// /            connect#0 ON:  this byte is usually what tells us if a body has been added to in the last turn,
				// /                           in which case body needs NOT be erased and therefore also NOT be stored
				// /                           YET, to get here, connect#1 must be OFF
				// /                           and connect#1 OFF + connect#0 ON --> body merged/diverged (and of course, that implies that it was continued)
				// /                           ??? does connect#0 OFF and connect#1 OFF happen? // NO, because divergence/convergence requires the body to be added to
				
				if ((mask[sprout+1][q]==0)&&((posQ>20)||(mask[connect][q])))
				{
					mask[stored][0]=mask[height][q];
					mask[stored][1]=(mask[height+2][q]+mask[height+3][q])>>1;
					mask[stored][2]=mask[height][q]-((posQ-height)>>1);//+height-4; //-4 sort of seems to work... but why?//because height-4 = height/2
					mask[stored][3]=(mask[posQ][q]+mask[posQ+1][q])>>1;
					
					mask[stored][4]=mask[sprout][q];
					mask[stored][5]=mask[merge][q];
					mask[stored][6]=mask[merge+1][q];
					
					//mask[stored][7]=mask[height+1][q];
					mask[stored][7]=15;
					
					stored++;
				}
				//*/

				//for (tmp=0; tmp<=col; tmp++)  //not really necessary to erase everything, 'occupy' byte(s) suffice #optimize
				for (tmp=0; tmp<=239; tmp++) //debugging...
				{
					mask[tmp][q]=0;
				}
			}
			mask[connect][q]=0;
			mask[connect+1][q]=0;
			if (mask[1][q])
			{
				mask[0][q]=126;
			}
		}
		
		if (number>maxnumber)
			maxnumber=number;
		
	} 
	
	//bwdisplay1();
	//while(1){}
	
	//ILI9341_DrawLine( 0, 0, 320, 240 );
	
	sprintf(phrase,"objects %d akk",maxnumber);
	
	//ILI9341_DispStringLine_EN(220,phrase);
	
	if (draw)
	{
		for (tmp=8; tmp<stored; tmp++)
		{
			if (mask[tmp][0])
				ILI9341_DrawLine( mask[tmp][1], Hsize-mask[tmp][0]-1, mask[tmp][3], Hsize-mask[tmp][2]-1 );
				//ILI9341_DrawLine( mask[tmp][1], Hsize-mask[tmp][0]-1, 0, 0 );
		}
	}
	/*
	ILI9341_DrawLine( 255, 0, 255, 240 );
	ILI9341_DrawLine( 0, 113, 320, 113 );
	
	ILI9341_DrawLine( 112, 0, 112, 240 );
	ILI9341_DrawLine( 0, 240-14, 320, 240-14 );
	*/
	if (block)
		while(1){LED_WHITE;}
}



void border_det2(void)
{
	int i, j, kk, kkk, ii, iii, JJ, jj, jjj;
	short indi, indj;
	short a1, a2, counter, doub;
	
	int boole;

	BYTE red,green,blue;	
	
	uint16_t Camera_Data;
	
	maskvoid();
	
	char lele=2;

	int8_t index_i[2]={-2,0};
	int8_t index_j[2]={ 0,2};
	
	uint8_t bwbyte;
	bwbyte=0;
	char rest;
	
	ILI9341_OpenWindow(0, 0, 320, 240);
	ILI9341_Write_Cmd (CMD_SetPixel ); 
	
	white_cntr=0;
	

	
	for (i=0; i<3; i++)
	{
		for (j=0; j<Bsize; j++)
		{
			READ_FIFO_PIXEL(Camera_Data);
		
			red =  GETR_FROM_RGB16(Camera_Data);
			green =  GETG_FROM_RGB16(Camera_Data);
			blue =  GETB_FROM_RGB16(Camera_Data);
			
			jj=j*3;
			
			buffer[i][jj] = red;
			buffer[i][jj+1] = green;
			buffer[i][jj+2] = blue;
		}
	}
	
	JJ=0;
	for (j=0; j<Bsize; j++)
	{
		rest=j%8;
		a1=0;
		counter=0;
		
		jj=j*3;
		
		//for (kk=3; kk<lele; kk++)
		
	
		for (kk=1; kk<lele; kk++)
		{
			indi=index_i[kk];
			indj=j+index_j[kk];
			jjj=indj*3;
			boole=(indi+1)*(indj+1)*(Hsize-indi)*(Bsize-indj);
			if (boole)
			{
				a2=0;
				for (kkk=0; kkk<3; kkk++)
				{
					doub=buffer[0][jj+kkk]-buffer[indi][jjj+kkk];
					a2+=abs(doub); 
				}
				//a1+=(uint8_t)(a2/3);
				a1+=a2;
				counter=counter+1;
			}
		}
		counter*=3;
		a1/=counter;
		
		if (a1>40)
		{					
			bwbyte = bwbyte|(128>>rest);
			white_cntr++;
		}
		
		if (rest==7)
		{
			bwdiff[0][JJ]=bwbyte;
			bwbyte=0;
			JJ++;
		}
	}
	
	char start=8;
	char end=39;//40;
	

	uint8_t lng;
	char whiteon;           // strip is being detected
	char abort=0;
	short black=0;          // soon to be used to measure distance between consecutive strips
	uint8_t strip_rank;
	
	uint8_t pointer;
	uint8_t posQ;
	uint8_t posCP;
	uint8_t posLP;
	
	char empty_slot=8;
	uint8_t partners;
	uint8_t curr_partner;
	uint8_t last_partner;
	uint8_t overlap0;
	//uint8_t stored;
	//uint8_t storedN;
	stored=7;  // will be incremented to 8 before first value is stored
	storedN=255; // will be decremented to 254 before first value is stored
	
	mask[0][0]=126; mask[0][1]=126; mask[1][0]=126; mask[1][1]=126;
	
	uint8_t strip_start;
	uint8_t q, p, tmp;
	

	// slot library
	
	stud=0;
	stag=1;
	
	detect0=2;
	detect1=3;
	alive=4;
	succumb=5;
	byte4=6;
	byte5=7;
	byte6=8;
	sprout=9;
	stored_alrd=10;
	merge0=11;
	merge1=12;
	root=13;
	tip=14;
	
	for (i=0;i<7;i++)
	{
		for (j=0;j<7;j++)
			mask[i][j]=126;
	}
	
	
	ii=0;
	
	for (i=1; i<240-2+3; i++)
	{	
		
		ii++;
		iii=ii;
		if (ii==3)
			ii=0;
		
		JJ=0;
		
		whiteon=0;
		
		lng=0;

		strip_rank = start-1;
		
		overlap0 = start;
		pointer = start; //desperate debugging
		
		//debug
		char last_one;
		char last_pos;
		
		last_one=7;
		last_pos=tip+1;		
		
		//
		//  BEGIN READING THROUGH A LINE OF PIXELS
		//
		
		for(j=0; j<320; j++) 										
		{
			rest = j%8;
			
			jj=j*3;
		
			a1=0;
			counter=0;
		
			for (kk=0; kk<lele; kk++)
			{
				indi=(iii+1+index_i[kk])%3;
				indj=j+index_j[kk];
				jjj=3*indj;
				boole=(indi+1)*(indj+1)*(Hsize-indi)*(Bsize-indj);
				if (boole)
				{
					a2=0;
					for (kkk=0; kkk<3; kkk++)
					{
						doub=buffer[ii][jj+kkk]-buffer[indi][jjj+kkk];
						a2+=abs(doub); 
					}
					a1+=a2;
					counter=counter+1;
				}
			}
			a1/=counter;
			
			if ((a1>20)&&(j<255)&&(i<239))
			{					
				bwbyte = bwbyte|(128>>rest);
				white_cntr++;
				ILI9341_Write_Data ( RGB24TORGB16 ( 255, 255, 255 ) );
				lng++;
				
				if (whiteon==0)
				{
					whiteon=1;
					strip_start=j;					
				}
			}
			else
			{
				ILI9341_Write_Data ( RGB24TORGB16 ( 0,0,0 ));
				black++;
				
				//if ((abort==0)&&(whiteon==1))  // abort allows to impede stored and storedN to overlap
				if (whiteon==1) 
				{
					whiteon=0;
					
					last_partner=7;
					
					partners=0;
					
					if (lng>=3)
					{
						strip_rank++;
						//for (q=start; q<end; q++)
						if ((i>0)&&(mask[stag][start]>0))
						{	
							
							pointer = overlap0;
							//pointer = start;  //blind debugging
							curr_partner = mask[stag][pointer];
							posCP = mask[tip][curr_partner];
							
							while (mask[posCP+1][curr_partner]<strip_start)
							{
								if (mask[stag][pointer]==0)
									break;
								
								pointer++;
								curr_partner = mask[stag][pointer];
								posCP = mask[tip][curr_partner];
								
								if (pointer>=end)
								{
									if (1)
									{
										bwdisplay1();
										while (1)
										{
											LED_CYAN;
										}
									}
									//break;
								}
							}
							
							overlap0 = pointer; // next strip will start looking for partners in the stag starting from this value
							
///MAIN WHILE LOOP
							
							while((mask[stag][pointer]>0)&&(mask[posCP+0][curr_partner]<=j))
							{
								//abort in case storage is full
								if (storedN-stored<10)
								{
									abort=1;
									break;
								}
							
							//posQ=mask[tip][q];    before...
							
							// CONDITION: strip overlaps with last stored,
							//            strip's start is not far beyond and stip's end is not far back						
						  //if ((mask[detect0][q])&&(strip_start<=mask[posQ+1][q]-1)&&(j>=mask[posQ][q]+1))
							
							
								posLP=mask[tip][last_partner];
								
								partners++;
								
								///NORMAL GROWTH
								if ((mask[alive][curr_partner]==0)&&(mask[succumb][curr_partner]==0)&&(partners==1))
								{
									mask[posCP+2][curr_partner]=strip_start;
									mask[posCP+3][curr_partner]=j;
	
									mask[alive][curr_partner]=24;
									
									mask[stud][strip_rank]=mask[stag][pointer];  // value equal to curr_partner
																															 // stud <--> strip_rank, stag <--> pointer
									
									//debug
									posQ=mask[150][curr_partner];
										//don't account for more than 2 consecutive added strips
									if((mask[150+posQ][curr_partner]!=1)||(mask[150+posQ-1][curr_partner]!=1)||(posQ==1))
									{
										posQ++;
										mask[150+posQ][curr_partner]=1;
										mask[150][curr_partner]=posQ;
									}
								}
								else
								{
									///CONVERGENCE
									if (mask[alive][curr_partner]==0)
									{
										mask[alive][curr_partner]=24;
										
										/*
										
										///FAKE CONVERGENCE
										char small = 2*10;
										
										// / DEAD BRANCH ROUTINE
										// / CONDITION: if either is short AND descends from no other body...
										// /            FIRST check for unconnectedness, THEN (if needed) compare tallnesses
										
										if ((0)&&(((posCP-root<small)&&(mask[sprout][curr_partner]==0))||((posLP-root<small)&&(mask[sprout][last_partner]==0)))) 
											  // 'posQ' needs to be replaced with the true tallness measure
										{
											if ((posLP-root<small)&&(mask[sprout][curr_partner])&&(mask[sprout][last_partner]==0))  // if LP small + unconnected and CP connected
											{
												mask[succumb][last_partner]=24; 
												mask[stored_alrd][last_partner]=255;  // LP hasn't been stored, we just don't want it to be stored during clean-up
												
												//extending current partner on the left (for eventual divergence next turn)
												mask[posCP+2][curr_partner]=mask[posLP+2][last_partner];
												mask[posCP+3][curr_partner]=j;
												
												//erasing last partner's false tip
												mask[posLP+2][last_partner]=0;
												mask[posLP+3][last_partner]=0;
												
												mask[stud][strip_rank]=curr_partner;  // same thing as mask[stag][pointer] in principle
											
											}
											else if ((posCP-root<small)&&(mask[sprout][curr_partner]==0)&&(mask[sprout][last_partner]))  // if CP small + unconnected and LP connected
											{
												mask[succumb][curr_partner]=24;
												mask[stored_alrd][curr_partner]=255;
												
												//extending last_partner on the right (for eventual divergence next turn)
												mask[posLP+3][last_partner]=j;
												
												//nothing to erase
												
												mask[stud][strip_rank]=last_partner;
											}
											else if ((mask[sprout][curr_partner]==0)&&(mask[sprout][last_partner]==0)&&((posCP-root<small)||(posLP<small)))  // if  LP and CP unconnected and one is small
											{
												if (posCP>posLP)
												{
													mask[stored_alrd][last_partner]=255; 
													mask[succumb][last_partner]=24;
													
													//extending current partner on the left (for eventual divergence next turn)
													mask[posCP+2][curr_partner]=mask[posLP+2][last_partner];
													mask[posCP+3][curr_partner]=j;
													
													//erasing last partner's false tip
													mask[posLP+2][last_partner]=0;
													mask[posLP+3][last_partner]=0;
													
													mask[stud][strip_rank]=curr_partner;
												}
												else
												{
													mask[stored_alrd][curr_partner]=255;
													mask[succumb][curr_partner]=24;
													
													//extending last_partner on the right (for eventual divergence next turn)
													mask[posLP+3][last_partner]=j;
													
													//nothing to erase
													
													mask[stud][strip_rank]=last_partner;
												}
											}							
										}
										
										
										///ACTUAL CONVERGENCE
										else if
										*/
										if (partners==2)
										{
											///CV#0
											if (mask[succumb][last_partner]==0)
											{
												storedN--;
												store_node(storedN, 0, i,(strip_start+j)>>1);
												
												prepare_slot1(empty_slot, 0, 1, 2, i, strip_start, j); //3rd and 4th argument correspond to merge 0 and 1: useless//used to debug
																								
												mask[stud][strip_rank]=empty_slot;
												
												mask[sprout][empty_slot]=storedN;
												mask[merge0][curr_partner]=storedN;												
												mask[merge1][last_partner]=storedN;
												
												mask[posLP+2][last_partner]=0;
												mask[posLP+3][last_partner]=0;
												
												
												stored++;
												store_body(stored, last_partner);
												mask[stored][7]=24; // debug
												
												
												mask[stored_alrd][last_partner]=stored;
												mask[succumb][last_partner]=24;
												
												stored++;
												store_body(stored, curr_partner);
												mask[stored][7]=96; // debug
												
												mask[stored_alrd][curr_partner]=stored;
												mask[succumb][curr_partner]=24;
												
												//refresh empty_slot's value
												for (p=start; p<end; p++)
												{
													if (mask[detect1][p]==0)
													{
														empty_slot=p;
														break;
													}
												}
												if (mask[detect1][empty_slot])
												{
													while (1){ LED_PURPLE; bwdisplay1(); while(1){} }
												}
												/*
												//debug
												mask[199][last_partner]=126;
												mask[199][curr_partner]=126;
												mask[200][last_partner]=1;
												mask[200][curr_partner]=2;
												*/
												//debug
												posQ=mask[150][curr_partner];
												posQ++;
												mask[150+posQ][curr_partner]=64+1;
												mask[150][curr_partner]=posQ;
												
												posQ=mask[150][last_partner];
												posQ++;
												mask[150+posQ][last_partner]=96+1; //96=64+32
												mask[150][last_partner]=posQ;
												
											}
											else
											{
												///CV#2						
												if (mask[merge0][last_partner])
												{
													mask[storedN][4]=0;
													mask[storedN][3]=(strip_start+j)>>1;
													mask[storedN][2]--;                  //Hsize-(i) = (Hsize-(i-1))-1 
													
													mask[merge1][last_partner]=storedN; // LP has already been stored, hence the following line
													mask[stored][6]=storedN;
													
													mask[merge0][curr_partner]=storedN;
													
													stored++;
													store_body(stored, curr_partner);
																										
													mask[stored_alrd][curr_partner]=stored;
													mask[succumb][curr_partner]=24;
													/*
													//debug
													mask[199][last_partner]=126;
													mask[199][curr_partner]=126;
													mask[200][last_partner]=3;
													mask[200][curr_partner]=4;
													*/
													//debug
													posQ=mask[150][curr_partner];
													posQ++;
													mask[150+posQ][curr_partner]=64+2;
													mask[150][curr_partner]=posQ;
													
													posQ=mask[150][last_partner];
													posQ++;
													mask[150+posQ][last_partner]=96+2;
													mask[150][last_partner]=posQ;
													
												}
												///CV#X						
												else
												{
													storedN--;
													store_node(storedN, 0, i, (strip_start+j)>>1);
													
													mask[merge1][last_partner]=storedN; // LP has already been stored, hence the following line
													mask[stored][6]=storedN;
													
													mask[merge0][curr_partner]=storedN;
													
													stored++;
													store_body(stored, curr_partner);
													
													mask[stored_alrd][curr_partner]=stored;
													mask[succumb][curr_partner]=24;
													/*
													//debug
													mask[199][last_partner]=126;
													mask[199][curr_partner]=126;
													mask[200][last_partner]=5;
													mask[200][curr_partner]=6;
													*/
													//debug
													posQ=mask[150][curr_partner];
													posQ++;
													mask[150+posQ][curr_partner]=64+4;
													mask[150][curr_partner]=posQ;
													
													posQ=mask[150][last_partner];
													posQ++;
													mask[150+posQ][last_partner]=96+4;
													mask[150][last_partner]=posQ;
												}
											}
										}
										///CV#Z
										else
										{
											mask[merge0][curr_partner]=storedN;
													
											stored++;
											store_body(stored, curr_partner);
											
											mask[stored_alrd][curr_partner]=stored;
											mask[succumb][curr_partner]=24;
											/*
											//debug
											mask[199][curr_partner]=126;
											mask[200][curr_partner]=8;
											*/
											//debug
											posQ=mask[150][curr_partner];
											posQ++;
											mask[150+posQ][curr_partner]=64+8;
											mask[150][curr_partner]=posQ;
										}
									}
									///DIVERGENCE
									else if (partners==1)
									{
										if ((mask[storedN][4]!=mask[stored_alrd][curr_partner])||(mask[stored_alrd][curr_partner]==0)) // if CP not stored, stored_alrd=0...
										{
											///DV#0
											if (mask[merge0][curr_partner]==0)
											{
												storedN--;
												store_node(storedN, 0, i-1, (mask[posCP][curr_partner]+mask[posCP+1][curr_partner])>>1);
												
												prepare_slot1(empty_slot, storedN, 2, 16, i, mask[posCP+2][curr_partner], mask[posCP+3][curr_partner]);
												
												//debug
												posQ=mask[150][empty_slot];
												posQ++;
												mask[150+posQ][empty_slot]=0;
												posQ++;
												mask[150+posQ][empty_slot]=curr_partner; //80=64+16
												posQ++;
												mask[150+posQ][empty_slot]=0;
												mask[150][empty_slot]=posQ;
												//
												
												mask[posCP+2][curr_partner]=0;
												mask[posCP+3][curr_partner]=0;
												
												mask[stud][strip_rank-1]=empty_slot; // last_strip was previously appended to current_partner,
																														 // hence, current_partner was stored to the stud at last_strip's rank
												
												stored++;
												store_body(stored, curr_partner);
												
												mask[succumb][curr_partner]=24;
												mask[stored_alrd][curr_partner]=stored;
												
												mask[storedN][4]=stored;
												
												for (p=start; p<end; p++)
												{
													if (mask[detect1][p]==0)
													{
														empty_slot=p;
														break;
													}
												}
												if (mask[detect1][empty_slot])
												{
													while (1){ LED_PURPLE; bwdisplay1(); while(1){} }
												}	
												
												prepare_slot1(empty_slot, storedN, 3, 17, i, strip_start, j);
																								
												mask[stud][strip_rank]=empty_slot; // add current_strip to the stud
												/*
												//debug
												mask[199][curr_partner]=126;
												mask[200][curr_partner]=16;
												*/
												//debug
												posQ=mask[150][curr_partner];
												posQ++;
												mask[150+posQ][curr_partner]=80+1; //80=64+16
												mask[150][curr_partner]=posQ;
												//debug
												if (mask[150+posQ-1][curr_partner]==81)
												{
													LED_GREEN;
													while(counter--){}
													bwdisplay1();
													while(1){}
												}
											}
											///DV#1
											else
											{
												storedN--;
												store_node(storedN, stored, i-1, (mask[posCP][curr_partner]+mask[posCP+1][curr_partner])>>1);
												
												prepare_slot1(empty_slot, storedN, 4, 32, i, strip_start, j);
												
												mask[stud][strip_rank]=empty_slot;
												/*
												//debug
												mask[199][curr_partner]=126;
												mask[200][curr_partner]=32;
												*/
												//debug
												posQ=mask[150][curr_partner];
												posQ++;
												mask[150+posQ][curr_partner]=80+2;
												mask[150][curr_partner]=posQ;
											}
										}
										///DV#Z
										else
										{
											prepare_slot1(empty_slot, storedN, 5, 64, i, strip_start, j);
											
											mask[stud][strip_rank]=empty_slot;
											/*
											//debug
											mask[199][curr_partner]=126;
											mask[200][curr_partner]=64;
											*/
											//debug
											posQ=mask[150][curr_partner];
											posQ++;
											mask[150+posQ][curr_partner]=80+4;
											mask[150][curr_partner]=posQ;
										}
										
										//refreshing empty_slot's value
										for (p=start; p<end; p++)
										{
											if (mask[detect1][p]==0)
											{
												empty_slot=p;
												break;
											}
										}
										if (mask[detect1][empty_slot])
										{
											while (1){LED_PURPLE; bwdisplay1(); while(1){}}
										}
									}
									else
									{
										//nothing, this case (CP not a virgin)&&(partners>1) is impossible
										//strip_rank--; //the iteration doesnt increment strip_rank
										//pointer--; //nope, this will make the loop go round
										
										//debug
										mask[146][curr_partner]=126;
										mask[147][curr_partner]=255;
										mask[50][curr_partner]=strip_start;
										mask[51][curr_partner]=j;
										if (1)
										{
											bwdisplay1();
											while (1){LED_BLUE;LED_RGBOFF;}
										}
									}
									
								}
								
								
								//TIP0 TIP15 DEBUG
								if ((last_one>7)&&(mask[posCP][curr_partner]<mask[last_pos][last_one]))
								{
									
									mask[50][curr_partner]=strip_start;
									mask[51][curr_partner]=j;
									
									ILI9341_DrawRectangle(mask[posCP][curr_partner],200,3,3,1);
									ILI9341_DrawRectangle(mask[last_pos][last_one],205,3,3,0);
									
									ILI9341_DrawLine(mask[posCP][curr_partner],198,mask[posCP][curr_partner],i+2);
									ILI9341_DrawLine(mask[last_pos][last_one],203,mask[last_pos][last_one],i+1);//mask[root][last_one]+1);//
									
									ILI9341_DrawRectangle(mask[tip+1][last_one]-2,mask[root][last_one]-1,1,1,1);
									
									char current[40], lastone[40], cptip[40];
									char c = last_one;
									char cc = curr_partner;
									char ccc = mask[tip][curr_partner];
									
									sprintf(current,"current: %d ",cc);
									sprintf(lastone,"lastone: %d ",c);
									sprintf(cptip,"tip: %d ",ccc);
									
									ILI9341_DispString_EN(100,200,current);
									ILI9341_DispString_EN(100,220,lastone);
									ILI9341_DispString_EN(100,180,cptip);
									
									int count=10000000;
									
									char debug1, debug2, debug3;
									debug3=0;
									
									for (q=start; q<end; q++)
									{
										if (mask[stag][q]>0)
										{
											
											debug1=mask[stag][q];
											debug2=mask[tip][debug1];
											
											ILI9341_DrawLine( mask[debug2][debug1], 150+debug3, mask[debug2+1][debug1], 150+debug3);
											
											debug3++;
											debug3++;
											
											while(count--){};
											count=10000000;
										}
									}
									count=100000000;
									while(count--){};
									bwdisplay1();
									while (1) {LED_RED;}
									
								} //end tip0 debug
								

								if (pointer>=end)
								{
									if (1)
									{
										bwdisplay1();
										while (1){LED_GREEN;}
									}
									//break;
								}

								
								pointer++;
								
								if (mask[stag][pointer]==0)
									break;
								
								last_partner=curr_partner;
								posLP=posCP;
								
								last_pos = posCP;
								last_one = curr_partner;
								
								curr_partner = mask[stag][pointer];
								posCP = mask[tip][curr_partner];
								
							} // end of main while loop // used to be 'if (overlap detected)'
							
							//debugging animation
							
							if (0)
							{
								int count=1000000;
								
								for (q=start; q<end; q++)
								{
									if (mask[stag][q]>0)
									{
										char debug1, debug2;
										
										debug1=mask[stag][q];
										debug2=mask[tip][debug1];
										
										ILI9341_DrawLine( mask[debug2][debug1], 180, mask[debug2+1][debug1], 180);
										
										while(count--){};
										count=1000000;
									}
								}
								
								LED_RED;
								while(count--)
								LED_RGBOFF;
								
								bwdisplay1();
						  }
							
							if ((0)&&(((storedN-stored<50))||(storedN<100)||(stored>200))) // replaced with the abortion protocol
							{
								while (1)
								{LED_YELLOW;
									bwdisplay1();
									while(1){}}
							}
							
						} // end of 'if stag isn't empty' // used to be for-q loop
						
						if (partners==0)
						{
							prepare_slot1(empty_slot, 0, 6, 128, i, strip_start, j);
							
							mask[stud][strip_rank]=empty_slot;
							
							for (p=start; p<end; p++)
							{
								if (mask[detect1][p]==0)
								{
									empty_slot=p;
									break;
								}
							}
							if (mask[detect1][empty_slot])
							{
								while (1)
								{
									LED_PURPLE;
									bwdisplay1();
									while(1){}
								}
							}
						}
						
					} //if strip length is sufficient
					
					lng=0;
					
			  } //if we're currently on a white strip (and just encountered a black pixel)	
				
			} // else <==> if the pixel is not considered sufficiently bright to be white ==> pixel is black						
					
			
			//need to store a byte to matrix every eight pixels
			if (rest==7)
			{
				bwdiff[i][JJ]=bwbyte;
				bwbyte=0;
				JJ++;
			}
			
			READ_FIFO_PIXEL(Camera_Data);
			
			red =  GETR_FROM_RGB16(Camera_Data);
			green =  GETG_FROM_RGB16(Camera_Data);
			blue =  GETB_FROM_RGB16(Camera_Data);

			buffer[iii-1][jj]=red;
			buffer[iii-1][jj+1]=green;
			buffer[iii-1][jj+2]=blue;
			
		} // end of for-j loop

		
	//
	// GENERAL CLEAN-UP AFTER READING THROUGH A LINE OF PIXELS
	//

	
		for (q=start; q<end; q++)
		{
			mask[stag][q]=mask[stud][q];
			mask[stud][q]=0;
			
			posQ=mask[tip][q];
			
			// detect0 is on for slots that have been occupied for more than 1 j-loop
			if (mask[detect0][q]==126)
			{
				// incrementing tip's location
				if ((mask[alive][q]==24)&&(mask[succumb][q]==0))
				{
					mask[tip][q]++;
					mask[tip][q]++;
					
					// breaking bodies that are too long in 2
					//if ((0)&&((mask[tip][q]-mask[root][q])>240))
					if ((0)&&(mask[tip][q]>140)) //taking the debugging grid into account
					{
						stored++;
						store_body(stored,q);
						mask[succumb][q]=24;
						mask[stored][q]=stored;
						prepare_slot1(empty_slot, stored, 0, 0, i, mask[posQ][q], mask[posQ+1][q]);
						
						//mask[tip][empty_slot]++;  //empty slots are prepared with tip pointing to first strip, 
						//mask[tip][empty_slot]++;  //not supposed to be incremented on the same turn they get created
						
						//debug
						mask[120][empty_slot]=24;
						mask[121][empty_slot]=24;
						mask[143][q]=8;
						mask[144][q]=28;
						mask[145][q]=8;
						
						//setting stag to new value
						for (p=start;p<end; p++)
						{
							
							if ((p<=q)&&(mask[stag][p]==q))
							{
								mask[stag][p]=empty_slot;
								break;
							}
							else if ((p>q)&&(mask[stud][p]==q))
							{
								mask[stud][p]=empty_slot;
								break;
							}
							else {} //nothing, carry on
								
							if (p==end){while(1){LED_BLUE;LED_YELLOW;}}
						}
						//refresh
						for (p=start; p<end; p++)
						{
							if (mask[detect1][p]==0)
							{
								empty_slot=p;
								break;
							}
						}
						if (mask[detect1][empty_slot])
						{
							while (1)
							{
								LED_PURPLE;
								bwdisplay1();
								while(1){}
							}
						}
					}
				}
				
				/// DELETION ZONE
				if (((mask[alive][q]==0)||(mask[succumb][q]==24)))
				{
					// bodies that have to be stored
					if ((abort==0)&&(((0)||((mask[tip][q]-tip)>10))||(mask[alive][q]==24)||(mask[sprout][q]>0))&&(mask[stored_alrd][q]==0))
					{
						stored++;
						store_body(stored,q);	
						mask[stored][7]=6; // debug
											
					}
					//debug
					uint8_t debug0;
					uint8_t debugz;
					
					debug0=mask[150][q];
					debugz=mask[201][q];
					
					if (debug0>80) //Hsize=240, not 255
					{
						debug0=0;
						for (p=150;p<200;p++)
						{
							mask[p][q]=0;
						}
					}
					else if ((debug0<50)&&(debug0>20)&&(debugz>0))
					{
						for (p=200;p<255;p++)
						{
							mask[p][q]=0;
						}
					}
					//if (mask[150+debug0-1][q]==127){} //this line could potentially hide some bugs features
																						//if a slot dies at birth, it's not accounted for (frees up space)
					//else
					//{
						//debug0++;
						debug0++;
						if (mask[succumb][q]==24)
							{mask[150+debug0][q]=99;} //99=64+32+2+1
						else if (mask[alive][q]==0)
							{mask[150+debug0][q]=28;} //28=16+8+4
						else
							{mask[150+debug0][q]=8;} //119=127-8  //this should not appear
						debug0++;
						mask[150+debug0][q]=127;
						mask[150][q]=debug0;
					//}
						
					// erase slot
					posQ=mask[tip][q]; //tip is also going to be erased, need to store its value
					for(tmp=detect0; tmp<posQ+2; tmp++)
					{
						mask[tmp][q]=0;
					}
					
				}
			}
			// setting detect0 ON for slots that have been activated in the preceding j-loop
			else if (mask[detect1][q]==126)
			{
				mask[detect0][q]=126;
			}
			
			mask[alive][q]=0;
			mask[succumb][q]=0;
		}
			
	} // end of for-i loop
		
	if (0)//draw)
	{
		for (tmp=9; tmp<=stored; tmp++)
		{
			if (mask[tmp][0])
				ILI9341_DrawLine( mask[tmp][1], mask[tmp][0]-1, mask[tmp][3], mask[tmp][2]-1 );
		}
		for (tmp=254; tmp>=storedN; tmp--)
		{
			if (mask[tmp][4])
				ILI9341_DrawRectangle(mask[tmp][3],mask[tmp][2],3,3,1);
		}
	}
	
	//ILI9341_DrawLine( 20, 40, 60, 80 );
	
	LED_RGBOFF;
}

void prepare_slot1(char slot, uint8_t sprout_b, uint8_t merge_b1, uint8_t merge_b2, uint8_t s_height, uint8_t s_start, uint8_t s_finish)
{
	if (slot<40) // make start and end extern if possible
	{
		mask[detect0][slot]=0;           // set byte#0 to OFF to prevent slot from being processed at the end of this turn
		mask[detect1][slot]=126;
		
		mask[alive][slot]=24;
		mask[succumb][slot]=0;
		
		mask[byte4][slot]=126;
		mask[byte5][slot]=126;
		mask[byte6][slot]=0;

		
		mask[sprout][slot]=sprout_b;  // what body do I derive from
		mask[stored_alrd][slot]=0;
		
		mask[merge0][slot]=0;//merge_b1;  // what body do I merge into (on the left)  //useless, no convergence can be detected as the slot gets created
		mask[merge1][slot]=0;//merge_b2;  // what body do I merge into (on the right)
		
		mask[root][slot]=s_height;
		mask[tip][slot]=base;  // pos (tip byte) is the location of LAST (previous) added item
		
		mask[base][slot]=s_start;
		mask[base+1][slot]=s_finish;
		/*
		//debug
		mask[189][slot] = 84;
		mask[190][slot] = merge_b2;
		*/
		//debug
		char posQ;
		posQ=mask[150][slot];
		posQ++;
		mask[150+posQ][slot]=112+merge_b1; //112=64+32+16
		mask[150][slot]=posQ;

	}
}

void store_body0(char stored, char slot)
{
	/*
	mask[stored][0]=mask[height][slot];
	mask[stored][1]=(mask[height+1][slot]+mask[height+1][slot+1])>>1;
	mask[stored][2]=mask[height][slot]-mask[height][slot+1]+height;
	mask[stored][3]=(mask[mask[height][slot+1]][slot]+mask[mask[height][slot+1]][slot])>>1;
	
	mask[stored][4]=mask[sprout][slot];
	mask[stored][5]=mask[merge][slot];
	mask[stored][6]=mask[merge][slot+1];
	*/
	
	uint8_t pos;
	pos=mask[tip][slot];
	
	mask[stored][0]=mask[root][slot];
	
	mask[stored][1]=(mask[base][slot]+mask[base+1][slot])>>1;
	
	mask[stored][2]=mask[root][slot]+((pos-tip)/2);

	mask[stored][3]=(mask[pos][slot]+mask[pos+1][slot])>>1;
	
	mask[stored][4]=mask[sprout][slot];
	mask[stored][5]=mask[merge0][slot];
	mask[stored][6]=mask[merge1][slot];
	
	/*
	position 0: ordinate of segment's start-strip
	position 1: average abscissa of segment's start-strip
	position 2: ordinate of segment's end-strip
	position 3: average abscissa of segment's end-strip
	position 4: segment's father
	position 5: segment's left merger
	position 6: segment's right merger
	
	*/
}

void store_node0(char storedN, char Nsprout, char i, char horizontal_pos)
{
	char vertical_pos = i;
	
	mask[storedN][0]=0;
	mask[storedN][1]=0;
	
	mask[storedN][2]=vertical_pos;
	mask[storedN][3]=horizontal_pos;
	
	mask[storedN][4]=Nsprout;
	
	mask[storedN][5]=0;
	mask[storedN][6]=0;
	
	/*
	position 0: null for nodes
	position 1: null for nodes
	position 2: ordinate of node
	position 3: abscissa of node
	position 4: node's sprout (if DIVERGENCE node)
	position 5: null for nodes
	position 6: null for nodes
	
	*/
}

/*
void draw_segments000(void)
{

	//char stored_value[40];
	//char c = stored;
	
	//sprintf(stored_value,"stored: %d ",c);
	
	//ILI9341_DispString_EN(100,200,stored_value); 
	
	//while (1){}

	int count;
	uint8_t i;
	
	//for (i=9; i<stored; i++)
	for (i=9; i<240; i++)
	{ LED_RED;
		if (1)//(mask[i][0])
		{
			//count=10000000;
			count=10;
			ILI9341_DrawLine( mask[i][1], mask[i][0]-0, mask[i][3], mask[i][2]-0 );
			LED_BLUE;
			//debug
			while(count--){}
		}
		if (mask[i][0]==mask[i][2])
		{
			//mask [i][8]=112;
			//mask[0][7]=85;
			//mask[1][7]=i;
			//bwdisplay1();
			if ((mask[i][3]==0)||(mask[i][3]==0))
				mask [i][8]=14;
		}
	}
	
	//debug
	
	char debug1, debug2, debug3, debug4;
	debug3=0;
	debug4=0;
	for (i=8; i<40; i++)
	{
		if (mask[stag][i]>0)
		{
			
			debug1=mask[stag][i];
			debug2=mask[tip][debug1];
			
			ILI9341_DrawLine( mask[debug2][debug1], 150+debug3, mask[debug2+1][debug1], 150+debug3);
			
			debug3++;
			debug3++;
			
			while(count--){};
			count=1000000;
		}
		if (mask[stud][i]>0)
		{
			
			debug1=mask[stud][i];
			debug2=mask[tip][debug1];
			
			ILI9341_DrawLine( mask[debug2][debug1], 180+debug4, mask[debug2+1][debug1], 180+debug4);
			
			debug4++;
			debug4++;
			
			while(count--){};
			count=1000000;
		}
	}
	
	LED_RGBOFF;
}
*/
