#ifndef __BSP_GSM_USART_H
#define	__BSP_GSM_USART_H

#include "stm32f7xx.h"
#include <stdio.h>


// GSM_GPRSʹ�õĴ���
#define  GSM_USARTx                          USART3
#define  GSM_USART_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE()
#define  GSM_USART_BAUDRATE                    115200

// USART GPIO ���ź궨��
#define  GSM_USART_TX_GPIO_CLK               __HAL_RCC_GPIOB_CLK_ENABLE()  
#define  GSM_TX_GPIO_PORT         		       GPIOB   
#define  GSM_TX_GPIO_PIN          		       GPIO_PIN_10
#define  GSM_TX_AF             			         GPIO_AF7_USART3


#define  GSM_USART_RX_GPIO_CLK               __HAL_RCC_GPIOB_CLK_ENABLE()  
#define  GSM_RX_GPIO_PORT       			       GPIOB
#define  GSM_RX_GPIO_PIN        			       GPIO_PIN_11
#define  GSM_RX_AF             			         GPIO_AF7_USART3



#define  GSM_USART_IRQ                       USART3_IRQn
#define  GSM_USART_IRQHandler                USART3_IRQHandler

void GSM_USART_Config(void);
void bsp_GSM_USART_IRQHandler(void);
char *get_rebuff(uint8_t *len);
void clean_rebuff(void);

void GSM_USART_printf(char *Data,...);

#endif /* __BSP_GSM_USART_H */
