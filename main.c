/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   摄像头火眼ov7725测试例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./ov7725/bsp_ov7725.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./led/bsp_led.h"   
#include "./usart/bsp_usart.h"
#include "./key/bsp_key.h"  
#include "./systick/bsp_SysTick.h"
#include "./bmp/bsp_bmp.h"
#include "ff.h"
//#include "./trace/trace.h"
#include "bd_dt.h"

#include "./ImgP_rs.h"
#include "./file_access.h" 


extern uint8_t Ov7725_vsync;

unsigned int Task_Delay[NumOfTask]; 

extern OV7725_MODE_PARAM cam_mode;

FATFS fs;													/* FatF、



s文件系统对象 */
FRESULT res_sd;                /* 文件操作结果 */


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */

const int H=240;
const int B=320;
uint16_t Hsize=240;
uint16_t Bsize=320;
const int pich=17;
const int picw=320;

double  myPI = 3.1415926535;

int main(void) 	
{		
	
	float frame_count = 0;
	uint8_t retry = 0;
	uint8_t BW = 0, Flag = 0;
//  SEARCH_AREA area = {IMG_X, IMG_X+IMG_W, IMG_Y, IMG_Y+IMG_H};
	
	/* 液晶初始化 */
	ILI9341_Init();
	ILI9341_GramScan ( 3 );
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
	
	/********显示字符串示例*******/
  ILI9341_DispStringLine_EN(LINE(0),"BH OV7725 Test Demo");

	USART_Config();
	LED_GPIO_Config();
	Key_GPIO_Config();
	SysTick_Init();
	
	
	/*挂载sd文件系统*/
	res_sd = f_mount(&fs,"0:",1);
	if(res_sd != FR_OK)
	{
		printf("\r\n请给开发板插入已格式化成fat格式的SD卡。\r\n");
	}
	
	printf("\r\n ** OV7725摄像头实时液晶显示例程** \r\n"); 
	
	/* ov7725 gpio 初始化 */
	OV7725_GPIO_Config();
	
	LED_BLUE;
	/* ov7725 寄存器默认配置初始化 */
	while(OV7725_Init() != SUCCESS)
	{
		retry++;
		if(retry>5)
		{
			printf("\r\n没有检测到OV7725摄像头\r\n");
			ILI9341_DispStringLine_EN(LINE(2),"No OV7725 module detected!");
			while(1);
		}
	}


	/*根据摄像头参数组配置模式*/
	OV7725_Special_Effect(cam_mode.effect);
	/*光照模式*/
	OV7725_Light_Mode(cam_mode.light_mode);
	/*饱和度*/
	OV7725_Color_Saturation(cam_mode.saturation);
	/*光照度*/
	OV7725_Brightness(cam_mode.brightness);
	/*对比度*/
	OV7725_Contrast(cam_mode.contrast);
	/*特殊效果*/
	OV7725_Special_Effect(cam_mode.effect);
	
	/*设置图像采样及模式大小*/
	OV7725_Window_Set(cam_mode.cam_sx,
														cam_mode.cam_sy,
														cam_mode.cam_width,
														cam_mode.cam_height,
														cam_mode.QVGA_VGA);

	/* 设置液晶扫描模式 */
	ILI9341_GramScan( cam_mode.lcd_scan );
	
	
	
	ILI9341_DispStringLine_EN(LINE(2),"OV7725 initialize success!");
	printf("\r\nOV7725摄像头初始化完成\r\n");
	
	Ov7725_vsync = 0;
	
/*
		uint16_t Red,Gre,Blu;
		u8 pastim[pich][picw][3];
*/

uint8_t bwdiff[240][40];
uint8_t baseimg[240][40];

/*
for (int i=0; i<240; i++)
{ 
	for (int j=0; j<40; j++)
	{
		if (j<40)
			baseimg[i][j]=255;
		else
			baseimg[i][j]=0;
	}
}
*/


	while(1)
	{
		u16 Camera_Data;
		
		/*接收到新图像进行显示*/
		if( Ov7725_vsync == 2 )
		{
			frame_count++;
			FIFO_PREPARE;  			/*FIFO准备*/					
			ImagDisp(cam_mode.lcd_sx,
								cam_mode.lcd_sy,
								cam_mode.cam_width,
								cam_mode.cam_height);			/*采集并显示*/
			
			//pastim[1][1][1]=1;
/*			FIFO_PREPARE;
			for (int i=0;i<H;i++)
				{
					for (int j=0;j<B;j++)
					{
						//FIFO_PREPARE;
						if ((i<pich)&&(j<picw))
						{
						READ_FIFO_PIXEL(Camera_Data);
						Red = (uint16_t)GETR_FROM_RGB16(Camera_Data);
						Gre = (uint16_t)GETG_FROM_RGB16(Camera_Data);
						Blu = (uint16_t)GETB_FROM_RGB16(Camera_Data);
						pastim[i][j][0]=Red;
						pastim[i][j][1]=Gre;
						pastim[i][j][2]=Blu;
						}
					}
				}  
				
*/
			
			Ov7725_vsync = 0;			
//			LED1_TOGGLE;

		}
		
		/*检测按键*/
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{		
			static int name_count = 0;
			char diffimage[40],name1[40],name2[40];
			char singleframe[40], frameBD[40], BDsegments[40], debugpanel[40];
			
			char divisions = 1;
			tmp_storage_end = glb_dbgpnl-15;
			
			secnd_slotline=tmp_storage_end;
			if (divisions==2)
			{
				secnd_slotline=tmp_storage_end/2;
			}
			else if (divisions ==3)
			{
				secnd_slotline=tmp_storage_end/3;
				third_slotline=2*tmp_storage_end/3;
			}
			max_slotlng=secnd_slotline-base-5;
			
			loc_pnlhlf = (Hsize-glb_dbgpnl)/2;
			glb_pnlhlf = glb_dbgpnl+loc_pnlhlf;
			loc_pnlend = Hsize-glb_dbgpnl-11;
			loc_ersflg = fmin(0.9*loc_pnlhlf, loc_pnlhlf-10);
			
			char div[5];
			strcpy(div, "!");
			
			name_count=0;
			
			char string[40],str_start[40],number[40];
			char *str_end_ptr;
			
			sprintf(singleframe,"0:SF_!_frame.bmp");
			sprintf(frameBD,"0:SF_!_brdrs.bmp");
			sprintf(BDsegments,"0:SF_!_segmt.bmp");
			sprintf(debugpanel,"0:SF_!_dbpnl.bmp");
			
			//finding last saved object's number
			
			name_count=newbmp_index(div, 4, singleframe, frameBD, BDsegments, debugpanel);			

			FIFO_PREPARE;
			//Camera_Shot(0,0,LCD_X_LENGTH,LCD_Y_LENGTH,singleframe);
			
			//maskfill();
			
			uint8_t block=1;
			uint8_t draw=1;
			
			char path[40]="0:";
			//scan_files(path);
			//scan_bmps(path);
			//ILI9341_DispString_EN(50,50,path);
			//show_mybmp ( path, 0);
			
			
			int counter=0;
			/*
			char filen[25]="absorbancetest1.txt";
			char mytext[3];
			char timemachine=0;
			int ariadne=0;

			freepic=0;
			bwpic=1;
			savepic=0;
			showpic=0;
			multidimensional_test(2, 2, mytext, &ariadne, filen);
			
			//display_value(ariadne, 100, 100);
			
			LED_YELLOW;
			while(1){}
			//*/
			while (1)    // border detection
			{
				FIFO_PREPARE;
				//Screen_Shot( 0, 0, Bsize, Hsize, name1);
				freepic=1;
				bwpic=0;
				savepic=1;
				showpic=1;
								
				//run_dblxprt(&stag, 1, 99);
				
				border_det3();
				Ov7725_vsync = 0;
				
				if (draw)
					draw_segments();
				
				maindebug(singleframe, frameBD, BDsegments, debugpanel);
			}
			
			while(1)
			{
			//name_count=0;
			
			

			//LED_BLUE;
				
			//printf("\r\n正在截图...");
			
			//ILI9341_GramScan ( cam_mode.lcd_scan );		
			
				
			//FIFO_PREPARE;
			//Camera_Shot(0,0,LCD_X_LENGTH,LCD_Y_LENGTH,name2);
			
				
			//name_count++; 
			//sprintf(name,"0:gray_%d.bmp",name_count);
			
			//Ov7725_vsync = 0;
			
			//frame_count++;
			
			//for (int yo=0; yo<1000; yo++)
					//FIFO_PREPARE;
			
			//LED_RED;
			
			
			
			//Frame_difference("0:gray_1.bmp","0:gray_2.bmp","0:seqdiff.bmp");
			//Frame_difference(name1,name2,"0:seqdiff.bmp");
			
			
			char loopcntr=0;
			char phrase[40];
			
			
			LED_GREEN;
			
			while (loopcntr<10)
			{	
				LED_GREEN;
				name_count++; 
				sprintf(name2,"0:gray_%d.bmp",name_count);
				
				sprintf(diffimage,"0:diff_%d.bmp",name_count);
				
				FIFO_PREPARE;
				
				Fast_difference(name1,name2,diffimage);

			//bwfill();
				clearstreaks();
		
				findstreaks();
				
				ILI9341_DispStringLine_EN(LINE(0),phrase);
				
				drawstreaks();
				
				sprintf(name1,"0:gray_%d.bmp",name_count);
				
				loopcntr++;
				
				sprintf(phrase,"lap number: %d",loopcntr);

				ILI9341_DispStringLine_EN(LINE(0),phrase);
				
				Ov7725_vsync = 0;
			}
			
			LED_RED;
			
			intersections();
			
			drawstreaks();

			cnvyrbase();
			
			whitensector(left,0,right,0);
			
			bwdisplay1();

			while(1)
			{
				LED_PURPLE;
			
			if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
				{
					LED_RED;
					//intersections();
					//cnvyrbase();
					//whitensector(left,0,right,0);
					cnvdetected=1;
					while (1)
					{
						name_count++; 
						sprintf(name2,"0:gray_%d.bmp",name_count);
						sprintf(diffimage,"0:diff_%d.bmp",name_count);
						FIFO_PREPARE;
						
						Fast_difference(name1,name2,diffimage);
						
						bwdisplay();
						
						if (white_cntr>3000)
						{
							sprintf(phrase,"object detected: white = %d",white_cntr);
							ILI9341_DispStringLine_EN(LINE(0),phrase);							
						}
						
						sprintf(name1,"0:gray_%d.bmp",name_count);
						
						Ov7725_vsync = 0;
					}
				}
			}

			
			//while(1)
			//	LED_RED;
			
			//bwdisplay();
			//bwdisplay0();
			//bwdisplay1();
			
			//LED_GREEN;
			
			//LCD_Show_BMP ( 0, 0, diffimage );
			
			
			sprintf(name1,"0:gray_%d.bmp",name_count);
			
			Ov7725_vsync = 0;
	  	}
			
			
			/*
			if(Screen_Shot(0,0,LCD_X_LENGTH,LCD_Y_LENGTH,name) == 0)
			{
				printf("\r\n截图成功！");
				LED_GREEN;
			}
			else
			{
				printf("\r\n截图失败！");
				LED_RED;
			}
			*/
		}
		
		//检测按键
				if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
				frame_count++;
				FIFO_PREPARE; 
				u16 Camera_Data=0;
				//static uint16_t R,G,B;
				//static u8 pastim[240][320][3];
				
					
			
			//BW = Itera_Threshold(320,240);
		  while(1)
	   {
			/*接收到新图像进行显示*/

			if( Ov7725_vsync == 2 )
			{ 
				frame_count++;
				FIFO_PREPARE; 
				ILI9341_OpenWindow(cam_mode.lcd_sx,
									cam_mode.lcd_sy,
									cam_mode.cam_width,
									cam_mode.cam_height);
				ILI9341_Write_Cmd ( CMD_SetPixel );	
/*				
				for (int i=0;i<H;i++)
				{
					for (int j=0;j<B;j++)
					{
						Red=pastim[i%pich][j%picw][0];
						Gre=pastim[i%pich][j%picw][1];
						Blu=pastim[i%pich][j%picw][2];
						uint16_t mydata=((Red>>3<<11)|(Gre>>2<<5)|(Blu>>3));
						ILI9341_Write_Data(mydata);
					//	ILI9341_Write_Cmd ( CMD_SetPixel );	
					}
				}  
				
*/				
				//frame_count++;
				//FIFO_PREPARE;  			//FIFO准备				
/*				ImagDisp_BW(cam_mode.lcd_sx,
									cam_mode.lcd_sy,
									cam_mode.cam_width,
									cam_mode.cam_height,130);	*/		//采集并显示
	/*		  if(Flag == 0)
				{				
					if(Trace(&condition, &result, area, 0))
					{
						ILI9341_DrawTrapezium( result.x, result.y, result.w,  result.h);
						//ILI9341_DrawRectangle( result.x-2, result.y-2,result.x+2, result.y+2, 0);
						ILI9341_DrawCircle ( result.x, result.y, 5, 1 );
						area.X_Start = result.x - ((result.w)>>1);
						area.X_End   = result.x + ((result.w)>>1);
						area.Y_Start = result.y - ((result.h)>>1);
						area.Y_End   = result.y + ((result.h)>>1);		
						Flag = 1;
						printf("\r\n找到目标区域");
						printf("%d,%d,%d,%d\n",area.X_Start,area.X_End, area.Y_Start, area.Y_End );
						LED_GREEN;
					}
			  }
				else
				{
					if(Trace(&condition, &result, area, 1))
					{
						printf("\r\n找到目标");
						ILI9341_DrawRectangle( result.x - ((result.w)>>1), result.y - ((result.h)>>1), result.w,  result.h, 0);
						//ILI9341_DrawRectangle( result.x-2, result.y-2,result.x+2, result.y+2, 0);
						ILI9341_DrawCircle ( result.x, result.y, 5, 1 );
				    LED_RED;
					}
					else
					{
						printf("\r\n找不到目标");
						LED_GREEN;
					}
				}  */
				Ov7725_vsync = 0;			
		//			LED1_TOGGLE;
		   }
		}
	}
		
		/*每隔一段时间计算一次帧率*/
		if(Task_Delay[0] == 0)  
		{			
			printf("\r\nframe_ate = %.2f fps\r\n",frame_count/10);
			frame_count = 0;
			Task_Delay[0] = 10000;
		}		
	}
}




/*********************************************END OF FILE**********************/

