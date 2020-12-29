
#include "./ImgP_rs.h"

void buffer_pixelgradient(int j, int ii, int iii, int i)
{	
	int jj;
	short indi_pos, indi_neg, indj_pos, indj_neg;
	char nb_subtractions=0;
	
	
	a1=0;
	
	jj=j*3;	

	{
		// simplified, for 2 subtractions
		// this framework centers the result on the pixel (no shift)
		
					indi_pos=(iii+0+1)%3;//(index_i[kk]>>1))%3; //previously +1 instead of +0, not the right lines...
					indi_neg=(iii+0-1)%3;//(index_i[kk]>>1))%3;
					indj_pos=(j+1)*3;
					indj_neg=(j-1)*3;
					
					
					if ((i>0)&&(i<Hsize))
					{	
						a1+=abs(buffer[indi_pos][jj+2]-buffer[indi_neg][jj+2]);
						a1+=abs(buffer[indi_pos][jj+1]-buffer[indi_neg][jj+1]);
						a1+=abs(buffer[indi_pos][jj]-buffer[indi_neg][jj]);
						nb_subtractions++;
					}
					
					if ((j>0)&&(j<Bsize))
					{
						//jj+=2*3;  // index_j[1]
						a1+=abs(buffer[ii][indj_pos+2]-buffer[ii][indj_neg+2]);
						a1+=abs(buffer[ii][indj_pos+1]-buffer[ii][indj_neg+1]);
						a1+=abs(buffer[ii][indj_pos]-buffer[ii][indj_neg]);
						nb_subtractions++;

					}
					
					if (nb_subtractions)
						a1/=nb_subtractions;
						//a1*=1;
	}
	
	//a1/=counter;

}

	/*
	//coordinates: i,j --> not x,y (inverted, vertical opposed)
	int8_t sqNW[4][2]={{-1,-1},{-1,0},{0,-1},{0,0}};
	int8_t sqNE[4][2]={{-1,0},{-1,+1},{0,0},{0,+1}};
	int8_t sqSW[4][2]={{0,-1},{0,0},{+1,-1},{+1,0}};
	int8_t sqSE[4][2]={{0,0},{0,+1},{+1,0},{+1,+1}};
	
	for (char tmp=0; tmp<4; tmp++)
	{
		sqNW[tmp][1]=sqNW[tmp][1]*3;
		sqNE[tmp][1]=sqNE[tmp][1]*3;
		sqSW[tmp][1]=sqSW[tmp][1]*3;
		sqSE[tmp][1]=sqSE[tmp][1]*3;
	}
	*/

	//int8_t sqNW[4][2]={{-1,-3},{-1,0},{0,-3},{0,0}};
	//int8_t sqNE[4][2]={{-1,0},{-1,+3},{0,0},{0,+3}};
	//int8_t sqSW[4][2]={{0,-3},{0,0},{+1,-3},{+1,0}};
	//int8_t sqSE[4][2]={{0,0},{0,+3},{+1,0},{+1,+3}};

void buffer_pixelgradient_square(int i, int j)
{	
	int k, kk;

	char nb_subtractions=0;
	char indiNW, indiNE, indiSW, indiSE;
	int indjNW, indjNE, indjSW, indjSE;
	
	//coordinates: i,j --> not x,y (inverted, vertical opposed)
	int8_t sqNW[4][2]={{-1,-1},{-1,0},{0,-1},{0,0}};
	int8_t sqNE[4][2]={{-1,0},{-1,+1},{0,0},{0,+1}};
	int8_t sqSW[4][2]={{0,-1},{0,0},{+1,-1},{+1,0}};
	int8_t sqSE[4][2]={{0,0},{0,+1},{+1,0},{+1,+1}};
	
	a1=0;
	
	//char prev,curr,next;
	//prev=lnorder[0];
	//curr=lnorder[1];
	//next=lnorder[2];
	
	if ((i>0)&&(i<Hsize)&&(j>0)&&(j<Bsize))
	{
		for (k=0; k<4; k++)
		{
			indiNW=lnorder[1+sqNW[k][0]];
			indiNE=lnorder[1+sqNE[k][0]];
			indiSW=lnorder[1+sqSW[k][0]];
			indiSE=lnorder[1+sqSE[k][0]];
			
			indjNW=3*(j+sqNW[k][1]);
			indjNE=3*(j+sqNE[k][1]);
			indjSW=3*(j+sqSW[k][1]);
			indjSE=3*(j+sqSE[k][1]);
			
			for (kk=2; kk>=0; kk--)
			{
				a1+=abs(buffer[indiNW][indjNW+kk]-buffer[indiSE][indjSE+kk]);
				
				a1+=abs(buffer[indiNE][indjNE+kk]-buffer[indiSW][indjSW+kk]);
			}
		}	
		nb_subtractions=nb_subtractions+8;
	}
	
	if (nb_subtractions)
		a1/=nb_subtractions;
	
}

void buffer_pixelgradient_radial(int i, int j)
{	
	int k, kk, J, I;

	char nb_subtractions=0;
	
	I=lnorder[1];
	J=3*j;
	
	a1=0;
	
	int8_t index_i[8]={-1,-1,-1,0,+1,+1,+1,0};
	int8_t index_j[8]={-1,0,+1,+1,+1,0,-1,-1};
	
	int indi, indj;
	
	if ((i>0)&&(i<Hsize)&&(j>0)&&(j<Bsize))
	{
		for (k=0; k<4; k++)
		{
			if (1)//(k!=3)
			{
				indi=lnorder[1+index_i[2*k+1]];
				indj=3*(j+index_j[2*k+1]);
				
				for (kk=2; kk>=0; kk--)
				{
					a1+=abs(buffer[I][J+kk]-buffer[indi][indj+kk]);
				}
				nb_subtractions++;
			}
		}
		
		//nb_subtractions=nb_subtractions+8;
	}
	
	if (nb_subtractions)
		a1/=nb_subtractions;
	
}

void buffer_pixelgradient_star(int j, int ii, int iii, int i) //subtracts pixels in 8 directions, distance = 2, 2sqrt(2)
{	
	int jj;
	short indi_pos, indi_neg, indj_pos, indj_neg;
	char nb_subtractions=0;
	
	a1=0;
	
	jj=j*3;	
	
	{
		// subtracts vertically, horizontally and diagonally in both directions	
		indi_pos=(iii+0+1)%3;//(index_i[kk]>>1))%3; //previously +1 instead of +0, not the right lines...
		indi_neg=(iii+0-1)%3;//(index_i[kk]>>1))%3;
		indj_pos=(j+1)*3;
		indj_neg=(j-1)*3;
		
		
		if ((i>0)&&(i<Hsize))
		{	
			a1+=abs(buffer[indi_pos][jj+2]-buffer[indi_neg][jj+2]);
			a1+=abs(buffer[indi_pos][jj+1]-buffer[indi_neg][jj+1]);
			a1+=abs(buffer[indi_pos][jj]-buffer[indi_neg][jj]);
			nb_subtractions++;
			
			if ((j>0)&&(j<Bsize))
			{
				a1+=abs(buffer[indi_pos][indj_pos+2]-buffer[indi_neg][indj_neg+2]);
				a1+=abs(buffer[indi_pos][indj_pos+1]-buffer[indi_neg][indj_neg+1]);
				a1+=abs(buffer[indi_pos][indj_pos]-buffer[indi_neg][indj_neg]);
				nb_subtractions++;
				
				a1+=abs(buffer[indi_pos][indj_neg+2]-buffer[indi_neg][indj_pos+2]);
				a1+=abs(buffer[indi_pos][indj_neg+1]-buffer[indi_neg][indj_pos+1]);
				a1+=abs(buffer[indi_pos][indj_neg]-buffer[indi_neg][indj_pos]);
				nb_subtractions++;
				
			}
		}
		
		if ((j>0)&&(j<Bsize))
		{
			//jj+=2*3;  // index_j[1]
			a1+=abs(buffer[ii][indj_pos+2]-buffer[ii][indj_neg+2]);
			a1+=abs(buffer[ii][indj_pos+1]-buffer[ii][indj_neg+1]);
			a1+=abs(buffer[ii][indj_pos]-buffer[ii][indj_neg]);
			nb_subtractions++;

		}
		
		if (nb_subtractions)
			a1/=nb_subtractions;
	}
}

//old version, subtracts 1 pixel above and one pixel to the side
//can be tweaked to extend its subtractions span (by extending index_i and _j and increasing lele accordingly)
void buffer_pixelgradient_L(int j, int ii, int iii)
{	
	int k, kk, J, JJ;
	short indi, indj;
	short doub;
	char nb_subtractions=0;
	
	int boole;
	
	char lele=2;  //index_i and _j length

	int8_t index_i[2]={-2,0};
	int8_t index_j[2]={ 0,2};
	
	a1=0;
	
	J=j*3;	
	
	
	for (k=0; k<lele; k++)
	{
		indi=(iii+2+(index_i[k]>>0))%3;
		indj=j+(index_j[k]>>0);
		JJ=3*indj;
		boole=(indi+1)*(indj+1)*(Hsize-indi)*(Bsize-indj);
		if (boole)
		{
			for (kk=0; kk<3; kk++)
			{
				doub=buffer[ii][J+kk]-buffer[indi][JJ+kk];
				a1+=abs(doub); 
			}
			nb_subtractions++;
		}
	}
	
	if (nb_subtractions)
		a1/=nb_subtractions;

}

void initialize_buffer(void)
{
	int i, j, jj;

	BYTE red,green,blue;	
	
	uint16_t Camera_Data;
	

// preparing buffer
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
			
			if ((freepic)&&(savepic))
				cam2SD_writepxl(red, green, blue);

		}
	}
	/* // this bloc caused the picture to shift to the right by a random distance
	// transcribing buffer to bwdiff
	JJ=0;
	for (j=0; j<Bsize; j++)
	{
		rest=j%8;
		
		buffer_pixelgradient(j, 0, 2, i);
		
		if (a1>40)
		{					
			bwbyte = bwbyte|(128>>rest);
			white_cntr++;
			ILI9341_Write_Data ( RGB24TORGB16 ( 255,255,255 ));  // adding this line solves the image shifting problem
		}
		else
			//ILI9341_Write_Data ( RGB24TORGB16 ( 0,0,0 ));  // commenting this out solves the white first line problem
		
		if (rest==7)
		{
			bwdiff[0][JJ]=bwbyte;
			bwbyte=0;
			JJ++;
		}
	}
	*/
}

void draw_segments(void)
{
	//draw_sprouts();// while(1){}
	//draw_mergers();
	
	char angle_display=0, stored_display=0, sprout_display=0, nodes_display=0, area_display=0;
	char pause=1;
	int timer, time=1000000;
	uint8_t i;
	for (i=8; i<=stored; i++)
	{ LED_RED;
		if (1)//(mask[i][0])
		{
			ILI9341_DrawLine( mask[i][col_j0], mask[i][col_i0], mask[i][col_j1], mask[i][col_i1] );
			
			if (angle_display)
			{
				char angle[40];
				sprintf(angle, "%d", mask[i][col_angle]);
				ILI9341_DispString_EN(mask[i][col_j0]-20,mask[i][col_i0], angle);
			}
			
			if (stored_display)
			{
				char stored_st[5];
				sprintf(stored_st, "%d", i);			
				ILI9341_DispString_EN(mask[i][col_j1]+10,mask[i][col_i1], stored_st);
				
				if (sprout_display)
				{
					char sprt[5];
					sprintf(sprt, "%d", mask[i][col_sprout]);
					ILI9341_DispString_EN(mask[i][col_j1]+20,mask[i][col_i1]+10, sprt);
				}
			}
			
			if (nodes_display)
			{
				//ILI9341_DrawRectangle(mask[i][col_j0], mask[i][col_i0], 3, 3, 0);
				ILI9341_DrawRectangle(mask[i][col_j1], mask[i][col_i1], 3, 3, 0);
			}
			
			if (area_display)
			{
				char area_st[5];
				sprintf(area_st, "%d", mask[i][col_area1]*256+mask[i][col_area0]);
				ILI9341_DispString_EN(mask[i][col_j1]+10,mask[i][col_i1], area_st);
			}
			
			if (pause)
			{
				timer=time;
				while(timer--){}
			}
			LED_BLUE;
		}
	}
	
	LED_RGBOFF;
}

void draw_connections(void)
{
	int count=10000000/4, counter=0, lng_counter=0;
	uint8_t i;
	for (i=8; i<=stored; i++)
	{ LED_RED;
		
		//if (1)//(mask[i][0])
		if (mask[i][col_merge0]>=storedN)//it's always true normally...
		{
			ILI9341_DrawLine( mask[i][col_j1], mask[i][col_i1]-1, mask[mask[i][col_merge0]][col_j1], mask[mask[i][col_merge0]][col_i1] );
			while(count--){};
			count=10000000/4;
		}
		if (mask[i][col_merge1]>=storedN)
		{
			ILI9341_DrawLine( mask[i][col_j1], mask[i][col_i1]-1, mask[mask[i][col_merge1]][col_j1], mask[mask[i][col_merge1]][col_i1] );
			while(count--){};
			count=10000000/4;
		}
		if (mask[i][col_sprout])
		{
			ILI9341_DrawLine( mask[i][col_j0], mask[i][col_i0]+1, mask[mask[i][col_sprout]][col_j1], mask[mask[i][col_sprout]][col_i1] );
			
			while(count--){};
			count=10000000/4;
		}
	}	
	LED_RGBOFF;
}

void draw_mergers(void)
{
	int count=10000000/4, counter=0, lng_counter=0;
	uint8_t i;
	for (i=8; i<=stored; i++)
	{ LED_RED;
		
		//if (1)//(mask[i][0])
		if (mask[i][col_merge0])//>=storedN)//it's always true normally...
		{
			ILI9341_DrawLine( mask[i][col_j1], mask[i][col_i1]-1, mask[mask[i][col_merge0]][col_j1], mask[mask[i][col_merge0]][col_i1] );
			while(count--){};
			count=10000000/4;
		}
		if (mask[i][col_merge1])//>=storedN)
		{
			ILI9341_DrawLine( mask[i][col_j1], mask[i][col_i1]-1, mask[mask[i][col_merge1]][col_j1], mask[mask[i][col_merge1]][col_i1] );
			while(count--){};
			count=10000000/4;
		}
	}	
	LED_RGBOFF;
}

void draw_sprouts(void)
{
	int count=10000000/4, counter=0, lng_counter=0;
	uint8_t i;
	for (i=8; i<=stored; i++)
	{ LED_RED;
		if (mask[i][col_sprout])
		{
			ILI9341_DrawLine( mask[i][col_j0], mask[i][col_i0]+1, mask[mask[i][col_sprout]][col_j1], mask[mask[i][col_sprout]][col_i1] );
			
			while(count--){};
			count=10000000/4;
		}
	}	
	LED_RGBOFF;
}

uint8_t noise_eraser(int i, int j, char rest, uint8_t bwbyte, char halt, char display)
{
	if (halt)
	{
		LED_YELLOW;
		while(1)
		{
			if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
				break;
		}
	}
	
	char er_rest=(j-1)%8;
	char bwerase= 255 -(128>>(er_rest));
	if (rest)
	{ bwbyte&=bwerase;}
	else
	{ bwdiff[i][(j-1-er_rest)/8]&=bwerase; }
	if (display)
		bwdisplay();
	return bwbyte;
}

