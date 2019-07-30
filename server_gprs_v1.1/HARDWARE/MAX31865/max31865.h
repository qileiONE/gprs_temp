#ifndef __MAX31865_H
#define __MAX31865_H
#include "stm32f10x.h"


/* MAX31865�ο����� */
#define RREF  430  //400��

#define		DELAY_COUNT			20

#define	MAX_ERR_ok		0
#define	MAX_ERR_fail	1		//ͨѶʧ��
#define	MAX_ERR_fault	2		//31865��⵽����

#define	CONF_ON				0x80		// as power on������ON���ܲ���
#define	CONF_AUTO			0x40		//auto measure
#define	CONF_1SHOT		0x20		// trigger onece measure
#define	CONF_3WIRE		0x10		//3 wire mode
#define	CONF_CLEAR		0x02		//3 wire mode
#define	CONF_FILTER		0x01		//0 60hz filter  1 50 hz filter

//��ʼ�������üĴ����������Ҫ3���� ���� CONF_3WIRE
#define	MAX_CONFIG_INIT		(CONF_ON | CONF_AUTO | CONF_CLEAR | CONF_FILTER | CONF_3WIRE)

#define	MAX_REG_CONFIG		0x00
#define	MAX_REG_AD_MSB		0x01
#define	MAX_REG_AD_LSB		0x02
#define	MAX_REG_FAULT			0x07




/* MAX31865���ƿ� */
#define	MAX31865_GPIO_RCC				(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB)
#define MAX31865_CS_PORT  			GPIOA
#define MAX31865_CLK_PORT  			GPIOA
#define MAX31865_SDI_PORT  			GPIOA
#define MAX31865_SDO_PORT  			GPIOA
#define MAX31865_DRDY_PORT  		GPIOB

#define MAX31865_CS      GPIO_Pin_4
#define MAX31865_SCLK    GPIO_Pin_5
#define MAX31865_SDI     GPIO_Pin_7
#define MAX31865_SDO     GPIO_Pin_6

#define MAX31865_DRDY 	 GPIO_Pin_0  


#define MAX31865_CS_SET      GPIO_WriteBit(MAX31865_CS_PORT,MAX31865_CS,Bit_SET)
#define MAX31865_CS_CLR      GPIO_WriteBit(MAX31865_CS_PORT,MAX31865_CS,Bit_RESET)
#define MAX31865_SCLK_SET    GPIO_WriteBit(MAX31865_CLK_PORT,MAX31865_SCLK,Bit_SET)
#define MAX31865_SCLK_CLR    GPIO_WriteBit(MAX31865_CLK_PORT,MAX31865_SCLK,Bit_RESET)
#define MAX31865_SDI_SET     GPIO_WriteBit(MAX31865_SDI_PORT,MAX31865_SDI,Bit_SET)
#define MAX31865_SDI_CLR     GPIO_WriteBit(MAX31865_SDI_PORT,MAX31865_SDI,Bit_RESET)

#define MAX31865_SDO_READ    GPIO_ReadInputDataBit(MAX31865_SDO_PORT,MAX31865_SDO)
#define MAX31865_DRDY_READ   GPIO_ReadInputDataBit(MAX31865_DRDY_PORT,MAX31865_DRDY)

void MAX31865_Init(void);//��ʼ��		1
void MAX31865_Cfg(void);//����������
unsigned char MAX31865_GetTemp(float *out);//���¶�		2
unsigned char MAX31865_Read(unsigned char addr);//�����üĴ����ӿ�

#endif

