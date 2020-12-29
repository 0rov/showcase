#include "./lcd/bsp_ili9341_lcd.h"
#include "./bmp/bsp_bmp.h"
#include "./ov7725/bsp_ov7725.h"

#include "./led/bsp_led.h"
#include "./key/bsp_key.h"

#define BMP_DEBUG_PRINTF(FORMAT,...)  printf(FORMAT,##__VA_ARGS__)	


void load_image(char *pic1_name);
void load_pxlns2bff(void);
void pic2bff_update(int i);
void close_image(void);

void show_mybmp ( char * pic_name, char flipv );
void show_bmpdiff ( char * pic1_name, char * pic2_name );

int cam2SD_prepare(char * filename);
void cam2SD_writepxl(BYTE r, BYTE g, BYTE b);
int matrix2picture(char factor, char matrix_id, char * filename);

int textfile_save(int length, char * content, char * filename);
void textfile_read(int start, int length, char * content, char * filename);
void newbmp_index_old(int * name_count_pt, char * singleframe, char * frameBD, char * BDsegments, char * debugpanel);
int newbmp_index(char * locus, char nb_arg, ...);

int min(int days, int arg_count, ...);

#include "ff.h"
//FRESULT scan_files ( char* path );
void scan_bmps ( char* path );
