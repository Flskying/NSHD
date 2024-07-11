#ifndef __biaopan_H
#define __biaopan_H
#include "stm32f10x.h"                  // Device header
#include <string.h>

void biaopan(void);
void biaopan_io_init(void);
void biaopan_byd(u8 x,u8 y);
void bittt(u8 x,u8 y,u8 i,u8 p);
void biaopan_Timer(u8 x,u8 y);
void biaopan_Timer_data(void);
void biaopan_Timer_small(u8 x,u8 y);
void biaopan_date(uint8_t x,uint8_t y);
void biaopan_week(uint8_t x,uint8_t y);
void biaopan_APP(uint8_t x,uint8_t y);


#endif

