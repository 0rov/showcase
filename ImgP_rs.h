

//#include "bd_dt.h"

#include "./lcd/bsp_ili9341_lcd.h"
#include "./bmp/bsp_bmp.h"
#include "./ov7725/bsp_ov7725.h"

#include "./led/bsp_led.h"
#include "./key/bsp_key.h"



void buffer_pixelgradient(int j, int ii, int iii, int i);
void buffer_pixelgradient_star(int j, int ii, int iii, int i);
void buffer_pixelgradient_L(int j, int ii, int iii);
void buffer_pixelgradient_square(int i, int j);
void buffer_pixelgradient_radial(int i, int j);

void initialize_buffer(void);

typedef unsigned char BYTE;

extern short a1;
extern char freepic, savepic, showpic, bwpic;

void draw_segments(void);
void draw_connections(void);
void draw_mergers(void);
void draw_sprouts(void);

uint8_t noise_eraser(int i, int j, char rest, uint8_t bwbyte, char halt, char display);


