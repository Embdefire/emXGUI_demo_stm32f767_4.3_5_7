#ifndef __BOARD_H__
#define __BOARD_H__

/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/
/* STM32 �̼���ͷ�ļ� */
#include "stm32f7xx.h"

/* ������Ӳ��bspͷ�ļ� */
#include "./led/bsp_led.h" 
#include "./key/bsp_key.h" 
#include "./lcd/bsp_lcd.h"
#include "./sdram/bsp_sdram.h" 
#include "./touch/bsp_i2c_touch.h"
#include "./touch/gt9xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./flash/bsp_qspi_flash.h"
#include "./fonts/fonts.h"
#include "./tim/bsp_basic_tim.h"
#include "./bsp/mpu/bsp_mpu.h" 
#include "./clock/RTC/bsp_rtc.h"

/*
*************************************************************************
*                               ��������
*************************************************************************
*/
	

#endif /* __BOARD_H__ */
