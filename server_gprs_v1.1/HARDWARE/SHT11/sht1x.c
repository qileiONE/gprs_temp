
#define __SHT1X_C

#include "sht1x.h"
#include <math.h>

#define		DelayCnt		500

void SHT10_ConReset(void);

/*************************************************************
  Function   :SHT10_Dly  
  Description:SHT10时序延时
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_Dly(void)
{
        u16 i;
        for(i = DelayCnt; i > 0; i--);
}


/*************************************************************
  Function   :SHT10_Config  
  Description:初始化 SHT10 引脚
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_Config(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;        
        //初始化SHT10时钟
        RCC_APB2PeriphClockCmd(SHT10_AHB2_CLK ,ENABLE);
                
        //PD0 DATA 推挽  
        GPIO_InitStructure.GPIO_Pin = SHT10_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//GPIO_Mode_Out_OD;GPIO_Mode_Out_PP
        GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
        //PD1 SCK 推挽 
        GPIO_InitStructure.GPIO_Pin = SHT10_SCK_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(SHT10_SCK_PORT, &GPIO_InitStructure);

        SHT10_ConReset();        //复位通讯
}


/*************************************************************
  Function   :SHT10_DATAOut
  Description:设置输出引脚DATA
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_DATAOut(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        //PD0 DATA 推挽输出      
        GPIO_InitStructure.GPIO_Pin = SHT10_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//GPIO_Mode_Out_OD; GPIO_Mode_Out_PP        
        GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
}


/*************************************************************
  Function   :SHT10_DATAIn  
  Description:设置输入DATA
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_DATAIn(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        //PD0 DATA 浮空输入       
        GPIO_InitStructure.GPIO_Pin = SHT10_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//GPIO_Mode_Out_OD;GPIO_Mode_IN_FLOATING
        GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
}


/*************************************************************
  Function   :SHT10_WriteByte  
  Description:写1字幕
  Input      : value:要写入的字节 
  return     : err: 0-ok  1-fault    
*************************************************************/
u8 SHT10_WriteByte(u8 value)
{
        u8 i, err = 0;
				u16	count;
        
				SHT10_SCK_L();
        SHT10_Dly();
        SHT10_DATAOut();                         

        for(i = 0x80; i > 0; i =i>> 1) 
        {
                if(i & value)
                    SHT10_DATA_H();
                else
                    SHT10_DATA_L();
                SHT10_Dly();
                SHT10_SCK_H();
                SHT10_Dly();
                SHT10_SCK_L();
                SHT10_Dly();
        }
        SHT10_DATAIn(); 
				SHT10_DATA_H();				
				SHT10_Dly();
        SHT10_SCK_H();
				for(count=0;count<1000;count++)
				{
					SHT10_Dly();
					err = SHT10_DATA_R(); 
					if(err == 0)
						break;
				}					
        SHT10_SCK_L();
				
        return err;
}

/*************************************************************
  Function   :SHT10_ReadByte  
  Description:读1字节
  Input      : Ack: 0-不应答  1-应答       
  return     : err: 0-ok 1-fault   
*************************************************************/
u8 SHT10_ReadByte(u8 Ack)
{
        u8 i, val = 0;

				SHT10_SCK_L();
        SHT10_Dly();
        SHT10_DATAIn();                               
        for(i = 0x80; i > 0; i = i>>1) 
        {
						SHT10_Dly();
						SHT10_SCK_H();
						SHT10_Dly();
						if(SHT10_DATA_R())
								val = (val | i);
						SHT10_SCK_L();
        }
        SHT10_DATAOut();                        
        if(Ack)
                SHT10_DATA_L();                       
        else
                SHT10_DATA_H();                 
        SHT10_Dly();
        SHT10_SCK_H();
        SHT10_Dly();
        SHT10_SCK_L();
        SHT10_Dly();
				SHT10_DATA_H();

        return val;                                     
}


/*************************************************************
  Function   :SHT10_TransStart  
  Description:讯动传输:
                     _____         ________
               DATA:      |_______|
                         ___     ___
               SCK : ___|   |___|   |______        
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_TransStart(void)
{
                              
		SHT10_SCK_L();
		SHT10_Dly();
		SHT10_DATAOut(); 
		SHT10_DATA_H();        
		SHT10_Dly();
		SHT10_SCK_H();
		SHT10_Dly();
		SHT10_DATA_L();
		SHT10_Dly();
		SHT10_SCK_L();
		SHT10_Dly();
		SHT10_SCK_H();
		SHT10_Dly();
		SHT10_DATA_H();
		SHT10_Dly();
		SHT10_SCK_L();
		SHT10_Dly();
		//SHT10_DATA_L();

}


/*************************************************************
  Function   :SHT10_ConReset  
  Description:通讯复位:
                     _____________________________________________________         ________
               DATA:                                                      |_______|
                        _    _    _    _    _    _    _    _    _        ___     ___
               SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_ConReset(void)
{
        u8 i;

        SHT10_DATAOut();

        SHT10_DATA_H();
        SHT10_SCK_L();
				SHT10_Dly();
        for(i = 0; i < 9; i++)              
        {
                SHT10_SCK_H();
                SHT10_Dly();
                SHT10_SCK_L();
                SHT10_Dly();
        }
        SHT10_TransStart();                       
}



/*************************************************************
  Function   :SHT10_SoftReset  
  Description:软复位
  Input      : none        
  return     : err: 0-ok  1-err  
*************************************************************/
u8 SHT10_SoftReset(void)
{
        u8 err = 0;

        SHT10_ConReset();                           
        err += SHT10_WriteByte(SOFTRESET);

        return err;
}


/*************************************************************
  Function   :SHT10_ReadStatusReg  
  Description:读状态寄存器
  Input      : p_value-读到的数据;p_checksun-读到的校验数据     
  return     : err: 0-ok  0-er  
*************************************************************/
u8 SHT10_ReadStatusReg(u8 *p_value, u8 *p_checksum)
{
        u8 err = 0;

        SHT10_TransStart();                                 
        err = SHT10_WriteByte(STATUS_REG_R);
        *p_value = SHT10_ReadByte(ACK);           
        *p_checksum = SHT10_ReadByte(noACK);
        
        return err;
}



/*************************************************************
  Function   :SHT10_WriteStatusReg  
  Description:写状态寄存器
  Input      : p_value-要定入的数据     
  return     : err: 0-ok  1-err    
*************************************************************/
u8 SHT10_WriteStatusReg(u8 *p_value)
{
        u8 err = 0;

        SHT10_TransStart();                              
        err += SHT10_WriteByte(STATUS_REG_W);
        err += SHT10_WriteByte(*p_value);        

        return err;
}



/*************************************************************
  Function   :SHT10_Measure  
  Description:从温度传感器读取的温湿度
  Input      : p_value-读到的值;p_checksum-读到的校验        
  return     : err: 0-ok 1err   
*************************************************************/
u8 SHT10_Measure(u16 *p_value, u8 *p_checksum, u8 mode)
{
        u8 err = 0;
        u32 i;
        u8 value_H = 0;
        u8 value_L = 0;

        SHT10_TransStart();                                               
        switch(mode)                                                         
        {
        case TEMP:                                                               
                err += SHT10_WriteByte(MEASURE_TEMP);
                break;
        case HUMI:
                err += SHT10_WriteByte(MEASURE_HUMI);
                break;
        default:
                break;
        }
        SHT10_DATAIn();
        for(i = 0; i < 2000000; i++)                          
        {
                if(SHT10_DATA_R() == 0) break;          
        }
        if(SHT10_DATA_R() == 1)                              
                err += 1;
        value_H = SHT10_ReadByte(ACK);
        value_L = SHT10_ReadByte(ACK);
        *p_checksum = SHT10_ReadByte(noACK);         
        *p_value = (value_H << 8) | value_L;
        return err;
}




/*************************************************************
  Function   :SHT10_CalcuDewPoint  
  Description:计算露点
  Input      : h-实际湿度;t-实际温度       
  return     : dew_point-露点    
*************************************************************/
float SHT10_CalcuDewPoint(float t, float h)
{
        float logEx, dew_point;

        logEx = 0.66077 + 7.5 * t / (237.3 + t) + (log10(h) - 2);
        dew_point = ((0.66077 - logEx) * 237.3) / (logEx - 8.16077);

        return dew_point; 
}

u8	getSHT1xTemp(float *out)
{
			u8 err = 0, checksum = 0;
			u16 temp_val;
			const float d1 = -39.7;
      const float d2 = +0.01;
	
			err = SHT10_Measure(&temp_val, &checksum, TEMP);
			if(err != 0)
			{
					//SHT10_SoftReset();
					*out = 0;
					return SHT1X_ERR_FAULT;
			}
			*out = d1 + d2 * temp_val;
			return SHT1X_ERR_OK;
}

u8	getSHT1xHumi(float *out)
{
			u8 err = 0, checksum = 0;
			u16 humi_val;
			const float C1 = -2.0468;
      const float C2 = +0.0367;
      const float C3 = -0.0000015955; 
	
			err = SHT10_Measure(&humi_val, &checksum, HUMI);
			if(err != 0)
			{
					SHT10_SoftReset();
					*out = 0;
					return SHT1X_ERR_FAULT;
			}
			*out = C1 + C2 * humi_val + C3 * humi_val * humi_val;
			return SHT1X_ERR_OK;
}

//int main(void)
//{  
//        u16 humi_val, temp_val;
//        u8 err = 0, checksum = 0;
//        float humi_val_real = 0.0; 
//        float temp_val_real = 0.0;
//        float dew_point = 0.0;
//        
//        BSP_Init();
//        printf("\nSHT10???????!!!\n");
//        SHT10_Config();
//        while(1)
//        {
//                err += SHT10_Measure(&temp_val, &checksum, TEMP);                  //???????
//                err += SHT10_Measure(&humi_val, &checksum, HUMI);                  //???????
//                if(err != 0)
//                        SHT10_ConReset();
//                else
//                {
//                        SHT10_Calculate(temp_val, humi_val, &temp_val_real, &humi_val_real); //?????????
//                        dew_point = SHT10_CalcuDewPoint(temp_val_real, humi_val_real);                 //??????
//                } 
//                printf("???????:%2.1f?,???:%2.1f%%,?????%2.1f?\r\n", temp_val_real, humi_val_real, dew_point);
//                LED1_Toggle();
//                Delay_ms(1000);
//        }
//}
