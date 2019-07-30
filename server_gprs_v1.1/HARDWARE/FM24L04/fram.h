#ifndef __FRAM_H
#define __FRAM_H


#include "stm32f10x.h"
/*FRAM寄存器定义*/

#define FRAM_ADDR_CMD		0xa0	/*命令*/

#define		FRAM_CLK_SET()	{ GPIOB->BSRR  = GPIO_Pin_15; }	  	//FRAM CLK = 1
#define		FRAM_CLK_CLR()	{ GPIOB->BRR  = GPIO_Pin_15; }		//FRAM CLK = 0

#define		FRAM_SDA_SET()	{ GPIOA->BSRR  = GPIO_Pin_8; }	  	//FRAM SDA = 1
#define		FRAM_SDA_CLR()	{ GPIOA->BRR  = GPIO_Pin_8; }		//FRAM SDA = 0


#define		FRAM_SDA( )	  (GPIOA->IDR & 0x100	)			//PB15


extern u8 write_str_sub(u8 sla,u16 suba,u8 *s,u16 no);		//写数据
extern u8 read_str_sub(u8 sla,u16 suba,u8 *s,u16 no);		//读数据

extern u8 Fram_Write_Data( u16 suba,u8 *s,u16 no );
extern u8 Fram_Read_Data( u16 suba,u8 *s,u16 no );

extern void write_file_sys_bit( void );
extern void GPIO_INIT_Fram( void );


extern u16 write_data( u16 addr,u16 len,u8 *pbuf );
extern void write_bit( u16 addr,u16 len );
extern u16 read_bit( u16 addr );
extern void read_data( u16 addr,u16 len,u8 *pbuf );


void Delayms( int n );


#endif







