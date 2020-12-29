//#include "bd_dt.h"

#include "./bmp/bsp_bmp.h"

#include "./led/bsp_led.h"

extern double myPI;

void prepare_slot(char slot, uint8_t sprout_b, uint8_t merge_b1, uint8_t merge_b2, uint8_t s_height, uint16_t s_start, uint16_t s_finish);

int store_body(char stored, char slot);

void store_node(char storedN, char Nsprout, char i, char horizontal_pos);

void refresh_emptyslot(void);

void area_update(int j, int strip_start, char curr_partner);
void area_decrease(int j, int strip_start, char prtnr);
