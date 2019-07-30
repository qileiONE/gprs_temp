#include "max31865.h"
#include "math.h"
#include "usart.h"
#include "delay.h"


/* MAX31865 初始化 */
void MAX31865_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure ;

	RCC_APB2PeriphClockCmd(MAX31865_GPIO_RCC,ENABLE);

	GPIO_InitStructure.GPIO_Pin = MAX31865_CS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MAX31865_CS_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MAX31865_SCLK;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MAX31865_CLK_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MAX31865_SDI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MAX31865_SDI_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MAX31865_SDO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(MAX31865_SDO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MAX31865_DRDY;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(MAX31865_DRDY_PORT,&GPIO_InitStructure);
	
	MAX31865_CS_SET;
	MAX31865_SCLK_SET;
	
	MAX31865_Cfg();
}

void DelayCount(vu32 nCount) 
{ 
  for(; nCount != 0; nCount--); 
} 

/* MAX31865 写寄存器 
addr:寄存器地址
data:数据
*/
void MAX31865_Write(unsigned char addr, unsigned char data)
{
	unsigned char i;
	
	addr = addr | 0x80;
	
	MAX31865_CS_CLR;
	DelayCount(DELAY_COUNT);
	for(i=0;i<8;i++)  //写地址
	{
		MAX31865_SCLK_CLR;
		if(addr&0x80) MAX31865_SDI_SET;
		else MAX31865_SDI_CLR;
		DelayCount(DELAY_COUNT);
		MAX31865_SCLK_SET;
		addr<<=1;
		DelayCount(DELAY_COUNT);
	}
	for(i=0;i<8;i++)  //写数据
	{
		MAX31865_SCLK_CLR;
		DelayCount(DELAY_COUNT);
		if(data&0x80) MAX31865_SDI_SET;
		else MAX31865_SDI_CLR;
		DelayCount(DELAY_COUNT);
		MAX31865_SCLK_SET;
		data<<=1;
		DelayCount(DELAY_COUNT);
	}
	MAX31865_CS_SET;
}

/* MAX31865 读寄存器 
addr:寄存器地址
*/
unsigned char MAX31865_Read(unsigned char addr)
{
	unsigned char i;
	unsigned char data=0;
	
	MAX31865_CS_CLR;
	DelayCount(DELAY_COUNT);
	for(i=0;i<8;i++)  //写地址
	{
		MAX31865_SCLK_CLR;
		if(addr&0x80) MAX31865_SDI_SET;
		else MAX31865_SDI_CLR;
		DelayCount(DELAY_COUNT);
		MAX31865_SCLK_SET;
		addr<<=1;
		DelayCount(DELAY_COUNT);
	}
	for(i=0;i<8;i++)  //读数据
	{
		MAX31865_SCLK_CLR;
		DelayCount(DELAY_COUNT);
		data<<=1;		
		MAX31865_SCLK_SET;		
		if(MAX31865_SDO_READ) data|=0x01;
		else data|=0x00;
		DelayCount(DELAY_COUNT);
	}
	MAX31865_CS_SET;
	return data;
}

/* MAX31865 配置*/
void MAX31865_Cfg(void)
{
	MAX31865_Write(MAX_REG_CONFIG, MAX_CONFIG_INIT); //BIAS ON,自动，4线，50HZ  根据文件修改四线还是三线
}

/* MAX31865 获取温度 */
unsigned char MAX31865_GetTemp(float *out)
{ 
	unsigned int data;
	float Rt;
	float Rt0 = 100;  //PT100	
	float Z1,Z2,Z3,Z4,temp;
	float a = 3.9083e-3;
	float b = -5.775e-7;
	float rpoly;

	data=MAX31865_Read(0x07);
//	MAX31865_Write(0x80, 0xD3);
	data=MAX31865_Read(0x01)<<8;
	data|=MAX31865_Read(0x02);
	if(data & 0x0001)
	{
		*out = 0;
		return MAX_ERR_fault;
	}
	data>>=1;  //去掉Fault位
//	printf("Read=0x%02X\r\n",data);
	Rt=(float)data/32768.0*RREF;
	
//	printf("Rt=0x%.1f\r\n",Rt);
	
	Z1 = -a;
  Z2 = a*a-4*b;
  Z3 = 4*b/Rt0;
  Z4 = 2*b;

  temp = Z2+Z3*Rt;
  temp = (sqrt(temp)+Z1)/Z4;
  
  if(temp>=0)
	{
		return *out = temp;
	}
  rpoly = Rt;
  temp = -242.02;
  temp += 2.2228 * rpoly;
  rpoly *= Rt;  // square
  temp += 2.5859e-3 * rpoly;
  rpoly *= Rt;  // ^3
  temp -= 4.8260e-6 * rpoly;
  rpoly *= Rt;  // ^4
  temp -= 2.8183e-8 * rpoly;
  rpoly *= Rt;  // ^5
  temp += 1.5243e-10 * rpoly;

	*out = temp;
  return MAX_ERR_ok;
}



