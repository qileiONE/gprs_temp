/****************************************Copyright (c)**************************************************
**                                Copyright  (C)  W.ch (2004--2010)
**                                Web:  http://www.winchiphead.com 
**                                                                                       
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: fram.c																							
**创   建   人: fqb
**最后修改日期: 2009年09月09日
**描        述: 
**版        本：v1.0              
**工 程 说  明: FM24C64操作
**--------------------------------------------------------------------------------------------------------
*/
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>



#include  "fram.h"

#define u8 unsigned char
#define u16 unsigned short

u8 ack;	         /*应答标志位*/
u8 iic_speed;   


void Fram_SDA_OUT( void )
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

void Fram_SDA_IN( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
}


void GPIO_INIT_Fram( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	FRAM_CLK_SET( );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);		
	FRAM_CLK_SET();
	FRAM_SDA_SET();		
	Fram_SDA_OUT();		
}

void mDelayuS1( u8 cnt )
{
	unsigned char	i;

	while( cnt -- )
	{
		i = 2;
		while( i -- )
			__nop( );
	}
}

/*******************************************************************
                     起动总线函数               
函数原型: void  Start_I2c();  
功能:       启动I2C总线,即发送I2C起始条件.
  
********************************************************************/
void Start_I2c()
{
  	Fram_SDA_OUT( );	//方向设置为输出
	mDelayuS1( 1 );
	FRAM_SDA_SET( );   /*发送起始条件的数据信号，SDA = 1*/
	mDelayuS1( 1 );
  	FRAM_CLK_SET( );	/*scl = 1*/
	mDelayuS1( 1 );
  	FRAM_SDA_CLR( );   /*发送起始信号sda = 0 */
	mDelayuS1( 1 );
  	FRAM_CLK_CLR( );   /*钳住I2C总线，准备发送或接收数据 scl = 0*/
	mDelayuS1( 1 );
//	Fram_SDA_IN();	  //方向为输入
}


/*******************************************************************
                      结束总线函数               
函数原型: void  Stop_I2c();  
功能:       结束I2C总线,即发送I2C结束条件.
  
********************************************************************/
void Stop_I2c()
{
  	Fram_SDA_OUT( );	//方向设置为输出
	mDelayuS1( 1 );
	FRAM_CLK_CLR( );	//scl = 0
	mDelayuS1( 1 );
	FRAM_SDA_CLR( );  /*发送结束条件的数据信号sda = 0*/
	mDelayuS1( 1 );
  	FRAM_CLK_SET( );  //scl = 1
	mDelayuS1( 1 );  	
  	FRAM_SDA_SET( );  /*发送I2C总线结束信号 sda = 1*/
	mDelayuS1( 1 );  
//	Fram_SDA_IN();	  //方向为输入
}



/*******************************************************************
                 字节数据传送函数               
函数原型: void  SendByte(uchar c);
功能:  将数据c发送出去,可以是地址,也可以是数据,发完后等待应答,并对
     此状态位进行操作.(不应答或非应答都使ack=0 假)     
     发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
********************************************************************/
void  SendByte(u8 c)
{
	u8 BitCnt;
  	Fram_SDA_OUT( );	//方向设置为输出
 	for(BitCnt=0;BitCnt<8;BitCnt++)  /*要传送的数据长度为8位*/
    {
//		FRAM_CLK_CLR( );	//scl = 0
		if( c&0x80 ){FRAM_SDA_SET();} //sda = 1
     	else{  FRAM_SDA_CLR(); }      //sda = 0         
     	FRAM_CLK_CLR( );               /*scl = 1置时钟线为高，通知被控器开始接收数据位*/
		mDelayuS1( 1 );
     	FRAM_CLK_SET( ); 			   //scl = 0
		mDelayuS1( 1 );
     	FRAM_CLK_CLR( );               /*scl = 1置时钟线为高，通知被控器开始接收数据位*/
		mDelayuS1( 1 );
		c <<= 1;
	}
//    FRAM_SDA_SET( );               /*sda = 1 8位发送完后释放数据线，准备接收应答位*/
	Fram_SDA_IN( );	  	//方向设置为输入
	mDelayuS1( 1 );
    FRAM_CLK_SET( );			   //scl  = 1
	mDelayuS1( 1 );
    if(FRAM_SDA( ))ack=0;  //输入 
    else ack=1;        /*判断是否接收到应答信号*/
	mDelayuS1( 1 );
    FRAM_CLK_CLR( );			   //scl  = 0
}


/*******************************************************************
                 字节数据传送函数               
函数原型: uchar  RcvByte();
功能:  用来接收从器件传来的数据,并判断总线错误(不发应答信号)，
     发完后请用应答函数。  
********************************************************************/	
u8  RcvByte( )
{
 	u8 retc;
  	u8 BitCnt;
  	Fram_SDA_OUT( );	//方向设置为输出
  
  	retc=0; 
  	FRAM_SDA_SET( );             /*sda = 1置数据线为输入方式*/
	Fram_SDA_IN( );	  	//方向设置为输入
  	for(BitCnt=8;BitCnt!=0;BitCnt--)
    {
		mDelayuS1( 1 );
        FRAM_CLK_CLR( );       /*scl = 0置时钟线为低，准备接收数据位*/
		mDelayuS1( 1 );
        FRAM_CLK_SET( );       /*scl = 1置时钟线为高使数据线上数据有效*/
		mDelayuS1( 1 );
        retc=retc<<1;
        if(FRAM_SDA( ))retc++; /*读数据位,接收的数据位放入retc中 */
    }
  	FRAM_CLK_CLR(); 		//scl = 0   
	mDelayuS1( 1 );
	Fram_SDA_IN();		//方向为输入
  	return(retc);
}


/********************************************************************
                     应答子函数
原型:  void Ack_I2c(bit a);
 
功能:主控器进行应答信号,(可以是应答或非应答信号)
********************************************************************/
void Ack_I2c(unsigned char a)
{
  	Fram_SDA_OUT( );	//方向设置为输出
	if( a ) {FRAM_SDA_SET();}
	else {FRAM_SDA_CLR(); } 
	mDelayuS1( 1 );
  	FRAM_CLK_SET( );
	mDelayuS1( 1 );
 	FRAM_CLK_CLR( );                /*清时钟线，钳住I2C总线以便继续接收*/
	mDelayuS1( 1 );
//	Fram_SDA_IN( );		//方向为输入
}


/*******************************************************************
                    向有子地址器件发送多字节数据函数               
函数原型: bit  ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
功能:     从启动总线到发送地址，子地址,数据，结束总线的全过程,从器件
          地址sla，子地址suba，发送内容是s指向的内容，发送no个字节。
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
********************************************************************/
u8 write_str_sub(u8 sla,u16 suba,u8 *s,u16 no)
{
    u16 i;
	Start_I2c();               /*启动总线*/
	SendByte(sla);            /*发送器件地址*/
	if( ack == 0 )return 0;
	SendByte(suba>>8);
	if(ack==0)return(0);	
	SendByte(suba);            /*发送器件子地址*/
   	if(ack==0)return(0);
    for(i=0;i<no;i++)
  	{	   
       SendByte(*s);               /*发送数据*/
       if(ack==0)return(0);
       s++;
    } 
 	Stop_I2c();                 /*结束总线*/ 
  	return(1);
}



/*******************************************************************
                    向有子地址器件读取多字节数据函数               
函数原型: bit  ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
功能:     从启动总线到发送地址，子地址,读数据，结束总线的全过程,从器件
          地址sla，子地址suba，读出的内容放入s指向的存储区，读no个字节。
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
********************************************************************/
u8 read_str_sub(u8 sla,u16 suba,u8 *s,u16 no)
{
   	u16 i;
	Start_I2c();               /*启动总线*/
   	SendByte(sla);			//读，器件地址加 1
    if(ack==0)return(0);

	SendByte(suba>>8);
	SendByte(suba);            /*发送器件子地址*/
    if(ack==0)return(0);

   	Start_I2c();
   	SendByte(sla+1);			//读，器件地址加 1
    if(ack==0)return(0);
   	for(i=0;i<no-1;i++)
    {   
     	*s=RcvByte();               /*发送数据*/
      	Ack_I2c(0);                /*发送就答位*/  
     	s++;
    } 
   	*s=RcvByte();
    Ack_I2c(1);                 /*发送非应位*/
 	Stop_I2c();                    /*结束总线*/ 
  	return(1);
}

u8 Fram_Read_Data( u16 suba,u8 *s,u16 no )
{
	return read_str_sub(FRAM_ADDR_CMD,suba,s,no);
}

u8 Fram_Write_Data( u16 suba,u8 *s,u16 no )
{
	return write_str_sub(FRAM_ADDR_CMD,suba,s,no);
}

/*
	写标志位数据，地址为8192-512=7680=0x1e00
	addr：表示在7.5K的地址空间的位置
	len： 表示在7.5K的有效数据长度
*/
void write_bit( u16 addr,u16 len )
{
	u8 buf[8];
	buf[0] = addr>>8;buf[1] = addr;
	buf[2] = len>>8;buf[3] = len;
	Fram_Write_Data( 0x1e00,buf,4 );
}

/*
	读标志位数据，地址为8192-512=7680=0x1e00
	addr：表示在7.5K的地址空间的位置
	len： 表示在7.5K的有效数据长度
*/
u16 read_bit( u16 addr )
{
   u8 buf[8];
   u16 len;
	Fram_Read_Data( 0x1e00,buf,4 );
	addr = (u16)buf[0]<<8;
	addr |= buf[1];
	len = (u16)buf[2]<<8;
	len |= buf[3];

printf("len=%02x\n",len);
printf("addr=%02x\n",addr);
	return len;

}


/*
	写数据至FRAM里面
	返回数据：计算后的数据地址
*/
#define  FRAM_SIZE	0x1e00	
u16 write_data( u16 addr,u16 len,u8 *pbuf )
{
	u16 back_addr,i;
//	u8 test_buf[100];
	if( addr+len < FRAM_SIZE ){
		Fram_Write_Data( addr,pbuf,len);
		addr+=len;
	}
	else if( addr+len == FRAM_SIZE ){
		Fram_Write_Data( addr,pbuf,len );
		addr = 0;	
	}
	else if( addr+len > FRAM_SIZE ){
		back_addr = addr+len-FRAM_SIZE;
		i = FRAM_SIZE-addr;
		Fram_Write_Data( addr,pbuf,i );
		addr = 0;pbuf+=i;
		Fram_Write_Data( addr,pbuf,back_addr );
//		Fram_Read_Data( addr,test_buf,i );
//		Fram_Read_Data( 0,&test_buf[i],back_addr );
//for( i=0;i!=100;i++ )printf("%02x ",test_buf[i]);

		addr = back_addr;
		

	}
	return addr;	
}


/*
	读取数据至RAM里面
*/
void read_data( u16 addr,u16 len,u8 *pbuf )
{
	u16 back_addr,i;
	if( addr+len <= FRAM_SIZE ){
		Fram_Read_Data( addr,pbuf,len );	
	}
	else if( addr+len > FRAM_SIZE ){
		back_addr = addr+len-FRAM_SIZE;
		i = FRAM_SIZE-addr;
		Fram_Read_Data( addr,pbuf,i );
		addr = 0;pbuf+=i;
		Fram_Read_Data( addr,pbuf,back_addr );	
	}
}

