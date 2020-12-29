#include "./file_access.h" 

#define RGB24TORGB16(R,G,B) ((unsigned short int)((((R)>>3)<<11) | (((G)>>2)<<5)	| ((B)>>3)))
	
static void showBmpHead(BITMAPFILEHEADER* pBmpHead)
{
    BMP_DEBUG_PRINTF("位图文件头:\r\n");
    BMP_DEBUG_PRINTF("文件大小:%ld\r\n",(*pBmpHead).bfSize);
    BMP_DEBUG_PRINTF("保留字:%d\r\n",(*pBmpHead).bfReserved1);
    BMP_DEBUG_PRINTF("保留字:%d\r\n",(*pBmpHead).bfReserved2);
    BMP_DEBUG_PRINTF("实际位图数据的偏移字节数:%ld\r\n",(*pBmpHead).bfOffBits);
		BMP_DEBUG_PRINTF("\r\n");	
}

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


BITMAPFILEHEADER bitHead1;
BITMAPINFOHEADER bitInfoHead1;

void load_image(char *pic1_name)
{
	WORD fileType;
	unsigned int read_num;
	FRESULT bmpres0;
	
	int width, height, l_width;
	width = bitInfoHead1.biWidth;
	height = bitInfoHead1.biHeight;
	
	l_width = WIDTHBYTES(width* bitInfoHead1.biBitCount);	

	if(l_width > 960)
	{
		BMP_DEBUG_PRINTF("\n 本图片太大，无法在液晶屏上显示 (<=320)\n");
		return;
	}
	
	//first pixel


	bmpres0 = f_open( &bmpfsrc , (char *)pic1_name, FA_OPEN_EXISTING | FA_READ);	
//-------------------------------------------------------------------------------------------------------//
	if(bmpres0 == FR_OK)
	{
		BMP_DEBUG_PRINTF("打开文件成功\r\n");
      
		f_read(&bmpfsrc,&fileType,sizeof(WORD),&read_num);     

		if(fileType != 0x4d42)
		{
			BMP_DEBUG_PRINTF("这不是一个 .bmp 文件!\r\n");
			return;
		}
		else
		{
			BMP_DEBUG_PRINTF("这是一个 .bmp 文件\r\n");	
		}        

		f_read(&bmpfsrc,&bitHead1,sizeof(tagBITMAPFILEHEADER),&read_num);        
		showBmpHead(&bitHead1);

		f_read(&bmpfsrc,&bitInfoHead1,sizeof(BITMAPINFOHEADER),&read_num);        
		showBmpInforHead(&bitInfoHead1);
	}    
	else
	{
		BMP_DEBUG_PRINTF("打开文件失败!错误代码：bmpres = %d \r\n", bmpres0);
		return;
	}  

		if (bitInfoHead1.biBitCount >= 24)
	{
		ILI9341_OpenWindow(0, 0, width, height);
		ILI9341_Write_Cmd (CMD_SetPixel ); 
			
	}    	
	else 
	{        
		BMP_DEBUG_PRINTF("这不是一个24位真彩色BMP文件！");
		return ;
	}
}


//new version allowing to uses the same gradient code
void load_pxlns2bff(void)
{
	int k, kk;
	int width, l_width;
	width = bitInfoHead1.biWidth;
	unsigned int read_num;
	
	l_width = WIDTHBYTES(width* bitInfoHead1.biBitCount);	
	
	for (k=0; k<3; k++)
	{
		f_lseek ( & bmpfsrc, bitHead1 .bfOffBits + ( k ) * l_width );		
		f_read ( & bmpfsrc, pColorData, l_width, & read_num );	
		
		for (kk=0; kk<960; kk++)
		{buffer[k][kk]=pColorData[kk];}
	}
}

void pic2bff_update(int i)
{
	int kk;
	int width, l_width;
	width = bitInfoHead1.biWidth;
	unsigned int read_num;
	
	l_width = WIDTHBYTES(width* bitInfoHead1.biBitCount);	
	
	f_lseek ( & bmpfsrc, bitHead1 .bfOffBits + ( i ) * l_width );		
	f_read ( & bmpfsrc, pColorData, l_width, & read_num );	
	
	for (kk=0; kk<960; kk++)
	{buffer[lnorder[2]][kk]=pColorData[kk];}
}

void close_image(void){f_close(&bmpfsrc);}
	

void show_mybmp ( char * pic_name, char flipv )
{
	int i, j, k;
	int width, height, l_width;

	BYTE red,green,blue;
	BITMAPFILEHEADER bitHead;
	BITMAPINFOHEADER bitInfoHead;
	WORD fileType;

	unsigned int read_num;

	bmpres = f_open( &bmpfsrc , (char *)pic_name, FA_OPEN_EXISTING | FA_READ);	
//-------------------------------------------------------------------------------------------------------//
	if(bmpres == FR_OK)
	{
		
		BMP_DEBUG_PRINTF("打开文件成功\r\n");
    
		f_read(&bmpfsrc,&fileType,sizeof(WORD),&read_num);     

		if(fileType != 0x4d42)
		{
			BMP_DEBUG_PRINTF("这不是一个 .bmp 文件!\r\n");
			return;
		}
		else
		{
			BMP_DEBUG_PRINTF("这是一个 .bmp 文件\r\n");	
		}        

		f_read(&bmpfsrc,&bitHead,sizeof(tagBITMAPFILEHEADER),&read_num);        
		showBmpHead(&bitHead);

		f_read(&bmpfsrc,&bitInfoHead,sizeof(BITMAPINFOHEADER),&read_num);        
		showBmpInforHead(&bitInfoHead);
	}    
	else
	{
		BMP_DEBUG_PRINTF("打开文件失败!错误代码：bmpres = %d \r\n",bmpres);
		return;
	}    
//-------------------------------------------------------------------------------------------------------//
	width = bitInfoHead.biWidth;
	height = bitInfoHead.biHeight;

	l_width = WIDTHBYTES(width* bitInfoHead.biBitCount);	

	if(l_width > 960)
	{
		BMP_DEBUG_PRINTF("\n 本图片太大，无法在液晶屏上显示 (<=320)\n");
		return;
	}

	ILI9341_OpenWindow(0, 0, width, height);
	ILI9341_Write_Cmd (CMD_SetPixel ); 

	if( bitInfoHead.biBitCount >= 24 )
	{
		for ( i = 0; i < height; i ++ )
		{
			if (flipv==0)
				f_lseek ( & bmpfsrc, bitHead .bfOffBits + ( height - i - 1 ) * l_width );	
			else
				f_lseek ( & bmpfsrc, bitHead .bfOffBits + ( i + 1 ) * l_width );	
			
			f_read ( & bmpfsrc, pColorData1, l_width, & read_num );				

			for(j=0; j<width; j++) 											   //一行有效信息
			{
				k = j*3;																	 //一行中第K个像素的起点
				red = pColorData1[k+2];
				green = pColorData1[k+1];
				blue = 	pColorData1[k];
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

void show_bmpdiff ( char * pic1_name, char * pic2_name )
{
	char flipv=0;
	
	int i, j, k;
	int width, height, l_width;

	BYTE red,green,blue;
	BITMAPFILEHEADER bitHead1, bitHead2;
	BITMAPINFOHEADER bitInfoHead1, bitInfoHead2;
	WORD fileType1, fileType2;
	FRESULT bmpres1, bmpres2;

	unsigned int read_num;

	bmpres1 = f_open( &bmpfsrc1 , (char *)pic1_name, FA_OPEN_EXISTING | FA_READ);	
	bmpres2 = f_open( &bmpfsrc2 , (char *)pic2_name, FA_OPEN_EXISTING | FA_READ);	
//-------------------------------------------------------------------------------------------------------//
	if((bmpres1 == FR_OK)&&(bmpres2 == FR_OK))
	{
		
		BMP_DEBUG_PRINTF("打开文件成功\r\n");
    
		f_read(&bmpfsrc1,&fileType1,sizeof(WORD),&read_num);   
		f_read(&bmpfsrc2,&fileType2,sizeof(WORD),&read_num);   

		if((fileType1 != 0x4d42)||(fileType2 != 0x4d42))
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
		
		f_read(&bmpfsrc2,&bitHead2,sizeof(tagBITMAPFILEHEADER),&read_num);        
		showBmpHead(&bitHead2);

		f_read(&bmpfsrc2,&bitInfoHead2,sizeof(BITMAPINFOHEADER),&read_num);        
		showBmpInforHead(&bitInfoHead2);
	}    
	else
	{
		BMP_DEBUG_PRINTF("打开文件失败!错误代码：bmpres = %d \r\n",bmpres);
		return;
	}    
//-------------------------------------------------------------------------------------------------------//
	//the following assumes the 2 pics have the same size
	width = bitInfoHead1.biWidth;
	height = bitInfoHead1.biHeight;

	l_width = WIDTHBYTES(width* bitInfoHead1.biBitCount);	

	if(l_width > 960)
	{
		BMP_DEBUG_PRINTF("\n 本图片太大，无法在液晶屏上显示 (<=320)\n");
		return;
	}

	ILI9341_OpenWindow(0, 0, width, height);
	ILI9341_Write_Cmd (CMD_SetPixel ); 

	if( bitInfoHead1.biBitCount >= 24 )
	{
		for ( i = 0; i < height; i ++ )
		{
			if (flipv==0)
			{
				f_lseek ( & bmpfsrc1, bitHead1 .bfOffBits + ( height - i - 1 ) * l_width );
				f_lseek ( & bmpfsrc2, bitHead2 .bfOffBits + ( height - i - 1 ) * l_width );
			}				
			else
			{
				f_lseek ( & bmpfsrc1, bitHead1 .bfOffBits + ( i + 1 ) * l_width );
				f_lseek ( & bmpfsrc2, bitHead2 .bfOffBits + ( i + 1 ) * l_width );
			}
			
			f_read ( & bmpfsrc1, pColorData1, l_width, & read_num );	
			f_read ( & bmpfsrc2, pColorData2, l_width, & read_num );	

			for(j=0; j<width; j++) 											   //一行有效信息
			{
				k = j*3;																	 //一行中第K个像素的起点
				red = abs(pColorData1[k+2]-pColorData2[k+2]);
				green = abs(pColorData1[k+1]-pColorData2[k+1]);
				blue = 	abs(pColorData1[k]-pColorData2[k]);
				ILI9341_Write_Data ( RGB24TORGB16 ( red, green, blue ) ); //写入LCD-GRAM
			}            			
		}        		
	}    	
	else 
	{        
		BMP_DEBUG_PRINTF("这不是一个24位真彩色BMP文件！");
		return ;
	}
	
	f_close(&bmpfsrc1);  
	f_close(&bmpfsrc2);  
  
}

unsigned int mybw;
int cam2SD_prepare(char * filename)
{
	uint16_t Width = Bsize; 
	uint16_t Height = Hsize;
	
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
	long width;
	long height;

	file_size = (long)Width * (long)Height * 3 + Height*(Width%4) + 54;		

	header[2] = (unsigned char)(file_size &0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;
	
	width=Width;	
	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) &0x000000ff;
	header[20] = (width >> 16) &0x000000ff;
	header[21] = (width >> 24) &0x000000ff;

	height = Height;
	header[22] = height &0x000000ff;
	header[23] = (height >> 8) &0x000000ff;
	header[24] = (height >> 16) &0x000000ff;
	header[25] = (height >> 24) &0x000000ff;

	bmpres = f_open( &bmpfsrc , (char*)filename, FA_CREATE_ALWAYS | FA_WRITE );

	f_close(&bmpfsrc);
		
	bmpres = f_open( &bmpfsrc , (char*)filename,  FA_OPEN_EXISTING | FA_WRITE);

	if ( bmpres == FR_OK )
	{    
		bmpres = f_write(&bmpfsrc, header,sizeof(unsigned char)*54, &mybw);		

	}	
	else
		return -1;
	
	return 0;
}

void cam2SD_writepxl(BYTE r, BYTE g, BYTE b)
{ 
		bmpres = f_write(&bmpfsrc, &b,sizeof(unsigned char), &mybw);
		bmpres = f_write(&bmpfsrc, &g,sizeof(unsigned char), &mybw);
		bmpres = f_write(&bmpfsrc, &r,sizeof(unsigned char), &mybw);		
}

int matrix2picture(char factor, char matrix_id, char * filename)
{
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
	
	int i,j;
	char ii, jj;
	long file_size;     
	long width;
	long height;
	unsigned int mybw;

	BYTE bytesave;
	char c;
	
	uint16_t Width=Bsize*factor;
	uint16_t Height=Hsize*factor;
	
	file_size = (long)Width * (long)Height * 3 + Height*(Width%4) + 54;		

	header[2] = (unsigned char)(file_size &0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;
	
	width=Width;	
	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) &0x000000ff;
	header[20] = (width >> 16) &0x000000ff;
	header[21] = (width >> 24) &0x000000ff;
	
	height = Height;
	header[22] = height &0x000000ff;
	header[23] = (height >> 8) &0x000000ff;
	header[24] = (height >> 16) &0x000000ff;
	header[25] = (height >> 24) &0x000000ff;
		
	bmpres = f_open( &bmpfsrc , (char*)filename, FA_CREATE_ALWAYS | FA_WRITE );
	
	f_close(&bmpfsrc);
		
	bmpres = f_open( &bmpfsrc , (char*)filename,  FA_OPEN_EXISTING | FA_WRITE);

	if ( bmpres == FR_OK )
	{    
		bmpres = f_write(&bmpfsrc, header,sizeof(unsigned char)*54, &mybw);		

		for(i=Height-1; i>=0; i--)	
		//for(i=0; i<Height; i++)					
		{
			for (ii=0;ii<factor;ii++)
			{
				for(j=0; j<40; j++)  
				{
					for (jj=0;jj<factor;jj++)
					{
						if(matrix_id)
							bytesave = mask[i][j];
						else
							bytesave = bwdiff[i][j];
						
						for (int k=0; k<8; k++)
						{
							if (bytesave&128)
								c=255;
							else
								c=0;
							
							bmpres = f_write(&bmpfsrc, &c,sizeof(unsigned char), &mybw);
							bmpres = f_write(&bmpfsrc, &c,sizeof(unsigned char), &mybw);
							bmpres = f_write(&bmpfsrc, &c,sizeof(unsigned char), &mybw);
							
							bytesave=(bytesave<<1);
						}
					}
				}
			}
		}

		f_close(&bmpfsrc); 
		
		return 0;
		
	}	
	else
		return -1;
}

int textfile_save(int length, char * content, char * filename)
{	
	unsigned int write_txt;
	//int size = sizeof(content);
	char txtpres;

	txtpres = f_open( &bmpfsrc , (char*)filename, FA_CREATE_ALWAYS | FA_WRITE );

	f_close(&bmpfsrc);
		
	txtpres = f_open( &bmpfsrc , (char*)filename,  FA_OPEN_EXISTING | FA_WRITE);

	if ( txtpres == FR_OK )
	{    
		
		//bmpres = f_write(&bmpfsrc, header,sizeof(unsigned char)*54, &mybw);		
		txtpres = f_write(&bmpfsrc, content,sizeof(unsigned char)*length, &write_txt);	
		
		//bmpres = f_write(&bmpfsrc, &b,sizeof(unsigned char), &mybw);

		f_close(&bmpfsrc); 
		
		return 0;	
	}	
	else
		return -1;
}

void textfile_read(int start, int length, char * content, char * filename)
{
	char txtpres;
	unsigned int read_txt;

	txtpres = f_open( &bmpfsrc , (char *)filename, FA_OPEN_EXISTING | FA_READ);	
//-------------------------------------------------------------------------------------------------------//
	if(txtpres == FR_OK)
	{
		//file is present?
     
		f_lseek (&bmpfsrc, start ); //value is an array index, starting at 0
		  
		f_read(&bmpfsrc,content,sizeof(BYTE)*length,&read_txt);  

	}    
	else
	{
		//file is absent?	
		return;
	}    
	
	f_close(&bmpfsrc);  
  
}

void newbmp_index_old(int * name_count_pt, char * singleframe, char * frameBD, char * BDsegments, char * debugpanel)
{
	int NC=*name_count_pt;
	
	sprintf(singleframe,"0:SF_%d_frame.bmp",NC);
	sprintf(frameBD,"0:SF_%d_brdrs.bmp",NC);
	sprintf(BDsegments,"0:SF_%d_segmt.bmp",NC);
	sprintf(debugpanel,"0:SF_%d_dbpnl.bmp",NC);
						
	bmpres = f_open( &bmpfsrc , singleframe, FA_OPEN_EXISTING | FA_READ);	
	if (bmpres != FR_OK)
		bmpres = f_open( &bmpfsrc , frameBD, FA_OPEN_EXISTING | FA_READ);
	if (bmpres != FR_OK)
		bmpres = f_open( &bmpfsrc , BDsegments, FA_OPEN_EXISTING | FA_READ);
	if (bmpres != FR_OK)
		bmpres = f_open( &bmpfsrc , debugpanel, FA_OPEN_EXISTING | FA_READ);
	
	char i;
	
	while(bmpres == FR_OK)
	{
		for (i=0; i<1; i++)
		{
			f_close(&bmpfsrc); 
			
			NC++; 
			sprintf(singleframe,"0:SF_%d_frame.bmp",NC);
			bmpres = f_open( &bmpfsrc , singleframe, FA_OPEN_EXISTING | FA_READ);	
			if (bmpres == FR_OK)
				break;
			sprintf(frameBD,"0:SF_%d_brdrs.bmp",NC);
			bmpres = f_open( &bmpfsrc , frameBD, FA_OPEN_EXISTING | FA_READ);
			if (bmpres == FR_OK)
				break;
			sprintf(BDsegments,"0:SF_%d_segmt.bmp",NC);
			bmpres = f_open( &bmpfsrc , BDsegments, FA_OPEN_EXISTING | FA_READ);
			if (bmpres == FR_OK)
				break;
			sprintf(debugpanel,"0:SF_%d_dbpnl.bmp",NC);
			bmpres = f_open( &bmpfsrc , debugpanel, FA_OPEN_EXISTING | FA_READ);
			if (bmpres == FR_OK)
				break;
		}
	}
	*name_count_pt = NC;
	
	sprintf(singleframe,"0:SF_%d_frame.bmp",NC);
	sprintf(frameBD,"0:SF_%d_brdrs.bmp",NC);
	sprintf(BDsegments,"0:SF_%d_segmt.bmp",NC);
	sprintf(debugpanel,"0:SF_%d_dbpnl.bmp",NC);
}

#include <stdarg.h>
#include <stdio.h>

int newbmp_index(char * locus, char nb_arg, ...)//char * singleframe, char * frameBD, char * BDsegments, char * debugpanel)
{
	int i;
	int index=0;
	
	char*ptrlist[20];
	
	char string[40],str_start[40],number[40], number1[40],empty[40];
	char *str_end_ptr;
	
	va_list ap;
	
	va_start(ap, nb_arg);
	
	for (i=0; i<nb_arg; i++)
	{
		ptrlist[i]=va_arg(ap, char *);
	}

	sprintf(number, "%d",index); 
	
	//initialize
	for (i=0; i<nb_arg; i++)
	{
		sprintf(number, "%d",index);
		
		strlcpy(str_start, ptrlist[i],40); 
		//sprintf(str_start, ptrlist[i]); 

		strtok_r(str_start, locus, &str_end_ptr);
		
		strcat(number,str_end_ptr);
		
		strcat(str_start,number);
		//strcat(str_start,str_end_ptr);
		strcpy(string, str_start);
		
		bmpres = f_open( &bmpfsrc , string, FA_OPEN_EXISTING | FA_READ);	
		
		if (bmpres==FR_OK)
			break;
	}

	while(bmpres == FR_OK)
	{
		index++;
		sprintf(number, "%d",index);
		
		for (i=0; i<nb_arg; i++)
		{
			sprintf(number, "%d",index);
			
			//strcpy(str_start, ptrlist[i]);
			strlcpy(str_start, ptrlist[i],40);
			strtok_r(str_start, locus, &str_end_ptr);
			
			strcat(number,str_end_ptr);
			
			strcat(str_start,number);
			//strcat(str_start,str_end_ptr);
			strcpy(string, str_start);
			
		
			bmpres = f_open( &bmpfsrc , string, FA_OPEN_EXISTING | FA_READ);	
			
			if (bmpres==FR_OK)
				break;
		}
	}
	
	//rename original objects
	sprintf(number, "%d",index);
	for (i=0; i<nb_arg; i++)
	{
		sprintf(number, "%d",index);
		
		strcpy(str_start, ptrlist[i]);
		strtok_r(str_start, locus, &str_end_ptr);
		
		strcat(number,str_end_ptr); 
		strcat(str_start,number);   
		//strcat(str_start,str_end_ptr);  
		strcpy(ptrlist[i], str_start);
	}
	return index;
}

int min(int days, int arg_count, ...)
{
	int i;
	
	int min, a;
	
	va_list ap;
	
	va_start(ap, arg_count);
	
	min = va_arg(ap, int);
	
	for (i = 2; i <= arg_count; i++)  //2, because first argument jas already been called
		if ((a = va_arg(ap, int)) < min)
			min = a;
	
	va_end(ap);
		
	return min;
}

void scan_bmps ( char* path ) // path: Start node to be scanned (***also used as work area***) */
{	
	char path1[40];

	FRESULT res;
	DIR dir;
	UINT i;
	static FILINFO fno;

	char *fn;   /* This function is assuming non-Unicode cfg. */
	#if _USE_LFN
	static char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof lfn;
	#endif


	res = f_opendir(&dir, path);                       /* Open the directory */
	if (res == FR_OK) {
		for (int k=0; k<10000; k++) 
		{
			//for (int kk=0; kk<1; kk++)
			{
				res = f_readdir(&dir, &fno);                   /* Read a directory item */
				if (res != FR_OK || fno.fname[0] == 0) 
				{
					break;  /* Break on error or end of dir */
				}
				
				#if _USE_LFN
				fn = *fno.lfname ? fno.lfname : fno.fname;
				#else
				fn = fno.fname;
				#endif
				
				if (fno.fattrib & AM_DIR) // It is a directory 
				{                    
					i = strlen(path);
					sprintf(&path[i], "/%s", fno.fname);
					//res = scan_files(path);                    // Enter the directory 
					if (res != FR_OK) break;
					path[i] = 0;
				} 
				else  // It is a file. 
				{ 
					 //if (k==5)
					 {
						 char test1[10];
						 memcpy(test1,&fno.fname[9],4*sizeof(*fno.fname));
						 char test2[5]="BMP";
						 //if (strcmp(test1,"BMP"))
						 if (strcmp(test1,test2)==0)
						 {
							 LED_YELLOW;
							 show_mybmp ( fno.fname, 0);
							 //ILI9341_DispString_EN(50,220,fno.fname);
							 ILI9341_DispString_EN(50,220,fn);
							 sprintf(path1,"%s%s", path, fn);
							 ILI9341_DispString_EN(50,50,path1);
							 while(1)
							 {
								 if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
									 {break;}
							 }
						 }
					 }                      
				}
			}	
		}
		f_closedir(&dir);
	}

	//return res;
}
