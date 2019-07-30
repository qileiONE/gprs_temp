#ifndef __SHT10_H__
#define __SHT10_H__
#include "stm32f10x.h"

enum {TEMP, HUMI};

/* GPIOºê¶¨Òå */
#define SHT10_AHB2_CLK        RCC_APB2Periph_GPIOB
#define SHT10_DATA_PIN        GPIO_Pin_8
#define SHT10_SCK_PIN        	GPIO_Pin_9
#define SHT10_DATA_PORT       GPIOB
#define SHT10_SCK_PORT        GPIOB

#define SHT10_DATA_H()        GPIO_SetBits(SHT10_DATA_PORT, SHT10_DATA_PIN)        
#define SHT10_DATA_L()        GPIO_ResetBits(SHT10_DATA_PORT, SHT10_DATA_PIN)          
#define SHT10_DATA_R()        GPIO_ReadInputDataBit(SHT10_DATA_PORT, SHT10_DATA_PIN)      

#define SHT10_SCK_H()        GPIO_SetBits(SHT10_SCK_PORT, SHT10_SCK_PIN)           
#define SHT10_SCK_L()        GPIO_ResetBits(SHT10_SCK_PORT, SHT10_SCK_PIN)     


#define noACK        0
#define ACK          1
                             //addr  command         r/w
#define STATUS_REG_W        0x06        //000         0011          0      Ð´×´Ì¬¼Ä´æÆ÷
#define STATUS_REG_R        0x07        //000         0011          1      ¶Á×´Ì¬¼Ä´æÆ÷
#define MEASURE_TEMP         0x03        //000         0001          1     ²âÁ¿ÎÂ¶È
#define MEASURE_HUMI         0x05        //000         0010          1     ²âÁ¿Êª¶È
#define SOFTRESET       		0x1E        //000         1111          0      ¸´Î»

#define		SHT1X_ERR_OK		0
#define		SHT1X_ERR_FAULT	1

void SHT10_Config(void);
u8	getSHT1xTemp(float *out);
u8	getSHT1xHumi(float *out);

u8 SHT10_ReadStatusReg(u8 *p_value, u8 *p_checksum);
u8 SHT10_WriteStatusReg(u8 *p_value);
#endif

