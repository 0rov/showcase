
#include "./BD_subfct.h" 

//void prepare_slot(char slot, uint8_t sprout_b, uint8_t merge_b1, uint8_t merge_b2, uint8_t s_height, uint16_t s_start, uint16_t s_finish)
void prepare_slot(char slot, uint8_t sprout_b, uint8_t track_1, uint8_t merge_b2, uint8_t s_height, uint16_t s_start, uint16_t s_finish)
{
	if (slot<=end) // make start and end extern if possible
	{
		mask[detect0][slot]=0;           // set byte#0 to OFF to prevent slot from being processed at the end of this turn
		mask[detect1][slot]=126;
		
		mask[alive][slot]=24;  // use? detect0 does the trick...
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
		
		mask[area0][slot]=s_finish-s_start;
		
		mask[base][slot]=s_start;
		mask[base+1][slot]=s_finish;
		/*
		//debug
		mask[189][slot] = 84;
		mask[190][slot] = merge_b2;
		*/
		//debug
		char posQ;
		posQ=mask[glb_dbgpnl][slot];
		posQ++;
		mask[glb_dbgpnl+posQ][slot]=112+track_1; //112=64+32+16
		mask[glb_dbgpnl][slot]=posQ;

	}
}

int store_body(char stored, char slot)
{	
	uint8_t pos, angle;
	uint16_t parent = mask[sprout][slot];
	pos=mask[tip][slot];
	angle= atan2(((pos-base)/2),(((mask[pos][slot]+mask[pos+1][slot])>>1)-((mask[base][slot]+mask[base+1][slot])>>1)))*180/myPI;
	
	if((parent>0)&&(parent<=stored))
		total_bodies++;
	
	if ((1)&&(parent>0)&&(parent<=stored)&&((abs(angle-mask[parent][col_angle])<=5/*maxtol*/)||(mask[parent][col_i1]-mask[parent][col_i0]<=3)))
	{
		absrb_bodies++;
		body_absorb (slot);
		return 0;
	}
	else
	{
		mask[stored][col_i0]=mask[root][slot];
		
		mask[stored][col_j0]=(mask[base][slot]+mask[base+1][slot])>>1;
		
		mask[stored][col_i1]=mask[root][slot]+((pos-base)/2);

		mask[stored][col_j1]=(mask[pos][slot]+mask[pos+1][slot])>>1;
		
		mask[stored][col_sprout]=mask[sprout][slot];
		mask[stored][col_merge0]=mask[merge0][slot];
		mask[stored][col_merge1]=mask[merge1][slot];
		
		mask[stored][col_angle]=angle;
		
		mask[stored][col_area1]=mask[area1][slot];
		mask[stored][col_area0]=mask[area0][slot];
		
		/*
		position 0: ordinate of segment's start-strip
		position 1: average abscissa of segment's start-strip
		position 2: ordinate of segment's end-strip
		position 3: average abscissa of segment's end-strip
		position 4: segment's father
		position 5: segment's left merger
		position 6: segment's right merger
		
		*/
		
		return 1;
	}
}

void store_node(char storedN, char Nsprout, char i, char horizontal_pos)
{
	char vertical_pos = i;
	
	mask[storedN][col_i0]=0;
	mask[storedN][col_j0]=0;
	
	mask[storedN][col_i1]=vertical_pos;
	mask[storedN][col_j1]=horizontal_pos;
	
	mask[storedN][col_sprout]=Nsprout;
	
	mask[storedN][col_merge0]=0;
	mask[storedN][col_merge1]=0;
	
	mask[storedN][nod_connect]=2;
	
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

void refresh_emptyslot(void)
{
	new_slot=empty_slot;
	char p;
		
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

void area_update(int j, int strip_start, char curr_partner)
{
	area+=j-strip_start;
	
	if (area>255)
	{
		mask[area0][curr_partner]=area&255;
		mask[area1][curr_partner]=(area&65281)>>8;
	}
	else
	{mask[area0][curr_partner]=area;}
}

void area_decrease(int j, int strip_start, char prtnr)
{
	if (mask[area0][prtnr]>=(j-0-strip_start))
	{mask[area0][prtnr]-=(j-0-strip_start);}
	else if (mask[area1][prtnr])
	{
		mask[area0][prtnr]+=255-(j-0-strip_start);
		mask[area1][prtnr]--;
	}
	else
	{
		//error
		error_message(100,10,"negative area implied",1,"area", mask[area1][prtnr]*256+mask[area0][prtnr]);
		display_value(j-strip_start,150,10);
		display_value(prtnr,170,10);
		LED_RED;
		inspection_mode();
		//while(1){}
	}//*/
}
