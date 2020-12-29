
//#include "bd_dt.h"

#include "./lcd/bsp_ili9341_lcd.h"
#include "./bmp/bsp_bmp.h"
#include "./ov7725/bsp_ov7725.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "./ImgP_rs.h"

void track_activity(char boolean, char cases, uint8_t CP, uint8_t LP);

void trackparent(char curr_partner);

void staginversion_bug(uint8_t CP, uint8_t LP, uint8_t posCP, uint8_t posLP, int i, int strip_start, int j);
void phantomcase_bug(int j, int strip_start, char curr_partner, char last_partner);

void error_message( int i, int j, char * err_message, char hasvalue, char * value_name, int value);
void display_value(int value, int i, int j);

void inspection_mode(void);
void run_dblxprt(char * parameter, double value1, double value2);
void compare_output(int index1, int index2);

extern char empty_slot;

void maindebug(char * singleframe, char * frameBD, char * BDsegments, char * debugpanel);

void floppy_disc(int x, int y);
void tiny_floppy(int x, int y);

extern char glb_dbgpnl, loc_ersflg, loc_pnlhlf, glb_pnlhlf, loc_pnlend;

void recurse(int depth, int * counter_pt);
void multidimensional_test(int ndim, int maxdim, char * textd, int * ariadnes_thread, char * filename);