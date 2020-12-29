#include "./BD_cvdv.h"

void strip_append(int j, int strip_start, char curr_partner, char posCP, char last_partner, char strip_rank, char pointer)
{
	mask[posCP+2][curr_partner]=strip_start;
	mask[posCP+3][curr_partner]=j;

	mask[alive][curr_partner]=24;
	
	mask[stud][strip_rank]=mask[stag][pointer];  // value equal to curr_partner
	
	// stud <--> strip_rank, stag <--> pointer
	/*
	area+=j-1-strip_start;
	
	if (area>255)
	{
		mask[area0][curr_partner]=area&255;
		mask[area1][curr_partner]=(area&65281)>>8;
	}
	else
	{mask[area0][curr_partner]=area;}
	//*/
	area_update(j, strip_start, curr_partner);
						
	track_activity(track,1, curr_partner, 170); //170=error
}

void CV0(int i, int strip_start, int j, char strip_rank, char curr_partner, char last_partner, char posLP)
{
	storedN--;
	//store_node(storedN, 0, i,(strip_start+j)>>1);
	store_node(storedN, 0, i,(strip_start+j)/2);
	
	//prepare_slot(empty_slot, 0, 1, 2, i, strip_start, j); //3rd and 4th argument correspond to merge 0 and 1: useless//used to debug
	prepare_slot(empty_slot, storedN, 1, 2, i, strip_start, j);  // the slot is supposed to sprout from the node...
	
	mask[stud][strip_rank]=empty_slot;
	
	mask[sprout][empty_slot]=storedN;
	mask[merge0][curr_partner]=storedN;												
	mask[merge1][last_partner]=storedN;
	
	mask[posLP+2][last_partner]=0;
	mask[posLP+3][last_partner]=0;
	
	/*
	if (mask[area0][last_partner]>=(j-1-strip_start))
	{mask[area0][last_partner]-=(j-1-strip_start);}
	else if (mask[area1][last_partner])
	{
		mask[area0][last_partner]+=255-(j-1-strip_start);
		mask[area1][last_partner]--;
	}
	else
	{
		//error
		error_message(100,10,"negative area implied",1,"area", mask[area1][last_partner]*256+mask[area0][last_partner]);
		LED_RED;
		while(1){}
	}
	//*/
	area_decrease(j, strip_start, last_partner);
	
	stored++;
	store_body(stored, last_partner);
	//mask[stored][7]=24; // debug
	
	
	mask[stored_alrd][last_partner]=stored;
	mask[succumb][last_partner]=24;
	
	stored++;
	store_body(stored, curr_partner);
	//mask[stored][7]=96; // debug
	
	mask[stored_alrd][curr_partner]=stored;
	mask[succumb][curr_partner]=24;
	
	//refresh empty_slot's value
	refresh_emptyslot();

	track_activity(track, 2, curr_partner, last_partner);
}

void CV2(int strip_start, int j,char curr_partner, char last_partner)
{
	mask[storedN][col_sprout]=0;
	mask[storedN][col_j1]=(strip_start+j)/2;
	mask[storedN][col_i1]--;                  //Hsize-(i) = (Hsize-(i-1))-1 
	
	mask[storedN][nod_connect]=2; //presumably was =1 after DV1
	
	mask[merge1][last_partner]=storedN; // LP has already been stored, hence the following line
	mask[stored][col_merge1]=storedN;
	
	mask[merge0][curr_partner]=storedN;
	
	stored++;
	store_body(stored, curr_partner);
														
	mask[stored_alrd][curr_partner]=stored;
	mask[succumb][curr_partner]=24;
	
	track_activity(track, 3, curr_partner, last_partner);
}

void CVX(int i, int strip_start, int j, char curr_partner, char last_partner)
{
	mask[storedN][nod_connect]=1; //presumably was =2 after DV0
	
	storedN--;
	//store_node(storedN, 0, i, (strip_start+j)>>1);
	store_node(storedN, 0, i, (strip_start+j)/2);
	
	mask[sprout][new_slot]=storedN;
	
	mask[merge1][last_partner]=storedN; // LP has already been stored, hence the following line
	mask[stored][col_merge1]=storedN;
	
	mask[merge0][curr_partner]=storedN;
	
	stored++;
	store_body(stored, curr_partner);
	
	mask[stored_alrd][curr_partner]=stored;
	mask[succumb][curr_partner]=24;

	track_activity(track, 4, curr_partner, last_partner);
}

void CVZ(char curr_partner, char last_partner)
{
	mask[merge0][curr_partner]=storedN;
	mask[storedN][nod_connect]++;
	
	stored++;
	store_body(stored, curr_partner);
	
	mask[stored_alrd][curr_partner]=stored;
	mask[succumb][curr_partner]=24;

	track_activity(track, 5, curr_partner, last_partner);
}

void DV0(int i, int strip_start, int j, char strip_rank, char curr_partner, char posCP)
{
	storedN--;
	//store_node(storedN, 0, i-1, (mask[posCP][curr_partner]+mask[posCP+1][curr_partner])>>1);
	store_node(storedN, 0, i-1, (mask[posCP][curr_partner]+mask[posCP+1][curr_partner])/2);
	
	prepare_slot(empty_slot, storedN, 2, 16, i, mask[posCP+2][curr_partner], mask[posCP+3][curr_partner]);
	
	trackparent(curr_partner);
	
	area_decrease(mask[posCP+3][curr_partner], mask[posCP+2][curr_partner], curr_partner);
	
	mask[posCP+2][curr_partner]=0;
	mask[posCP+3][curr_partner]=0;
	
	mask[stud][strip_rank-1]=empty_slot; // last_strip was previously appended to current_partner,
																			 // hence, current_partner was stored to the stud at last_strip's rank
	
	stored++;
	store_body(stored, curr_partner);
	
	mask[succumb][curr_partner]=24;
	mask[stored_alrd][curr_partner]=stored;
	
	mask[storedN][col_sprout]=stored;
	refresh_emptyslot();

	prepare_slot(empty_slot, storedN, 3, 17, i, strip_start, j);
													
	mask[stud][strip_rank]=empty_slot; // add current_strip to the stud
	
	track_activity(track, 6, curr_partner, 170); //170=10101010, problem
}

void DV1(int i, int strip_start, int j, char strip_rank, char curr_partner, char posCP)
{
	storedN--;
	//store_node(storedN, stored, i-1, (mask[posCP][curr_partner]+mask[posCP+1][curr_partner])>>1);
	store_node(storedN, stored, i-1, (mask[posCP][curr_partner]+mask[posCP+1][curr_partner])/2);
	mask[storedN][nod_connect]--;
	
	prepare_slot(empty_slot, storedN, 4, 32, i, strip_start, j);
	
	mask[stud][strip_rank]=empty_slot;

	track_activity(track, 7, curr_partner, 170); //170=problem
}

void DVZ(int i, int strip_start, int j, char strip_rank, char curr_partner)
{
	prepare_slot(empty_slot, storedN, 5, 64, i, strip_start, j);
	mask[storedN][nod_connect]++;
	
	mask[stud][strip_rank]=empty_slot;

	track_activity(track, 8, curr_partner, 170); //170=error
}

void body_breakdown(char slot, int i)
{
	char pos = mask[tip][slot], true_sprout;
	stored++;
	/*
	store_body(stored,slot);
	mask[succumb][slot]=24;
	mask[stored_alrd][slot]=stored;
	
	prepare_slot(empty_slot, stored, 7, 0, i, mask[pos+2][slot], mask[pos+3][slot]);
	//prepare_slot(empty_slot, 0, 7, 0, i, mask[pos+2][slot], mask[pos+3][slot]);
	//*/
	
	char is_stored =store_body(stored,slot);
	
	if (is_stored==1)
		true_sprout=stored;
	else if (is_stored==0)
		true_sprout=mask[sprout][slot];
	else
	{error_message(50,50,"store_body didn't work",0,"",0); LED_RED; while(1){}}
	/*
	if (true_sprout==0)
	{LED_RED; while(1){}}
	//*/
	mask[succumb][slot]=24;
	mask[stored_alrd][slot]=true_sprout;
	prepare_slot(empty_slot, true_sprout, 7, 0, i, mask[pos+2][slot], mask[pos+3][slot]);
	//*/
	mask[pos+2][slot]=0;
	mask[pos+3][slot]=0;
	
	if (empty_slot<slot)
	{
		mask[detect0][empty_slot]=126;
		mask[alive][empty_slot]=0;
	}
	
	//debug: cross and square
	mask[glb_dbgpnl-11][empty_slot]=24;
	mask[glb_dbgpnl-10][empty_slot]=24;
	mask[glb_dbgpnl-7][slot]=8;
	mask[glb_dbgpnl-6][slot]=28;
	mask[glb_dbgpnl-5][slot]=8;
	
	//setting stag to new value
	
	char p;
	
	for (p=start;p<end; p++)
	{
		
		if ((p<=slot)&&(mask[stag][p]==slot))
		{
			mask[stag][p]=empty_slot;
			break;
		}
		else if ((p>slot)&&(mask[stud][p]==slot))
		{
			mask[stud][p]=empty_slot;
			break;
		}
		else {} //nothing, carry on
			
		if (p==end){while(1){LED_BLUE;LED_YELLOW;}}
	}
	
	refresh_emptyslot();
}

//if (parent < stored)&&(angle is similar)

void body_absorb (char slot)
{
	uint8_t pos=mask[tip][slot], angle;
	uint16_t parent = mask[sprout][slot];
	uint16_t area_p = mask[parent][col_area1]*256+mask[parent][col_area0];
	
	//mask[sprout][slot]=mask[parent][col_sprout];
	
	//angle= atan2(((pos-tip)/2),(mask[parent][col_j1]-mask[parent][col_j0]))*180/myPI;
	
	mask[parent][col_i1]=mask[root][slot]+((pos-base)/2);
	mask[parent][col_j1]=(mask[pos][slot]+mask[pos+1][slot])>>1;
	
	//angle= atan2(((pos-tip)/2),(mask[parent][col_j1]-mask[parent][col_j0]))*180/myPI;
	angle= atan2(((mask[root][slot]+(pos-base)/2)-mask[parent][col_i0]),(mask[parent][col_j1]-mask[parent][col_j0]))*180/myPI;
	
	mask[parent][col_angle]=angle;
	
	area_p+=mask[area1][slot]*256+mask[area0][slot];
	
	mask[parent][col_area0]=area_p&255;
	mask[parent][col_area1]=(area_p&65281)>>8;
	
	mask[parent][col_merge0]=mask[merge0][slot];
	mask[parent][col_merge1]=mask[merge1][slot];
	
	stored--;
}

