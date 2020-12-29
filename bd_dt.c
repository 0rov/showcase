
#include "bd_dt.h"


char tmp_storage_end, secnd_slotline, third_slotline, max_slotlng;
uint8_t maxlng=20, maxtol=5;
int total_bodies, absrb_bodies;
uint16_t area;

//library

	char col_i0=0, col_j0=1, col_i1=2, col_j1=3, col_sprout=4, col_merge0=5, col_merge1=6, col_angle=7, col_area1=8, col_area0=9, col_debug=10;
	char /*nod_parents=1, nod_children=1,*/ nod_connect=1;
	
	char stud=0;
	char stag=1;

	char detect0=2;
	char detect1=3;
	char alive=4;
	char succumb=5;
	char byte4=6;
	char byte5=7;
	char byte6=8;
	char sprout=9;
	char stored_alrd=10;
	char merge0=11;
	char merge1=12;
	char root=13;
	char tip=14;
	uint8_t area1=15, area0=16;
	char base=18;
	
	uint8_t stored;
	uint8_t storedN;

	char track=1;

char freepic, bwpic;//, savepic;
int8_t sticky_crnr=0;

char erase_noise=0;
char mask_image=1;
//double mask_att[4]={3.73, -440, 3.73, -605};
double mask_att[4]={0, 240, 0, 0};

char lnorder[3]={0,1,2};
char savedframe[40]="saved_frame1.bmp";
char tmp;

short a1;

const char start=11, end=39;
char empty_slot, new_slot; // only 2 calls of new_slot, may be discarded



#define RGB24TORGB16(R,G,B) ((unsigned short int)((((R)>>3)<<11) | (((G)>>2)<<5)	| ((B)>>3)))
	
void border_det3(void)
{
	int i, j, ii=0, iii, jj, JJ;
	uint8_t q, p, tmp;

	BYTE red,green,blue;		
	uint16_t Camera_Data;	
	uint8_t bwbyte=0, rest;

	uint8_t partners, curr_partner, last_partner, overlap0;	
	uint8_t lng, strip_rank, strip_start;  // note: strip_start cannot reach beyond 255
	uint8_t pointer, posQ, posCP, posLP;
	//uint8_t stored, storedN;
	
	total_bodies=0; absrb_bodies=0;
	
	empty_slot=start;
	new_slot=start;
	
	stored=7;  // will be incremented to 8 before first value is stored
	storedN=240; // will be decremented to 249 before first value is stored
	
	char narrow;
	//short counter;
	char whiteon;           // strip is being detected
	short black=0;          // soon to be used to measure distance between consecutive strips
	char abort=0;//, track=1;//, savepic=0;//, freepic=1;
	
	//debug
	/*
	uint8_t debug_counter=0;
	uint8_t debug_counter0=0;
	uint8_t debug_counter2=0;
	*/

	maskvoid();

	//storage slots
	for (i=0;i<7;i++)
	{
		for (j=0;j<col_debug;j++)
			mask[i][j]=126;
	}
	
	///PROGRAM STARTS
	
	ILI9341_OpenWindow(0, 0, 320, 240);
	ILI9341_Write_Cmd (CMD_SetPixel ); 
	
	white_cntr=0;
	
	
	
	if (freepic)
	{
		//while(1){LED_CYAN;} ///lastdebug
		if (savepic)
		{cam2SD_prepare(savedframe);}
		
		initialize_buffer();
		//vertical down shift
		for (j=0; j<320; j++)
			ILI9341_Write_Data ( RGB24TORGB16 ( 0, 0, 0 ) );
	}
	else
	{
		//open image file
		
		//char path[40]="0:";
		//scan_bmps(path);
		
		//char filename[20]="gray_19.bmp";
		//char filename[20]="gray_1.bmp";
		//char filename[20]="SF_20_brdrs.bmp";
		//char filename[20]="test_area.bmp";
		
		char filename[20]="saved_frame.bmp";
		//char filename[20]="phtmC_0_scrshot.bmp";
		//char filename[20]="phtmC_1_scrshot.bmp";
		//char filename[20]="phtmC_7_scrshot.bmp";
		//char filename[20]="phtmC_10_scrshot.bmp";
		//char filename[20]="phtmC_11_scrshot.bmp";
		//char filename[20]="phtmC_12_scrshot.bmp";
		//char filename[20]="SF_0_frame.bmp";
		//char filename[20]="SF_1_frame.bmp";
		//char filename[20]="SF_2_frame.bmp";
		
		//char filename[20]=".bmp";
		
		load_image(filename);
		
		load_pxlns2bff();
		
		if (savepic)
		{
			//while(1){LED_CYAN;} ///lastdebug
			//add a vertical (down) shift
			for (i=0; i<4; i++)
				for (j=0; j<320; j++)
					ILI9341_Write_Data ( RGB24TORGB16 ( 0, 0, 0 ) );
		}
	}
	
	for (i=1; i<240-2+3; i++)
	{	
		
		ii++;
		iii=ii;
		if (ii==3)
			ii=0;
		
		JJ=0; whiteon=0; lng=0;
		strip_rank = start-1; overlap0 = start; pointer = start /*debug?*/;
		
		//debug
		char last_one=start-1, last_pos=base;
		
		
		//
		//  BEGIN READING THROUGH A LINE OF PIXELS
		//
		
		for(j=0; j<320; j++) 										
		{
			rest = j%8;
			
			if ((bwpic)&&(freepic==0))
			{
				a1=0;
				if (((0)||(mask_image==0)||((i<(mask_att[0]*j+mask_att[1]))&&(i>(mask_att[2]*j+mask_att[3]))))||(((i<20)||(i>50))&&((i<90)||(i>120))&&((i<180)||(j>200))&&((i<120)||(j>140))&&((i>140)||(j<220))))
				//if ((0)||(mask_image==0)||((j==(mask_att[0]*i+mask_att[1]))&&(j==(mask_att[2]*i+mask_att[3]))))
					a1=buffer[ii][j*3];
			}
			else
			{
				//type of gradient
				
				buffer_pixelgradient(j,ii,iii,i);
				//buffer_pixelgradient_radial(i, j);
				//buffer_pixelgradient_square(i, j);
				//buffer_pixelgradient_L(j,ii,iii);
			}
			narrow = 20;
			
			if ((a1>20)&&(j<255-narrow)&&(j>65+narrow)&&(i<239))  //j cannot go beyond 255, can only store single bytes...
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
					whiteon=0; last_partner=7; partners=0;
					
					if ((erase_noise)&&(lng==1))
					{ bwbyte=noise_eraser( i,  j,  rest,  bwbyte,  0,  1);}
					
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
								if(mask[posCP+0][curr_partner]>j)
								{
									char error[40];
									sprintf(error, "jumped forward, CP: %d, OL: %d", curr_partner, overlap0);
									ILI9341_DispString_EN(100, i+20, error);
									LED_RED;
									while(1){}
								}
									
								if (mask[stag][pointer]==0) 
								{
									int dead_stag=0;
									for (tmp=pointer+1; tmp<=end; tmp++)
										if (mask[stag][tmp])
											dead_stag++;//=mask[stag][tmp];
									if (dead_stag) 
									{
										char error[40];
										sprintf(error, "swiss cheese, hole: %d, dead_stag: %d", pointer, dead_stag);
										ILI9341_DispString_EN(10, i+20, error);
										LED_RED;
										inspection_mode();
										//while(1){}
									}
									else
									{break;}	
								}
								pointer++;
								curr_partner = mask[stag][pointer];
								posCP = mask[tip][curr_partner];
							}
							
							overlap0 = pointer; // next strip will start looking for partners in the stag starting from this value
							
							area = mask[area1][curr_partner]*256+mask[area0][curr_partner];
							
              ///MAIN LOOP
							
							//while((mask[stag][pointer]>0)&&(mask[posCP+0][curr_partner]<=j))
							while((mask[stag][pointer]>0)&&(mask[posCP+0][curr_partner]<j+sticky_crnr)&&(mask[posCP+1][curr_partner]>strip_start-sticky_crnr))
							{
								if(mask[posCP+1][curr_partner]<strip_start)
								{
									char error[40];
									sprintf(error, "jumped backwards, CP: %d ", curr_partner);
									ILI9341_DispString_EN(100, i+20, error);
									LED_RED;
									while(1){}
								}
								
								//abort in case storage is full
								if (storedN-stored<10)
								{
									abort=1;
									break;
								}

								posLP=mask[tip][last_partner];							
								partners++;
								
								///NORMAL GROWTH
								
								if ((mask[alive][curr_partner]==0)&&(mask[succumb][curr_partner]==0)&&(partners==1))
								{
									strip_append(j, strip_start, curr_partner, posCP, last_partner, strip_rank, pointer);
								}
								else
								{
									///CONVERGENCE
									
									if (mask[alive][curr_partner]==0)
									{
										mask[alive][curr_partner]=24;
		
										if (partners==2)
										{
											if (mask[succumb][last_partner]==0) ///CV#0
											{ CV0(i, strip_start, j, strip_rank, curr_partner, last_partner, posLP); }
											
											else
											{				
												if (mask[merge0][last_partner]) ///CV#2		
												{ CV2(strip_start, j,curr_partner, last_partner); }		
												
												else ///CV#X	
												{ CVX(i, strip_start, j, curr_partner, last_partner); }
											}
										}
										
										else ///CV#Z
										{ CVZ(curr_partner, last_partner); }
									}
									
									///DIVERGENCE
									
									else if (partners==1)
									{
										if ((mask[storedN][col_sprout]!=mask[stored_alrd][curr_partner])||(mask[stored_alrd][curr_partner]==0)) // if CP not stored, stored_alrd=0...
										{
											if (mask[merge0][curr_partner]==0) ///DV#0
											{
												short origin=mask[sprout][curr_partner];
												
												if ((1)&&(origin>=storedN)&&(mask[origin][col_sprout]==0)&&(posCP-base<5))
												{
													if(mask[posCP+3][curr_partner]==0)
													{
														error_message(50,10,"no false tip",1,"strip_start",strip_start); 
														ILI9341_DrawLine(mask[posCP+2][curr_partner]+1, i+10, mask[posCP+3][curr_partner]+1, i+30);
														inspection_mode();
													}
													
													uint8_t hole = (mask[posCP+3][curr_partner]+strip_start)>>1;
													int8_t number = mask[origin][nod_connect];
													uint8_t siblings[10];//number];
													uint8_t sibling=0;
													for (q=8; (q<=stored)&&(sibling<number); q++)
													{
														if((mask[q][col_merge0]==origin)||(mask[q][col_merge1]==origin))
														{
															siblings[sibling]=q;
															sibling++;
														}
													}
													
													if(number<=1){error_message(80,50,"nod_connect less than 1",1,"number",number); LED_RED; inspection_mode();}
													if(sibling<=1){error_message(20,50,"less than 1 sibling",1,"sibling",sibling); LED_RED; inspection_mode();}
													if(sibling<number-1){error_message(50,50, "sibling number mismatch",1,"number",number); LED_RED; while(1){}}
													
													uint8_t leftmost=siblings[0], rightmost=siblings[sibling-1];//[number];	
													int16_t vct0[2]={mask[leftmost][col_j1]-mask[leftmost][col_j0],mask[leftmost][col_i1]-mask[leftmost][col_i0]}, 
																  vct1[2]={mask[rightmost][col_j1]-mask[rightmost][col_j0],mask[rightmost][col_i1]-mask[rightmost][col_i0]},
																  vct0h[2]={hole-mask[leftmost][col_j0],i-mask[leftmost][col_i0]}, 
																  vct1h[2]={hole-mask[rightmost][col_j0],i-mask[rightmost][col_i0]};
													int16_t tol0=vct0[0]*vct0h[0]+vct0[1]*vct0[1], tol1=vct1[0]*vct1h[0]+vct1[1]*vct1[1];
													// check alignment
													if ((abs(vct0[0]*vct0h[1]-vct0[1]*vct0h[0])<15*tol0)||(abs(vct1[0]*vct1h[1]-vct1[1]*vct1h[0])<15*tol1))//||(posCP-base<=4))
													{
														if (number==2)
														{
															for (q=0; q<start; q++){mask[origin][q]=0;}
															if((mask[leftmost][col_merge1]>stored)&&(mask[leftmost][col_merge1]==mask[rightmost][col_merge0]))
															{
																mask[leftmost][col_merge1]=0;
																mask[rightmost][col_merge0]=0;
															}
															else
															{
																error_message(100,20,"mergers not found",1,"leftmerger", mask[leftmost][col_merge1]);
																display_value(mask[rightmost][col_merge0],130,20);
																inspection_mode();
															}
														}
														else
														{
															if((mask[leftmost][col_merge1]>stored)&&(mask[leftmost][col_merge1]==mask[rightmost][col_merge0]))
															{/*ok*/}
															else
															{
																error_message(100,20,"mergers not found",1,"leftmerger", mask[leftmost][col_merge1]);
																display_value(mask[rightmost][col_merge0],130,20);
																inspection_mode();
															}
														}
														
														//mask[leftmost][col_i1]=i-1;
														//mask[leftmost][col_j1]=(mask[posCP][curr_partner]+hole)/2;
														prepare_slot(empty_slot,leftmost,0,0,i,mask[posCP+2][curr_partner],mask[posCP+3][curr_partner]);
														mask[stud][strip_rank-1]=empty_slot;
														
														refresh_emptyslot();
														
														//mask[rightmost][col_i1]=i-1;
														//mask[rightmost][col_j1]=(hole+mask[posCP+1][curr_partner])/2;
														
														//prepare_slot(empty_slot,rightmost,0,0,i,strip_start,j);
														//mask[stud][strip_rank]=empty_slot;
														mask[posCP][curr_partner]=hole;
														//mask[posCP+1][curr_partner] stays the same
														mask[sprout][curr_partner]=rightmost;
														
														//mask[posCP+2][curr_partner]=strip_start;
														//mask[posCP+3][curr_partner]=j;
														
														mask[stud][strip_rank]=curr_partner;
														
														char true_sprout;
														stored++;
														char is_stored = store_body(stored, curr_partner);
														
														
														if (is_stored==1)
															true_sprout=stored;
														else if (is_stored==0)
															true_sprout=mask[sprout][curr_partner];
														else
														{error_message(50,50,"store_body didn't work",0,"",0); LED_RED; while(1){}}
														
														//if(rightmost>20){display_value(rightmost,30,50); display_value(true_sprout,50,50); display_value(mask[true_sprout][col_sprout],70,50); while(1){}}														
														//if(true_sprout==0){error_message(50,50,"true_sprout is null",0,"",0); LED_RED; while(1){}}
															
														prepare_slot(empty_slot,true_sprout,0,0,i,strip_start,j);
														
														mask[succumb][curr_partner]=24;
														mask[stored_alrd][curr_partner]=stored;
														
														//erase node
														//current parent's true tip split into 2 and correct previous parents' stored tips
														//prepare 2 slots sprouting from previous parents with CS and LS, erase false tip 
														
														//inspection_mode();
													}
													else
													{ DV0(i, strip_start, j, strip_rank, curr_partner, posCP); }
												}
												else
												{ DV0(i, strip_start, j, strip_rank, curr_partner, posCP); }
											}
											
											else ///DV#1
											{ DV1(i, strip_start, j, strip_rank, curr_partner, posCP); }
										}
										else ///DV#Z
										{ DVZ(i, strip_start, j, strip_rank, curr_partner); }
										
										refresh_emptyslot();
									}
									
									///PHANTOM CASE
									
									else
									{
										//nothing, this case (CP alive)&&(partners>1) is impossible if partner's order is respected
										phantomcase_bug(j, strip_start, curr_partner, last_partner);
										
									}
									
								}
								
								
								//TIP0 TIP15 DEBUG  // STAGINVERSION
								if (((last_one>7)&&(mask[posCP][curr_partner]<mask[last_pos][last_one]))||(0))
								{	
									staginversion_bug(curr_partner, last_one, posCP, last_pos, i, strip_start, j);
								}
														
								if (pointer>=end)
								{
									char err_msg[20], value_name[10]; sprintf(err_msg, "pointer >= end"); sprintf(value_name, "pointer");
									error_message(200, 200, err_msg, 1, value_name, pointer);
									if (1)
									{
										//bwdisplay1();
										LED_GREEN;
										inspection_mode();
										while (1){}
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
													
								area = mask[area1][curr_partner]*256+mask[area0][curr_partner];
								
							} // end of main while loop // used to be 'if (overlap detected)'

							
						} // end of 'if stag isn't empty' // used to be for-q loop
						
						if (partners==0)
						{
							prepare_slot(empty_slot, 0, 6, 128, i, strip_start, j);
							
							mask[stud][strip_rank]=empty_slot;
							
							refresh_emptyslot();
						}
						
					} //if strip length is sufficient
					
					lng=0;
					
			  } //if we're currently on a white strip (and just encountered a black pixel)	
				
			} // else <==> if the pixel is not considered sufficiently bright to be white ==> pixel is black						
					
			
			//store a byte to matrix every eight pixels
			if (rest==7)
			{
				bwdiff[i][JJ]=bwbyte;
				bwbyte=0;
				JJ++;
			}
			
			if (freepic)
			{
				jj=j*3;
				
				READ_FIFO_PIXEL(Camera_Data);
				
				red =  GETR_FROM_RGB16(Camera_Data);
				green =  GETG_FROM_RGB16(Camera_Data);
				blue =  GETB_FROM_RGB16(Camera_Data);

				buffer[iii-1][jj]=red;
				buffer[iii-1][jj+1]=green;
				buffer[iii-1][jj+2]=blue;
				
				if (savepic)
					cam2SD_writepxl(red, green, blue);
			}
			
		} // end of for-j loop

		
	//
	// GENERAL CLEAN-UP AFTER READING THROUGH A LINE OF PIXELS
	//

	
		for (q=start; q<end; q++)
		{
			//update stag, erase stud
			mask[stag][q]=mask[stud][q];
			mask[stud][q]=0;
			
			posQ=mask[tip][q];
			
			// detect0 is on for slots that have been occupied for more than 1 j-loop
			if (mask[detect0][q]==126)
			{
				// incrementing tip's location
				if ((mask[alive][q]==24)&&(mask[succumb][q]==0))
				{		
					// breaking bodies that are too long in 2
					//if ((0)&&((mask[tip][q]-mask[root][q])>240))
					if ((1)&&((mask[tip][q]>base+maxlng*2)||(mask[tip][q]>secnd_slotline)))//140)) //taking the debugging grid into account
					{
						body_breakdown( q,  i);
					}
					else
					{
						mask[tip][q]++;
						mask[tip][q]++;
					}
				}
				
				/// DELETION ZONE
				if (((mask[alive][q]==0)||(mask[succumb][q]==24)))
				{
					// bodies that have to be stored
					//if ((abort==0)&&(((0)||((mask[tip][q]-tip)>10))||(mask[alive][q]==24)||(mask[sprout][q]>0))&&(mask[stored_alrd][q]==0))
					//if ((abort==0)&&(((0)||((mask[tip][q]-base)>10))||((mask[alive][q]==24)&&(mask[sprout][q]>0)))&&(mask[stored_alrd][q]==0))
					if ((abort==0)&&(((0)||((mask[area1][q])||(mask[area0][q]>0)))||((mask[alive][q]==24)&&(mask[sprout][q]>0)))&&(mask[stored_alrd][q]==0))
					{
						stored++;
						store_body(stored,q);	
						//mask[stored][7]=6; // debug					
					}
					track_activity(track, 0, q, 255);
					
					// erase slot
					posQ=mask[tip][q]; //tip is also going to be erased, need to store its value
					for(tmp=stag+1; tmp<posQ+2; tmp++)
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
			mask[succumb][q]=0;  // unnecessary in principle
		}
			
		//buffer lines actualization
		tmp=lnorder[0];
		lnorder[0]=lnorder[1];
		lnorder[1]=lnorder[2];
		lnorder[2]=tmp;
		
		if (freepic==0)
		{pic2bff_update(i+2);}  //we've processed line i, next up is line i+1 --> need i+2 to process it
		
		ILI9341_OpenWindow(0, i, 320, 240-i);
		ILI9341_Write_Cmd (CMD_SetPixel ); 
	} // end of for-i loop
	
	
	LED_RGBOFF;
		
	if ((freepic==0)||(savepic))
	{close_image();}

}

/*
uint8_t leftmost, rightmost;
int16_t vct0[2],vct1[2],vct0h[2],vct1h[2];
uint8_t hole;
uint8_t number;

void extendedDV0(char curr_partner, char posCP, int i, int strip_start, int j, char strip_rank )
{
	char q;
	short origin=mask[sprout][curr_partner];

	if ((1)&&(origin>=storedN)&&(mask[origin][col_sprout]==0)&&(posCP-base<5))
	{
		uint8_t hole = (mask[posCP+3][curr_partner]+strip_start)>>1;
		uint8_t number = mask[origin][nod_connect];
		uint8_t siblings[number];
		uint8_t sibling=0;
		for (q=8; (q<=stored)&&(sibling<number); q++)
		{
			if((mask[q][merge0]==origin)||(mask[q][merge1]==origin))
			{
				siblings[sibling]=q;
				sibling++;
			}
		}
		
		if(sibling<number-1)
		{error_message(50,50, "sibling number mismatch",0,"",0); LED_RED; while(1){}}
		
		leftmost=siblings[0], rightmost=siblings[number];	
		vct0[2]={mask[leftmost][col_j1]-mask[leftmost][col_j0],mask[leftmost][col_i1]-mask[leftmost][col_i0]}, 
		vct1[2]={mask[rightmost][col_j1]-mask[rightmost][col_j0],mask[rightmost][col_i1]-mask[rightmost][col_i0]},
		vct0h[2]={hole-mask[leftmost][col_j0],i-mask[leftmost][col_i0]}, 
		vct1h[2]={hole-mask[rightmost][col_j0],i-mask[rightmost][col_i0]};
		int16_t tol0=vct0[0]*vct0h[0]+vct0[1]*vct0[1], tol1=vct1[0]*vct1h[0]+vct1[1]*vct1[1];
		// check alignment
		if ((abs(vct0[0]*vct0h[1]-vct0[1]*vct0h[0])<5*tol0)||(abs(vct1[0]*vct1h[1]-vct1[1]*vct1h[0])<5*tol1))
		{
			for (q=0; q<start; q++)
				mask[origin][q]=0;
			
			mask[leftmost][col_i1]=i-1;
			mask[leftmost][col_j1]=(mask[posCP][curr_partner]+hole)/2;
			prepare_slot(empty_slot,leftmost,0,0,i,mask[posCP+2][curr_partner],mask[posCP+3][curr_partner]);
			
			refresh_emptyslot();
			
			mask[rightmost][col_i1]=i-1;
			mask[rightmost][col_j1]=(hole+mask[posCP+1][curr_partner])/2;
			prepare_slot(empty_slot,rightmost,0,0,i,strip_start,j);
			
			mask[posCP+2][curr_partner]=0;
			mask[posCP+3][curr_partner]=0;

			//erase node
			//current parent's true tip split into 2 and correct previous parents' stored tips
			//prepare 2 slots sprouting from previous parents with CS and LS, erase false tip 
			
		}
		else
		{ DV0(i, strip_start, j, strip_rank, curr_partner, posCP); }
	}
	else
	{ DV0(i, strip_start, j, strip_rank, curr_partner, posCP); }
}
*/
