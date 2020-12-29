//#include "bd_dt.h"
#include "./BD_subfct.h" 
#include "./BD_debug.h" 

void strip_append(int j, int strip_start, char curr_partner, char posCP, char last_partner, char strip_rank, char pointer);

void CV0(int i, int strip_start, int j, char strip_rank, char curr_partner, char last_partner, char posLP);

void CV2(int strip_start, int j,char curr_partner, char last_partner);

void CVX(int i, int strip_start, int j, char curr_partner, char last_partner);

void CVZ(char curr_partner, char last_partner);

void DV0(int i, int strip_start, int j, char strip_rank, char curr_partner, char posCP);

void DV1(int i, int strip_start, int j, char strip_rank, char curr_partner, char posCP);

void DVZ(int i, int strip_start, int j, char strip_rank, char curr_partner);

void body_breakdown(char slot, int i);

void body_absorb (char slot);