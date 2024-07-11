#ifndef __MP3_tf_H
#define __MP3_tf_H
#include "stm32f10x.h"                  // Device header

void MP3_Init(void);
void MP3_Power_OFF(void);
void MP3_chin(uint8_t e);

void MP3os(void);

void MP3ping(void);
void MP3CMD(uint8_t CMD ,uint8_t feedback , uint16_t dat);
void MP3_starts(uint8_t e);

	
extern uint8_t mp3play1;	//播放标志位
extern uint8_t mp3while;	//循环播放标志位
extern uint8_t mp3data;   //MP3需发送数据位
extern uint8_t mp3volume;   //MP3音量
extern uint8_t mp3PLSD;
extern uint8_t mp3chapter;   //MP3歌曲位
extern uint8_t mp3chapterMAX;   //MP3歌曲位
#endif
