#include "./BD_debug.h"

char savepic, showpic;
char glb_dbgpnl=200;
char loc_ersflg, loc_pnlhlf, glb_pnlhlf, loc_pnlend; 

void track_activity(char boolean, char cases, uint8_t CP, uint8_t LP)
{
	uint8_t posQ;
	if (boolean)
	{
		if ((1)&&(cases==1)) //NORMAL GROWTH
		{
			//don't account for more than 2 consecutive growth events
			posQ=mask[glb_dbgpnl][CP];
			if((mask[glb_dbgpnl+posQ][CP]!=1)||(mask[glb_dbgpnl+posQ-1][CP]!=1)||(posQ==1))
			{
				posQ++;
				mask[glb_dbgpnl+posQ][CP]=1;
				mask[glb_dbgpnl][CP]=posQ;
			}
		}
		else if ((1)&&(cases==2)) //CV0
		{
			posQ=mask[glb_dbgpnl][CP];
			posQ++;
			mask[glb_dbgpnl+posQ][CP]=64+1;
			mask[glb_dbgpnl][CP]=posQ;
			
			posQ=mask[glb_dbgpnl][LP];
			posQ++;
			mask[glb_dbgpnl+posQ][LP]=96+1; //96=64+32
			mask[glb_dbgpnl][LP]=posQ;
		}
		else if ((1)&&(cases==3)) //CV2
		{
			posQ=mask[glb_dbgpnl][CP];
			posQ++;
			mask[glb_dbgpnl+posQ][CP]=64+2;
			mask[glb_dbgpnl][CP]=posQ;
			
			posQ=mask[glb_dbgpnl][LP];
			posQ++;
			mask[glb_dbgpnl+posQ][LP]=96+2;
			mask[glb_dbgpnl][LP]=posQ;
		}
		else if ((1)&&(cases==4)) //CVX
		{
			posQ=mask[glb_dbgpnl][CP];
			posQ++;
			mask[glb_dbgpnl+posQ][CP]=64+4;
			mask[glb_dbgpnl][CP]=posQ;
			
			posQ=mask[glb_dbgpnl][LP];
			posQ++;
			mask[glb_dbgpnl+posQ][LP]=96+4;
			mask[glb_dbgpnl][LP]=posQ;
		}
		else if ((1)&&(cases==5)) //CVZ
		{
			posQ=mask[glb_dbgpnl][CP];
			posQ++;
			mask[glb_dbgpnl+posQ][CP]=64+8;
			mask[glb_dbgpnl][CP]=posQ;
		}
		else if ((1)&&(cases==6)) //DV0
		{
			posQ=mask[glb_dbgpnl][CP];
			posQ++;
			mask[glb_dbgpnl+posQ][CP]=80+1; //80=64+16
			mask[glb_dbgpnl][CP]=posQ;
		}
		else if ((1)&&(cases==7)) //DV1
		{
			posQ=mask[glb_dbgpnl][CP];
			posQ++;
			mask[glb_dbgpnl+posQ][CP]=80+2;
			mask[glb_dbgpnl][CP]=posQ;
		}
		else if ((1)&&(cases==8)) //DVZ
		{
				posQ=mask[glb_dbgpnl][CP];
				posQ++;
				mask[glb_dbgpnl+posQ][CP]=80+4;
				mask[glb_dbgpnl][CP]=posQ;
		}
		else if ((1)&&(cases==0))
		{
			uint8_t debugz, p;
					
			posQ=mask[glb_dbgpnl][CP];
			debugz=mask[glb_pnlhlf+1][CP];
			
			//if (posQ>80) //Hsize=240, not 255
			//if (posQ>130) 
			if (posQ>loc_pnlend)
			{
				posQ=0;
				for (p=glb_dbgpnl;p<glb_pnlhlf;p++)
				{
					mask[p][CP]=0;
				}
			}
			else if ((posQ<loc_pnlhlf)&&(posQ>loc_ersflg)&&(debugz>0))
			{
				for (p=glb_pnlhlf;p<Hsize;p++)
				{
					mask[p][CP]=0;
				}
			}
			
			if ((mask[glb_dbgpnl+posQ][CP]==118)&&(mask[glb_dbgpnl+posQ-3][CP]==118)&&(mask[glb_dbgpnl+posQ-2][CP]==mask[glb_dbgpnl+posQ-5][CP]))
			{
				posQ--; posQ--; posQ--;
			}
			//posQ++;
			posQ++;
			if (mask[succumb][CP]==24)
				{
					mask[glb_dbgpnl+posQ][CP]=99; //99=64+32+2+1
				}
			else if (mask[alive][CP]==0)
				{
					mask[glb_dbgpnl+posQ][CP]=28; //28=16+8+4
				}
			else
				{mask[glb_dbgpnl+posQ][CP]=8;} //119=127-8  //this should not appear
			posQ++;
			mask[glb_dbgpnl+posQ][CP]=127;
			
			mask[glb_dbgpnl][CP]=posQ;
		
		}
  }
}

void trackparent(char curr_partner)
{
	char pos;
	
	pos=mask[glb_dbgpnl][empty_slot];
	pos++;
	mask[glb_dbgpnl+pos][empty_slot]=0;
	pos++;
	mask[glb_dbgpnl+pos][empty_slot]=curr_partner;
	pos++;
	mask[glb_dbgpnl+pos][empty_slot]=0;
	mask[glb_dbgpnl][empty_slot]=pos;
}

//TIP0 TIP15 bug
void staginversion_bug(uint8_t CP, uint8_t LP, uint8_t posCP, uint8_t posLP, int i, int strip_start, int j)
{
	int count=10000000;
	
	char q, debug1, debug2, debug3=0;
	char start=8, end=39;
	
	mask[50][CP]=strip_start;
	mask[51][CP]=j;
	
	{ //basic report
		ILI9341_DrawRectangle(mask[posCP][CP],200,3,3,1);
		ILI9341_DrawRectangle(mask[posLP][LP],205,3,3,0);
		
		ILI9341_DrawLine(mask[posCP][CP],198,mask[posCP][CP],i+2);
		ILI9341_DrawLine(mask[posLP][LP],203,mask[posLP][LP],i+1);//mask[root][LP]+1);//
		
		//ILI9341_DrawLine(255,240,255,i+2);
		
		ILI9341_DrawRectangle((mask[posLP][LP]+mask[posLP+1][LP])>>1,mask[root][LP]-2,1,1,1);
		
		char current[40], lastone[40], cptip[40];//, currstart[40];
		char c = LP;
		char cc = CP;
		char ccc = mask[tip][CP];
		//char cccc = mask[posCP][CP];
		
		//ILI9341_DrawLine(255+cccc,240,255+cccc,i+2);
		
		sprintf(current,"current: %d ",cc);
		sprintf(lastone,"lastone: %d ",c);
		sprintf(cptip,"tip: %d ",ccc);
		//sprintf(currstart,"currstart: %d ",cccc);
		
		ILI9341_DispString_EN(100,200,current); //ILI9341_DispString_EN(200,200,currstart);
		ILI9341_DispString_EN(100,220,lastone);
		ILI9341_DispString_EN(100,180,cptip);
		
		//draw_connections();
	}

	
	
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
			count=10000000/2;
		}
	}
	int name_count = 0;
	char name0[40], name1[40], name2[40];
	sprintf(name0,"0:stagI_%d_bwimage.bmp",name_count);			
	bmpres = f_open( &bmpfsrc , name0, FA_OPEN_EXISTING | FA_READ);	
	
	while(bmpres == FR_OK)
	{
		f_close(&bmpfsrc); 
		
		name_count++; 
		sprintf(name0,"0:stagI_%d_bwimage.bmp",name_count);	
		
		bmpres = f_open( &bmpfsrc , name0, FA_OPEN_EXISTING | FA_READ);	
	}
	
	sprintf(name0,"0:stagI_%d_bwimage.bmp",name_count);		
	sprintf(name1,"0:stagI_%d_dbugpnl.bmp",name_count);		
	sprintf(name2,"0:stagI_%d_scrshot.bmp",name_count);	

	matrix2picture( 1, 0, name0);
	matrix2picture( 1, 1, name1);
	Screen_Shot( 0, 0, Bsize, Hsize, name2 );
	
	ILI9341_DrawRectangle(315,235,5,5,1);
	
	//count=100000000;
	//while(count--){};
	LED_RED;
	char updown=0;
	while (1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			if (updown==0)
			{
				bwdisplay1();
				updown++;
			}
			else if (updown==1)
			{
				bwdisplay();
				ILI9341_DrawLine(mask[posCP][CP],198,mask[posCP][CP],i+2);
				ILI9341_DrawLine(mask[posLP][LP],203,mask[posLP][LP],i+1);
				updown--;
			}
			
			//while(count--){}
			//count=1000;
		}
	}
}

void phantomcase_bug(int j, int strip_start, char curr_partner, char last_partner)
{
	mask[146][curr_partner]=126;
	mask[147][curr_partner]=255;
	mask[50][curr_partner]=strip_start;
	mask[51][curr_partner]=j;
	
	
	int name_count = 0;
	char name0[40], name1[40], name2[40];
	sprintf(name0,"0:phtmC_%d_bwimage.bmp",name_count);			
	bmpres = f_open( &bmpfsrc , name0, FA_OPEN_EXISTING | FA_READ);	
	
	while(bmpres == FR_OK)
	{
		f_close(&bmpfsrc); 
		
		name_count++; 
		sprintf(name0,"0:phtmC_%d_bwimage.bmp",name_count);	
		
		bmpres = f_open( &bmpfsrc , name0, FA_OPEN_EXISTING | FA_READ);	
	}
	
	sprintf(name0,"0:phtmC_%d_bwimage.bmp",name_count);		
	sprintf(name1,"0:phtmC_%d_dbugpnl.bmp",name_count);		
	sprintf(name2,"0:phtmC_%d_scrshot.bmp",name_count);	

	matrix2picture( 1, 0, name0);
	matrix2picture( 1, 1, name1);
	Screen_Shot( 0, 0, Bsize, Hsize, name2 );
	
	ILI9341_DrawRectangle(315,235,5,5,1);
	
	if (1)
	{
		bwdisplay1();
		while (1){LED_BLUE;LED_RGBOFF;}
	}
}

void error_message( int i, int j, char * err_message, char hasvalue, char * value_name, int value)
{
	ILI9341_DispString_EN(j,i,err_message);
	
	if (hasvalue)
	{
		char value_line[40];
		
		sprintf(value_line,"%s: %d ",value_name, value);
		
		ILI9341_DispString_EN(j,i+16,value_line);
	}

}

void display_value(int value, int i, int j)
{
	char value_st[40];
	sprintf(value_st, "value: %d ", value);
	ILI9341_DispString_EN(j,i,value_st);
}

void inspection_mode(void)
{
	char updown=0;
	ILI9341_DrawRectangle(315,0,5,5,1);
	while (1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			if (updown==0)
			{
				bwdisplay1();
				updown++;
			}
			else if (updown==1)
			{
				bwdisplay();
				updown--;
			}

		}
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			draw_segments();
		}
	}
} 

void run_dblxprt(char * parameter, double value1, double value2)
{
	char BDsegments[40], debugpanel[40];
	char tmp1_seg[40]="tmp1_s.bmp", tmp1_dbp[40]="tmp1_p.bmp", tmp2_seg[40]="tmp2_s.bmp", tmp2_dbp[40]="tmp2_p.bmp";
	int NC=0;
	char div[5];
	strcpy(div, "!");
	
	sprintf(BDsegments,"0:auto_SF_!_segmt.bmp");
	sprintf(debugpanel,"0:auto_SF_!_dbpnl.bmp");

	//ILI9341_DispString_EN(50,50,BDsegments);
	//ILI9341_DispString_EN(50,70,debugpanel);

	*parameter=value1;
	border_det3();
	draw_segments();
	
	Screen_Shot(0,0,Bsize,Hsize,tmp1_seg);
	matrix2picture( 1, 1, tmp1_dbp);
	
	*parameter=value2;
	border_det3();
	draw_segments();
	
	Screen_Shot(0,0,Bsize,Hsize,tmp2_seg);
	matrix2picture( 1, 1, tmp2_dbp);
	
	show_bmpdiff(tmp1_seg,tmp2_seg);
	
	while(1)
	{
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			tiny_floppy(300,120);			
			while(1)
			{
				if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
				{
					char message[40], savename[40];
						
					NC=newbmp_index(div, 2, BDsegments, debugpanel);
					
					f_rename(tmp1_seg, BDsegments);
					f_rename(tmp1_dbp, debugpanel);
					strcpy(tmp1_seg,BDsegments);
					
					sprintf(message, "first image stored");
					ILI9341_DispString_EN(100,100,message);
					ILI9341_DispString_EN(50,120,BDsegments);
					ILI9341_DispString_EN(50,140,debugpanel);
					
					sprintf(BDsegments,"0:auto_SF_%d_segmt.bmp", ++NC);
					sprintf(debugpanel,"0:auto_SF_%d_dbpnl.bmp", NC);
					
					f_rename(tmp2_seg, BDsegments);
					f_rename(tmp2_dbp, debugpanel);
					strcpy(tmp2_seg,BDsegments);
					
					sprintf(message, "second image stored");
					ILI9341_DispString_EN(90,100,message);
					ILI9341_DispString_EN(50,120,BDsegments);
					ILI9341_DispString_EN(50,140,debugpanel);
					
					show_bmpdiff(tmp1_seg,tmp2_seg);				
					sprintf(savename,"0:auto_SF_%d-%d_segmt_debug_rsltdiff.bmp", NC-1, NC);
					Screen_Shot(0,0,Bsize,Hsize,savename);
					
					sprintf(message, "output difference saved");
					ILI9341_DispString_EN(100,100,message);
					ILI9341_DispString_EN(20,120,savename);
					
					while(1)
					{
						if ((Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)||(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON))
							return;
					}
					
				}
				if (Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
				{
					return;
				}
			}
		}
	}	
}

void recurse(int depth, int * counter_pt)
{
	if (depth==0)
		return;
	
	for (int i=0; i<10; i++)
	{
		if (depth==1)
			(*counter_pt)++;
		recurse(depth-1, counter_pt);
	}
}

void multidimensional_test(int ndim, int maxdim, char * textd, int * ariadnes_thread, char * filename)
{
	/*
	int i;
	
	int min, a;
	
	va_list ap;
	
	va_start(ap, arg_count);
	
	min = va_arg(ap, int);
	
	for (i = 2; i <= arg_count; i++)  //2, because first argument jas already been called
		if ((a = va_arg(ap, int)) < min)
			min = a;
	*/
	unsigned int write_txt;
	char txtpres;
	char mystr[40], label[40];
	double val;
	char k, kk;
	
	for (k=0; k<10; k++)
	{
		if (ndim==0)
		{
			return;
		}
		/*if (ndim==maxdim)
		{
			for (kk=0; kk<maxdim; kk++)
			{textd[kk]=0;}
		}*/
		textd[maxdim-ndim]=k;
		if (ndim==1)
		{
			txtpres = f_open( &bmpfsrc1 , (char*)filename, FA_CREATE_ALWAYS | FA_WRITE );
			f_lseek (& bmpfsrc1, *ariadnes_thread);
			
			if (k==0)
			{
				sprintf(label,"maxtol=%d, maxlng varying \r\n",textd[0]); 
				txtpres = f_write(&bmpfsrc1, label ,sizeof(unsigned char)*(strlen(label)), &write_txt);
				(*ariadnes_thread)+=(strlen(label));
			}
			
			//store value
			//val=textd[0]*textd[0]*sin(textd[1]);
			maxtol=textd[0];
			maxlng=textd[1]*5;
			border_det3();
			draw_segments();
			//ILI9341_DispString_EN(100,100, "done");
			val = ((double)absrb_bodies)/((double)total_bodies);//
			//val = stored-7;
			sprintf(mystr, "%g \r\n", val);
			txtpres = f_write(&bmpfsrc1, mystr ,sizeof(unsigned char)*(strlen(mystr)), &write_txt);	
			*ariadnes_thread+=(strlen(mystr));

			
			if ((1)&&(k==9))
			{
				txtpres = f_write(&bmpfsrc1, "\r\n" ,sizeof(unsigned char)*(strlen("\r\n")), &write_txt);
				(*ariadnes_thread)+=(strlen("\r\n"));
			}
			f_close(&bmpfsrc1);	
		}
		//f_close(&bmpfsrc1);
		multidimensional_test(ndim-1, maxdim, textd, ariadnes_thread, filename);
	}
		
	//va_end(ap);
	//f_close(&bmpfsrc1); 	
}

void compare_output(int index1, int index2)
{
	char name1[30], name2[30];
	sprintf(name1,"0:SF_%d_segmt.bmp", index1);
	sprintf(name2,"0:SF_%d_segmt.bmp", index2);
	
	show_bmpdiff(name1, name2);
	
	while(1)
	{
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			char message[40], savename[40];
			sprintf(savename,"0:debug_rsltdiff_1_SF_%d-%d_segmt.bmp", index1, index2);
			
			Screen_Shot(0,0,Bsize,Hsize,savename);
			sprintf(message, "output difference saved");
			ILI9341_DispString_EN(100,100,message);
			ILI9341_DispString_EN(20,120,savename);
			while(1)
			{
				if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
					return;
			}
		}
	}
}

void maindebug(char * singleframe, char * frameBD, char * BDsegments, char * debugpanel)
{
	char updown=0, downup=1;				
	char justup=0, switches, switch2save;
	char saving_window;
	if (savepic&&showpic)
	{
		switches=12;
		switch2save=9;
	}
	else
	{
		switches=8;
		switch2save=6;
	}
	while (1)
	{
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{			
			bwdisplay1();
			while (1)
			{
				if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
				{
					if (justup==switches)
						justup=0;
					saving_window = (justup>=switch2save);
														
					downup=1;
					
					if (updown==0)
					{
						bwdisplay();
						updown++;
						if (saving_window)
							tiny_floppy(300,120);
					}
					else if (updown==1)
					{
						if (saving_window){bwdisplay();tiny_floppy(300,120);}
						draw_segments();
						if (savepic&&showpic)
							updown++;
						else
							updown--;
					}
					else if (updown==2)
					{
						show_mybmp (savedframe, 1);
						updown=0;
						if (saving_window)
							tiny_floppy(300,120);
					}		
					justup++;
				}
				else if((saving_window)&&(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  ))
				{
					if (updown==1)
					{
						matrix2picture(1,0,frameBD);
						
						char message[40];
						sprintf(message, "gradient image saved");
						ILI9341_DispString_EN(100,100,message);
						ILI9341_DispString_EN(100,120,frameBD);
					}
					else if ((updown==2)||((savepic==0)&&(updown==0))) //else if (updown==2*savepic)
					{
						Screen_Shot(0,0,Bsize,Hsize,BDsegments);
						
						char message[40];
						sprintf(message, "segmented borders saved");
						ILI9341_DispString_EN(100,100,message);
						ILI9341_DispString_EN(100,120,BDsegments);
					}
					else if ((savepic)&&(updown==0))
					{
						f_rename (savedframe, singleframe); 
						
						char message[40];
						sprintf(message, "frame stored");
						ILI9341_DispString_EN(100,100,message);
						ILI9341_DispString_EN(100,120,singleframe);
					}	
				}
				else if((saving_window==0)&&(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  ))
				{
					if (downup==1)
					{
						bwdisplay1();
						tiny_floppy(300,120);
						downup--;
					}
					else if (downup==0)
					{
						char message[40];
						matrix2picture( 1, 1, debugpanel);
						sprintf(message, "debug panel saved");
						ILI9341_DispString_EN(100,100,message);
						ILI9341_DispString_EN(100,120,debugpanel);
						
						downup++;
					}
				
				}
			}
		}
		
	}
}

void floppy_disc(int x, int y)
{
	uint8_t side=20;
		ILI9341_DrawLine( x, y, x+0.8*side, y ) ;
	ILI9341_DrawLine(x+0.8*side, y, x+side, y+0.2*side) ;
	ILI9341_DrawLine(x+side, y+0.2*side, x+side, y+side) ;
	ILI9341_DrawLine( x+side, y+side, x, y+side ) ;
	ILI9341_DrawLine( x, y+side, x, y ) ;
	
	ILI9341_DrawRectangle( x+0.25*side, y, 0.5*side, 0.3*side, 0 );
	ILI9341_DrawRectangle( x+0.5*side, y+0*side, 0.15*side, 0.2*side, 0 );
	ILI9341_DrawRectangle( x+0.2*side, y+0.5*side, 0.6*side, 0.55*side, 0 );
	
	ILI9341_DrawLine( x+0.25*side, y+0.6*side, x+0.75*side, y+0.6*side ) ;
	ILI9341_DrawLine( x+0.25*side, y+0.7*side, x+0.75*side, y+0.7*side ) ;
	ILI9341_DrawLine( x+0.25*side, y+0.8*side, x+0.75*side, y+0.8*side ) ;
	ILI9341_DrawLine( x+0.25*side, y+0.9*side, x+0.75*side, y+0.9*side ) ;
}

void tiny_floppy(int x, int y)
{
	char one[10];
	sprintf(one, "1:");
	ILI9341_DispString_EN(x-18, y, one);
	
	uint8_t side=15;
		ILI9341_DrawLine( x, y, x+0.8*side, y ) ;
	ILI9341_DrawLine(x+0.8*side, y, x+side, y+0.2*side) ;
	ILI9341_DrawLine(x+side, y+0.2*side, x+side, y+side) ;
	ILI9341_DrawLine( x+side, y+side, x, y+side ) ;
	ILI9341_DrawLine( x, y+side, x, y ) ;
	
	ILI9341_DrawRectangle( x+0.3*side, y, 0.5*side, 0.3*side, 0 );
	//ILI9341_DrawRectangle( x+0.5*side, y+0*side, 0.15*side, 0.2*side, 0 );
	ILI9341_DrawRectangle( x+0.2*side, y+0.5*side, 0.6*side, 0.6*side, 0 );
	
	ILI9341_DrawLine( x+0.25*side, y+0.65*side, x+0.75*side, y+0.6*side ) ;
	ILI9341_DrawLine( x+0.25*side, y+0.75*side, x+0.75*side, y+0.8*side ) ;
	ILI9341_DrawLine( x+0.25*side, y+0.9*side, x+0.75*side, y+0.9*side ) ;
}
