/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   RTC����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32F767 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f7xx.h"
#include "./clock/RTC/bsp_rtc.h"
#include "./usart/bsp_debug_usart.h"
#include "./lcd/bsp_lcd.h"
#include "./led/bsp_led.h"

RTC_HandleTypeDef Rtc_Handle;

__IO uint8_t Alarmflag =0;
/**
  * @brief  ����ʱ�������
  * @param  ��
  * @retval ��
  */
void RTC_TimeAndDate_Set(void)
{
    RTC_DateTypeDef  RTC_DateStructure;
    RTC_TimeTypeDef  RTC_TimeStructure;
	// ��ʼ��ʱ��
	RTC_TimeStructure.TimeFormat = RTC_H12_AMorPM;
	RTC_TimeStructure.Hours = HOURS;        
	RTC_TimeStructure.Minutes = MINUTES;      
	RTC_TimeStructure.Seconds = SECONDS;      
    HAL_RTC_SetTime(&Rtc_Handle,&RTC_TimeStructure, RTC_FORMAT_BIN);
    // ��ʼ������	
	RTC_DateStructure.WeekDay = WEEKDAY;       
	RTC_DateStructure.Date = DATE;         
	RTC_DateStructure.Month = MONTH;         
	RTC_DateStructure.Year = YEAR;        
    HAL_RTC_SetDate(&Rtc_Handle,&RTC_DateStructure, RTC_FORMAT_BIN);
    
    HAL_RTCEx_BKUPWrite(&Rtc_Handle,RTC_BKP_DRX, RTC_BKP_DATA);
}

/**
  * @brief  RTC���ã�ѡ��RTCʱ��Դ������RTC_CLK�ķ�Ƶϵ��
  * @param  ��
  * @retval ��
  */
void RTC_CLK_Config(void)
{
	RCC_OscInitTypeDef        RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	// ����RTC����
	Rtc_Handle.Instance = RTC;

	/*ʹ�� PWR ʱ��*/
	__HAL_RCC_PWR_CLK_ENABLE();
	/* PWR_CR:DBF��1��ʹ��RTC��RTC���ݼĴ����ͱ���SRAM�ķ��� */
	HAL_PWR_EnableBkUpAccess();

	#if defined (RTC_CLOCK_SOURCE_LSI) 
	/* ʹ��LSI��ΪRTCʱ��Դ������� 
	 * Ĭ��ѡ��LSE��ΪRTC��ʱ��Դ
	 */
	/* ��ʼ��LSI */ 
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	/* ѡ��LSI��ΪRTC��ʱ��Դ */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	#elif defined (RTC_CLOCK_SOURCE_LSE)
	/* ��ʼ��LSE */ 
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	/* ѡ��LSE��ΪRTC��ʱ��Դ */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	/* Configures the External Low Speed oscillator (LSE) drive capability */
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);  

	#endif /* RTC_CLOCK_SOURCE_LSI */

	/* ʹ��RTCʱ�� */
	__HAL_RCC_RTC_ENABLE(); 

	/* �ȴ� RTC APB �Ĵ���ͬ�� */
	HAL_RTC_WaitForSynchro(&Rtc_Handle);

	/*=====================��ʼ��ͬ��/�첽Ԥ��Ƶ����ֵ======================*/
	/* ����������ʱ��ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */

	/* �����첽Ԥ��Ƶ����ֵ */
	Rtc_Handle.Init.AsynchPrediv = ASYNCHPREDIV;
	/* ����ͬ��Ԥ��Ƶ����ֵ */
	Rtc_Handle.Init.SynchPrediv  = SYNCHPREDIV;	
	Rtc_Handle.Init.HourFormat   = RTC_HOURFORMAT_24; 
	/* ��RTC_InitStructure�����ݳ�ʼ��RTC�Ĵ��� */
	if (HAL_RTC_Init(&Rtc_Handle) != HAL_OK)
	{
		printf("\n\r RTC ʱ�ӳ�ʼ��ʧ�� \r\n");
	}	
	
	RTC_Check_Hrest();
}

void RTC_Check_Hrest(void)
{
		if ( HAL_RTCEx_BKUPRead(&Rtc_Handle,RTC_BKP_DRX) != 0x32F2)
	{
		/* ����ʱ������� */
		RTC_TimeAndDate_Set();
	}
	else
	{
		/* ����Ƿ��Դ��λ */
		if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
		{
			printf("\r\n������Դ��λ....\r\n");
		}
		/* ����Ƿ��ⲿ��λ */
		else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
		{
			printf("\r\n�����ⲿ��λ....\r\n");
		}
		printf("\r\n����Ҫ��������RTC....\r\n");    
		/* ʹ�� PWR ʱ�� */
		__HAL_RCC_RTC_ENABLE();
		/* PWR_CR:DBF��1��ʹ��RTC��RTC���ݼĴ����ͱ���SRAM�ķ��� */
		HAL_PWR_EnableBkUpAccess();
		/* �ȴ� RTC APB �Ĵ���ͬ�� */
		HAL_RTC_WaitForSynchro(&Rtc_Handle);
	} 
}
/**********************************END OF FILE*************************************/
