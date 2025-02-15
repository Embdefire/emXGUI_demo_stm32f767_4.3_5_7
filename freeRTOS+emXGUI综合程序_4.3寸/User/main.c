/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0  + STM32 任务管理
  *********************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 全系列开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             包含的头文件
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
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */


/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */


/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
 
/* hardfault跟踪器需要的定义 */
#define HARDWARE_VERSION               "V1.0.0"
#define SOFTWARE_VERSION               "V0.1.0"

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void GUI_Thread_Entry(void* pvParameters);/* Test_Task任务实现 */
static void BSP_Init(void);
static void NAND_CE_INIT(void);
  
/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/

int main(void)
{	
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	
  BusClock_Config();
  /* 开发板硬件初始化 */
  BSP_Init();  
  
   /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )GUI_Thread_Entry,  /* 任务入口函数 */
                        (const char*    )"gui",/* 任务名字 */
                        (uint16_t       )5*1024,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )10, /* 任务的优先级 */
                        (TaskHandle_t*  )NULL);/* 任务控制块指针 */ 
  /* 启动任务调度 */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1;  
  
  while(1);   /* 正常不会执行到这里 */    
}

extern void GUI_Startup(void);

/**********************************************************************
  * @ 函数名  ： gui_thread_entry
  * @ 功能说明： gui_thread_entry任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void GUI_Thread_Entry(void* parameter)
{	
  
  printf("野火emXGUI演示例程\n\n");

  /* 执行本函数不会返回 */
	GUI_Startup();
  
  while (1)
  {
    LED1_ON;
    printf("Test_Task Running,LED1_ON\r\n");
    vTaskDelay(500);   /* 延时500个tick */
    
    LED1_OFF;     
    printf("Test_Task Running,LED1_OFF\r\n");
    vTaskDelay(500);   /* 延时500个tick */
  }
}

/***********************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/	
static void BSP_Init(void)
{
	SCB->CACR|=1<<2;   //强制D-Cache透写,如不开启,实际使用中可能遇到各种问题	  

	/* 设置SDRAM为Normal类型,禁用共享, 直写模式*/  
	Board_MPU_Config(0,MPU_Normal_WT,0x08000000,MPU_1MB);
	Board_MPU_Config(1,MPU_Normal_WT,0xD0000000,MPU_16MB);//GUI虚拟内存(动态分配)
	Board_MPU_Config(2,MPU_Normal_WT,0xD1000000,MPU_8MB);
	Board_MPU_Config(3,MPU_Normal_WT,0xD1BC0000,MPU_256KB);//HEAP空间
	Board_MPU_Config(4,MPU_Normal_WT,0x20000000,MPU_512KB);
		
  /* Enable I-Cache */
  SCB_EnableICache(); 
  /* Enable D-Cache */
  SCB_EnableDCache();

  /* 系统时钟初始化成216MHz,在启动文件中初始化 */
//	SystemClock_Config();
	
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* 初始化SysTick */
  HAL_SYSTICK_Config( HAL_RCC_GetSysClockFreq() / configTICK_RATE_HZ );	

	/* 硬件BSP初始化统统放在这里，比如LED，串口，LCD等 */

	/* LED 端口初始化 */
	LED_GPIO_Config();
  
  /* 禁用 NAND */
  NAND_CE_INIT():
  
  /* SDRAM 初始化,在启动文件中初始化 */
//  SDRAM_Init();

	/* usart 端口初始化 */
  Debug_USART_Config();

  /* KEY 端口初始化 */
  Key_GPIO_Config();

	RTC_CLK_Config();

	/* 检测WM8978芯片，此函数会自动配置CPU的GPIO */
	if (wm8978_Init()==0)
	{
		printf("检测不到WM8978芯片!!!\n");
		while (1);	/* 停机 */
	}
	printf("WM8978芯片初始化成功\n");
	MODIFY_REG(FMC_Bank1->BTCR[0],FMC_BCR1_MBKEN,0); //关闭FMC_Bank1,不然LCD会闪.

  /*hardfault 跟踪器初始化*/ 
  cm_backtrace_init("Fire_FreeRTOS", HARDWARE_VERSION, SOFTWARE_VERSION);
}
 
/**
  * @brief  禁用NAND FLASH
  * @param  无
  * @param  无
  * @retval 无
  */
static void NAND_CE_INIT(void)
{
  /*定义一个GPIO_InitTypeDef类型的结构体*/
  GPIO_InitTypeDef GPIO_InitStruct;
  /*使能引脚时钟*/        
  __HAL_RCC_GPIOG_CLK_ENABLE();
  /*选择要控制的GPIO引脚*/                                                                                                                           
  GPIO_InitStruct.Pin = GPIO_PIN_9;        
  /*设置引脚的输出类型为推挽输出*/
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;      
  /*设置引脚为上拉模式*/
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  /*设置引脚速率为高速 */   
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST; 
  /*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);        
  /*禁用NAND FLASH*/
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_SET);  
}

/**
  * @brief  System Clock 配置
  *         system Clock 配置如下 : 
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
  * @param  无
  * @retval 无
  */
	
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  HAL_StatusTypeDef ret = HAL_OK;

  /* 使能HSE，配置HSE为PLL的时钟源，配置PLL的各种分频因子M N P Q 
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
  
  /* 激活 OverDrive 模式以达到216M频率  */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* 选择PLLCLK作为SYSCLK，并配置 HCLK, PCLK1 and PCLK2 的时钟分频因子 
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
	
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART7;
  PeriphClkInitStruct.Uart7ClockSelection = RCC_UART7CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
		while(1) { ; }
  }
}

/*
	* 使用外部SDRAM作为堆区,得先初始化SDRAM,故在.S文件里初始化系统时钟和总线时钟。
	* 但跳转从.S跳转回main中时,总线时钟会被重新初始化,所以再初始化一遍确保时钟正确。
	*/
void BusClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  HAL_StatusTypeDef ret = HAL_OK;

  /* 使能HSE，配置HSE为PLL的时钟源，配置PLL的各种分频因子M N P Q 
	 * PLLCLK = HSE/M*N/P = 25M / 25 *432 / 2 = 216M
	 */
  
  /* 选择PLLCLK作为SYSCLK，并配置 HCLK, PCLK1 and PCLK2 的时钟分频因子 
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
	
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART7;
  PeriphClkInitStruct.Uart7ClockSelection = RCC_UART7CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
		while(1) { ; }
  }
}
/*********************************************END OF FILE**********************/

