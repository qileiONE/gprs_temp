//////////////////////////////////////////////////////////////////////////////////	           
//All rights reserved
//******************************************************************************/
#include "sim900a.h"
#include "usart2.h"
#include "delay.h"
#include "sys.h"
#include "bmp.h"
#include "usart.h"
#include "timer.h"
#include "exti.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_dma.h"
#include "string.h"
#include "sht1x.h"
#include "max31865.h"
#include "led.h"

#define Device_Id "0001"

u8 MCU_SN[6] = {0};

void GetLockCode(void) 
{
    u32 stm32Id[3]={0};
	u32 stm32_id_data = 0;
    stm32Id[0]=*(vu32*)(0x1ffff7e8);
    stm32Id[1]=*(vu32*)(0x1ffff7ec);
    stm32Id[2]=*(vu32*)(0x1ffff7f0);
    //return (stm32Id[0]>>1)+(stm32Id[1]>>2)+(stm32Id[2]>>3);
	stm32_id_data = (stm32Id[0]>>1)+(stm32Id[1]>>2)+(stm32Id[2]>>3);
	MCU_SN[0] = 0x07;
	MCU_SN[1] = 0x24;
	MCU_SN[2] = stm32_id_data ; 
	MCU_SN[3] = stm32_id_data >> 8 ;
	MCU_SN[4] = stm32_id_data >> 16 ;
	MCU_SN[5] = stm32_id_data >> 24 ;
}


int main(void)
 {
	u8 res=1;
	float temp_probe;
	float temp_touch;
	int electric_quantity = 100;
	char message[150];  
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级 	LED_Init();			     //LED端口初始化
	USART2_Init(115200);	//初始化串口2 
	TIM2_NVIC_Configuration(); /* TIM2 定时配置 */
	TIM2_Configuration(); 		
//	Key_GPIO_Config();//按键端口初始化
	LED_GPIO_Config();
	MAX31865_Init();
	SHT10_Config();
	GetLockCode();
	EXTIX_Init();
	//BEEP_ON();delay_ms(1000);BEEP_OFF();	//提示开机完成
	 while(res)
	{
		res=sim900a_word_test();
		//OLED_Clear();		
		switch(res)
		{
			case 1:	//无通信失败
				//OLED_ShowString(0,0, (u8*)"communtion fail",24); 				 
				break;
			case 2:	//无SIM卡
				//OLED_ShowString(0,0, (u8*)"NO SIMCARD",24); 	
			  //OLED_ShowString(0,24,(u8*) "please insered the SIM card and repowered the module",12); 	
				break;
			case 3:	//等待注册到网络
				//OLED_ShowString(0,0, (u8*)"REGISTERING...",12); 	
				//OLED_ShowString(0,20, (u8*)"CSQ:",12); 
			  //OLED_ShowString(12*5,20, (u8*)SIM900_CSQ,12); 
				break;
			case 4:	//开启GPS失败
				//OLED_ShowString(0,0, (u8*)"open gps fail...",12); 
			default:
				break;				
		}	
    //BEEP_MODE(1);
	}	 //系统检测完毕
	//OLED_Clear();	
	//OLED_ShowString(0,0, (u8*)"MODE CONFIG...",24); 	
	res=1;
	while(res)
	{		
		res=SIM900A_CONNECT_SERVER((u8*)"47.103.68.81",(u8*)"12138");
		//BEEP_MODE(1);//		                          
	}
	//OLED_Clear();		
	//OLED_ShowString(0,0, (u8*)"SYS READY",24); 	
	//BEEP_MODE(0);	//初始化成功	
	delay_ms(1000);
	//while(sim808_open_gps());
	while(1)
	{
		if(Flag_Rec_TCP==1)	//收到服务器下发数据
		{
			Flag_Rec_TCP=0;
			res=SIM_HANDLE_TCP_CMD();
			if(res)
			{
				if(res==1)		
				{
					LED1(ON);
				}
				else if(res==2)	
				{
					LED1(OFF);	
				}
			}
		}

		delay_ms(500);
		sprintf((char*)message+1,",%02x:%02x:%02x:%02x:%02x:%02x",MCU_SN[0],MCU_SN[1],MCU_SN[2],MCU_SN[3],MCU_SN[4],MCU_SN[5]);
		res=SIM808_GPS_HANDLE((u8 *)message);
		MAX31865_GetTemp(&temp_probe);
		sprintf((char*)message+strlen((char *)message),"%6.2f",temp_probe);	
		getSHT1xTemp(&temp_touch);
		sprintf((char*)message+strlen((char *)message),",%6.2f",temp_touch);	
		sprintf((char*)message+strlen((char *)message),",%d",electric_quantity);	

		if((Flag_Send_data&0x8080))
		{
			if(Flag_Send_data&0x8000)	message[0]='0';
			//else if(Flag_Send_data&0x0800)	message[0]='1';
			else if(Flag_Send_data&0x0080)	message[0]='1';
			Flag_Send_data=0;
			START_TIME;
			res=SIM900A_GPRS_SEND_DATA((u8 *)message);
			switch(res)
			{
				case 0:
					//START_TIME;
				default:
					while(res)
					{		
						res=SIM900A_CONNECT_SERVER((u8*)"47.103.68.81",(u8*)"12138");	//123.166.16.208	
					}					
			}
		}
	}	
}

