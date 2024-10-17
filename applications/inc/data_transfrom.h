#pragma once

#include <stdint.h>
#define  STREAM_BUF_SIZE  8

typedef union data_tranform{
    uint8_t array[4];
    uint16_t u16;
    int16_t  s16;
    uint32_t u32;
    int32_t  s32;
    float    f32;  
}data_box_t;


typedef union dataSteam_tranform{
    uint8_t array[STREAM_BUF_SIZE];
	uint16_t u16_array[STREAM_BUF_SIZE/2];
	uint32_t u32_array[STREAM_BUF_SIZE/4];
}dataStream_box_t;



