/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0  + STM32 �������
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 ȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "stm32f7xx.h"
#include "main.h"
#include "board.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"   
#include "./usart/bsp_debug_usart.h"
#include "./touch/gt9xx.h"
#include "./bsp/mpu/bsp_mpu.h" 
#include <cm_backtrace.h>
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */


/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */


/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
 
/* hardfault��������Ҫ�Ķ��� */
#define HARDWARE_VERSION               "V1.0.0"
#define SOFTWARE_VERSION               "V0.1.0"

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void GUI_Thread_Entry(void* pvParameters);/* Test_Task����ʵ�� */
static void BSP_Init(void);
  
/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  /* ������Ӳ����ʼ�� */
  BSP_Init();  
  
   /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )GUI_Thread_Entry,  /* ������ں��� */
                        (const char*    )"gui",/* �������� */
                        (uint16_t       )5*1024,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )10, /* ��������ȼ� */
                        (TaskHandle_t*  )NULL);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}

extern void GUI_Startup(void);

/**********************************************************************
  * @ ������  �� gui_thread_entry
  * @ ����˵���� gui_thread_entry��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void GUI_Thread_Entry(void* parameter)
{	
  
  printf("Ұ��emXGUI��ʾ����\n\n");

  /* ִ�б��������᷵�� */
	GUI_Startup();
  
  while (1)
  {
    LED1_ON;
    printf("Test_Task Running,LED1_ON\r\n");
    vTaskDelay(500);   /* ��ʱ500��tick */
    
    LED1_OFF;     
    printf("Test_Task Running,LED1_OFF\r\n");
    vTaskDelay(500);   /* ��ʱ500��tick */
  }
}

/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/

static void BSP_Init(void)
{ 
	SCB->CACR|=1<<2;   //ǿ��D-Cache͸д,�粻����,ʵ��ʹ���п���������������	  

	/* ����SDRAMΪNormal����,���ù���, ֱдģʽ*/  
	Board_MPU_Config(0,MPU_Normal_WT,0xD0000000,MPU_16MB);
	Board_MPU_Config(1,MPU_Normal_WT,0xD1000000,MPU_8MB);
	Board_MPU_Config(2,MPU_Normal_WT,0x20020000,MPU_256KB);
  Board_MPU_Config(3,MPU_Normal_WT,0x20060000,MPU_128KB);
	Board_MPU_Config(4,MPU_Normal_WT,0x20000000,MPU_128KB);
//	
//	MPU_Config();	
	
  /* Enable I-Cache */
  SCB_EnableICache(); 
  /* Enable D-Cache */
  SCB_EnableDCache();
	
  /* ϵͳʱ�ӳ�ʼ����216MHz */
	SystemClock_Config();
	
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* ��ʼ��SysTick */
  HAL_SYSTICK_Config( HAL_RCC_GetSysClockFreq() / configTICK_RATE_HZ );	

	/* Ӳ��BSP��ʼ��ͳͳ�����������LED�����ڣ�LCD�� */

	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
  
  /* SDRAM ��ʼ�� */
  SDRAM_Init();
	
	/* usart �˿ڳ�ʼ�� */
  Debug_USART_Config();

  /* KEY �˿ڳ�ʼ�� */
  Key_GPIO_Config();

	RTC_CLK_Config();

	/* ���WM8978оƬ���˺������Զ�����CPU��GPIO */
	if (wm8978_Init()==0)
	{
		printf("��ⲻ��WM8978оƬ!!!\n");
		while (1);	/* ͣ�� */
	}
	printf("WM8978оƬ��ʼ���ɹ�\n");
	
	MODIFY_REG(FMC_Bank1->BTCR[0],FMC_BCR1_MBKEN,0); //�ر�FMC_Bank1,��ȻLCD����.

  /*hardfault ��������ʼ��*/ 
  cm_backtrace_init("Fire_FreeRTOS", HARDWARE_VERSION, SOFTWARE_VERSION);
}
 

/**
  * @brief  System Clock ����
  *         system Clock �������� : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  ��
  * @retval ��
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* ʹ��HSE������HSEΪPLL��ʱ��Դ������PLL�ĸ��ַ�Ƶ����M N P Q 
	 * PLLCLK = HSE/M*N/P = 25M / 25 *432 / 2 = 216M
	 */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* ���� OverDrive ģʽ�Դﵽ216MƵ��  */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* ѡ��PLLCLK��ΪSYSCLK�������� HCLK, PCLK1 and PCLK2 ��ʱ�ӷ�Ƶ���� 
	 * SYSCLK = PLLCLK     = 216M
	 * HCLK   = SYSCLK / 1 = 216M
	 * PCLK2  = SYSCLK / 2 = 108M
	 * PCLK1  = SYSCLK / 4 = 54M
	 */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 
  
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }  
}

/*********************************************END OF FILE**********************/

