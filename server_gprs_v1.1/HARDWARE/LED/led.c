//////////////////////////////////////////////////////////////////////////////////	           
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//SIM系列开发板		   
//全球鹰电子@UNV
//修改日期:2016/8/18
//版本：V1.0
//官方淘宝店地址：https://shop110330041.taobao.com
//版权所有，盗版必究。
//All rights reserved
///////////////////////////////////////////////////////////////////
#include "led.h"

/***************  配置LED用到的I/O口 *******************/
void LED_GPIO_Config(void)	
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); // 使能PC端口时钟  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PC端口
  GPIO_SetBits(GPIOA, GPIO_Pin_11);	 // 关闭所有LED
}



