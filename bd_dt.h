//#ifndef __BORDER_DETECTION_H
#define	__BORDER_DETECTION_H

#include "stm32f10x.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "./lcd/bsp_ili9341_lcd.h"
#include "./bmp/bsp_bmp.h"
#include "./ov7725/bsp_ov7725.h"

#include "./led/bsp_led.h"
#include "./key/bsp_key.h"

#include "./BD_debug.h"
#include "./ImgP_rs.h"
#include "./BD_subfct.h" 
#include "./BD_cvdv.h" 
#include "./file_access.h" 


typedef unsigned char	BYTE;

extern char col_i0, col_j0, col_i1, col_j1, col_sprout, col_merge0, col_merge1, col_angle, col_area1, col_area0, col_debug;
extern char /*nod_parents, nod_children,*/ nod_connect;
extern char stud, stag, detect0, detect1, alive, succumb, byte4, byte5, byte6, sprout, stored_alrd, merge0, merge1, root, base, tip;
extern uint8_t length, angle, ave_width, area1, area0, bbox_x, bbox_X, bbox_y, bbox_Y, fine, overlap0_0, overlap0_1, overlap0_2, overlap1_0, overlap1_1, overlap1_2;
extern uint8_t stored, storedN;
extern const char start, end;
extern char track;
extern char empty_slot, new_slot;
extern char lnorder[3], savedframe[40];

extern char tmp_storage_end, secnd_slotline, third_slotline, max_slotlng;
extern uint8_t maxlng, maxtol;
extern uint16_t area;

void border_det3(void);

extern int total_bodies, absrb_bodies;