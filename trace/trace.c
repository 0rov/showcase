#include "./trace/trace.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./bmp/bsp_bmp.h"


//extern uint8_t Itera_Threshold(uint16_t PIXEL_W,uint16_t PIXEL_H);
RESULT result;
SEARCH_AREA area;
TARGET_CONDITION condition={
	           
	20,        //Ŀ����С��ȣ�WIDTH_MIN
	20,        //Ŀ����С�߶ȣ�HEIGHT_MIN
	           
	320,       //Ŀ������ȣ�WIDTH_MAX
	240        //Ŀ�����߶ȣ�HEIGHT_MAX
};


/**************************************/
//��ȡĳ�����ɫ
static unsigned char ReadMux( uint16_t usX, uint16_t usY)
{
	uint16_t Camera_Data;
	uint16_t R,G,B;
	uint8_t mux;
	
	Camera_Data = ILI9341_LCD_ReadPoint( usX, usY );					//��ȡ��ɫ����
	R = (uint16_t)GETR_FROM_RGB16(Camera_Data);
	G = (uint16_t)GETG_FROM_RGB16(Camera_Data);
	B = (uint16_t)GETB_FROM_RGB16(Camera_Data);
	mux = (R*30+G*59+B*11)/100;
	//ͼ���ֵ��
			if( mux < 125)
			{
			  mux = 255;
			}
			else
			{
			  mux = 0;
			}		
	return mux;
}



/****************************************************/
//  Ѱ�Ҹ�ʴ����
//  x ����ʴ����x����
//  y ����ʴ����y����
//  condition ��TARGET_CONDITION�ṹ�壬���ϣ������ɫ������ֵ
//  area ��SEARCH_AREA�ṹ�壬���Ҹ�ʴ���ĵ�����
//  isBlack: isBlackΪ1��Ѱ�Һ�ɫĿ�ꣻisBlackΪ0��Ѱ�Ұ�ɫĿ�ꡣ
// 1���ҵ��˸�ʴ���ģ�x��yΪ��ʴ���ĵ����ꣻ0��û���ҵ���ʴ���ġ�
static int SearchCenter(unsigned int* x, unsigned int* y, const TARGET_CONDITION* condition, SEARCH_AREA* area, uint8_t isBlack)
{
	unsigned int i, j, k;
	uint8_t mux;
	unsigned int FailCount=0;
	unsigned int SpaceX, SpaceY;
	

	if(isBlack)
	{
		SpaceX = condition->WIDTH_MIN;
	  SpaceY = condition->HEIGHT_MIN;
		printf("����Ŀ��%d,%d,%d,%d\n",area->X_Start,area->X_End, area->Y_Start, area->Y_End );
		for(i=90; i<240; i+=SpaceY)
		{
			for(j=240-i ; j<80+i ; j+=SpaceX)
			{
				FailCount = 0;
				for(k=0; k<SpaceX+SpaceY; k++)
				{
					if(k<SpaceX)
						mux = ReadMux( j+k, i+SpaceY/2 );
					else
						mux = ReadMux( j+SpaceX/2, i+k-SpaceX );
					if(mux == 0)
						FailCount++;				
					if(FailCount>10)
						break;				
				}				
				if(k == SpaceX+SpaceY)
				{
					*x = j + SpaceX / 2;
					*y = i + SpaceY / 2;
					return 1;
				}				
			}			
		}
	}
	else
	{
		SpaceX = condition->WIDTH_MIN ;
	  SpaceY = condition->HEIGHT_MIN ;
		for(i=area->Y_Start; i<area->Y_End; i+=SpaceY)
		{
			for(j=area->X_Start; j<area->X_End; j+=SpaceX)
			{
				FailCount = 0;
				for(k=0; k<SpaceX+SpaceY; k++)
				{
					if(k<SpaceX)
						mux = ReadMux( j+k, i+SpaceY/2 );
					else
						mux = ReadMux( j+SpaceX/2, i+k-SpaceX );
					if(mux != 0)
						FailCount++;				
					if(FailCount>( (SpaceX+SpaceY) >> ALLOW_FAIL_PER ))
						break;				
				}				
				if(k == SpaceX+SpaceY)
				{
					*x = j + SpaceX / 2;
					*y = i + SpaceY / 2;
					return 1;
				}				
			}			
		}
	}
	return 0;		
}

/***************************************************/
// �Ӹ�ʴ�������ⸯʴ���õ��µĸ�ʴ����
//  oldX ����ǰ�ĸ�ʴ����x����
//  oldX ����ǰ�ĸ�ʴ����y����
//  condition ��TARGET_CONDITION�ṹ�壬���ϣ������ɫ������ֵ
//  result ��RESULT�ṹ�壬��ż����
// 1�����ɹ���0�����ʧ�ܡ�
 
static int Corrode(unsigned int oldX, unsigned int oldY, const TARGET_CONDITION* condition, RESULT* result, uint8_t isBlack )
{
	unsigned int Xmin, Xmax, Ymin, Ymax;
	unsigned int i;
	uint8_t mux;
	unsigned int FailCount=0;
	
	for(i=oldX; i>IMG_X; i--)
	{
		mux = ReadMux(i, oldY);
		if(isBlack)
		{
			if(mux == 0)
				FailCount++;
		}
		else
		{
			if(mux != 0)
				FailCount++;
		}		
		if(FailCount>(((condition->WIDTH_MIN+condition->WIDTH_MAX)>>2)>>ALLOW_FAIL_PER))
			break;
	}
	Xmin=i;
	
	FailCount=0;
	for(i=oldX; i<IMG_X+IMG_W; i++)
	{
		mux = ReadMux(i, oldY);
		if(isBlack)
		{
			if(mux == 0)
				FailCount++;
		}
		else
		{
			if(mux != 0)
				FailCount++;
		}		
		if(FailCount>(((condition->WIDTH_MIN+condition->WIDTH_MAX)>>2)>>ALLOW_FAIL_PER))
			break;
	}
	Xmax=i;
	
	FailCount=0;
	for(i=oldY; i>IMG_Y; i--)
	{
		mux = ReadMux(oldX,i);
		if(isBlack)
		{
			if(mux == 0)
				FailCount++;
		}
		else
		{
			if(mux != 0)
				FailCount++;
		}		
		if(FailCount>(((condition->HEIGHT_MIN+condition->HEIGHT_MAX)>>2)>>ALLOW_FAIL_PER))
			break;
	}
	Ymin=i;
	
	FailCount=0;
	for(i=oldY; i<IMG_Y+IMG_H; i++)
	{
		mux = ReadMux(oldX,i);
		if(isBlack)
		{
			if(mux == 0)
				FailCount++;
		}
		else
		{
			if(mux != 0)
				FailCount++;
		}		
		if(FailCount>(((condition->HEIGHT_MIN+condition->HEIGHT_MAX)>>2)>>ALLOW_FAIL_PER))
			break;
	}
	Ymax=i;
	
	FailCount=0;
	
	result->x = (Xmin + Xmax) / 2;
	result->y = (Ymin + Ymax) / 2;
	result->w = (Xmax - Xmin);
	result->h = (Ymax - Ymin);
	
	if( (result->w > condition->WIDTH_MIN) && (result->w < condition->WIDTH_MAX) &&
			(result->h > condition->HEIGHT_MIN) && (result->h < condition->HEIGHT_MAX)  )
		return 1;
	else
		return 0;
}


int Trace(const TARGET_CONDITION* condition, RESULT* result_final, SEARCH_AREA area, unsigned int isBlack)
{
	unsigned int i;
	static unsigned int x0, y0;
	RESULT result;
	
	if(SearchCenter(&x0, &y0, condition, &area, isBlack) == 0)
	{
		printf("�Ҳ���\n");
		return 0;
	}
	else
	{
		printf("��Χ%d,%d,%d,%d\n",area.X_Start,area.X_End,area.Y_Start,area.Y_End);
	}
	result.x = x0;
	result.y = y0;
	
	for(i=0; i<ITERATER_NUM; i++)
	{
		Corrode(result.x, result.y, condition, &result, isBlack);	//�Ӹ�ʴ�������ⸯʴ���õ��µĸ�ʴ����
		
	}
	
	if( Corrode(result.x, result.y, condition, &result, isBlack) )
	{
		x0 = result.x;
		y0 = result.y;
		result_final->x = result.x;
		result_final->y = result.y;
		result_final->w = result.w;
		result_final->h = result.h;

		return 1;
	}
	else
	{
		return 0;
	}
	
}
