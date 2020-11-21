/******************************************

功能：驱动AD0832读取模拟电压数值，将AD转换
	  后的数值，使用串口输出。ADC0832的CHO
	  （通道0）是检测电压变化。
	  实现串口输出，串口是单片机程序调试种
	  最常用最重要的工具。在使用前需要主要
	  开发板当前晶振频率是否为11.0592MHz,如
	  不是，请更换晶振，或者自行计算定时器
	  数值。
MCU:STC89C52
开发环境：KeilC

******************************************/


//头文件
#include <reg51.h>
#include <INTRINS.H>

//宏定义
#define uchar unsigned char 
#define uint unsigned int 

//管脚定义
sbit	ADC0832_CLK = P3^5;
sbit	ADC0832_DODI =P2^7;
sbit	ADC0832_CS = P3^4;

//函数声明
/******************************************/					
void delay(unsigned int time); 


/******************************************/
//延时子函数 
//参数范围 0-65536
void delay(unsigned int time)   //参数 time 大小
{							     //决定延时时间长短

	while(time--);
}

//------------------------------------------------ADC0832读取函数------------------------------------------------------
/************************************************************/
//读取ADC0832转换值函数
//入口函数：channel选择通道
uchar ADC0832_Read(uchar channel)
{
	uchar i = 0;	
	uchar tmp = 0;	//高字节数据
	uchar tmp1 = 0;	//低字节数据
	
	//开始标志位
	ADC0832_DODI = 1;
	_nop_();  _nop_();

	ADC0832_CS = 0;		//片选信号	

	ADC0832_CLK = 1;
	_nop_();  _nop_();
	ADC0832_CLK = 0;
	_nop_();  _nop_();

	//选择通道
	switch(channel)
	{
		case 0:		//通道0
			ADC0832_DODI = 1;	//SGL = 1；
			_nop_();  _nop_();
			ADC0832_CLK = 1;
			_nop_();  _nop_();
			ADC0832_CLK = 0;
			_nop_();  _nop_();

			ADC0832_DODI = 0;	//ODD = 0;
			_nop_();  _nop_();
 			ADC0832_CLK = 1;
 			_nop_();  _nop_();
			ADC0832_CLK = 0;
			_nop_();  _nop_();
			break;

		case 1:		//通道1
			ADC0832_DODI = 1;	//SGL = 1；
			_nop_();  _nop_();
			ADC0832_CLK = 1;
			_nop_();  _nop_();
			ADC0832_CLK = 0;
			_nop_();  _nop_();

			ADC0832_DODI = 1;	//ODD = 1;
			_nop_();  _nop_();
 			ADC0832_CLK = 1;
 			_nop_();  _nop_();
			ADC0832_CLK = 0;
			_nop_();  _nop_();
			break;
			
		default:
			break;		
	}

	ADC0832_DODI = 1;
	_nop_();  _nop_();

	//读取8位转换数值
	for(i=0;i<8;i++)
	{
		ADC0832_CLK = 1;	   //产生同步时钟
		_nop_();  _nop_();
		ADC0832_CLK = 0;
		_nop_();  _nop_();

		if(ADC0832_DODI)	  //按位获取8位数据
		{
			tmp |= 0x01;	  //从低位获取
		}
		else
		{
			tmp &= 0xfe;
		}
		if(i<7)
		{
			tmp <<= 1;		  //移位
		}					
	}

	tmp1 =  tmp & 0x01;
	tmp1 <<= 6;

	for(i=0;i<7;i++)		  //获取7位校验数据
	{
		ADC0832_CLK = 1;
		_nop_();  _nop_();
		ADC0832_CLK = 0;
		_nop_();  _nop_();

		if(ADC0832_DODI)
		{
			tmp1 |= 0x80;
		}
		else
		{
			tmp1 &= 0x7f;
		}
		if(i<6)
		{
			tmp1 >>= 1;
		}		
	}

	ADC0832_CS = 1;		//禁止ADC0832

	if(tmp == tmp1)	//当高字节与低字节相同时，该值有效
	{
		return tmp;	//返回数值
	}
}


//------------------------------------------------串口初始化------------------------------------------------------
void Uart_Init( void )
{  							//波特率9600
	TMOD = 0x20;	//定时器T1使用工作方式2
	TL1 = 0xfa;
	TH1 = 0xfa;
	TR1 = 1;    	// 开始计时
	SCON = 0x50;	//工作方式1，波特率9600bps @ 11.0592，允许接收
	PCON |= 0x80;
	//ES = 1;
	//EA = 1;    		// 打开所有中断
	TI = 0;
	//RI = 0;
	//REN = 1;
}
//------------------------------------------------串口显示----------------------------------------------------------
void RS_Byte(uchar R_Byte)
{	
	 SBUF = R_Byte;  
     while(TI == 0);				//查询法
  	 TI = 0;
     
}

void RS_String(uchar *p)
{
	uchar i = 0;

	while(*(p+i) != '\0' )
	{
		RS_Byte(*(p+i));
		i++;	
	}
}
					
/******************************************/
//主函数				
void main(void) 
{
	uchar AD_Value;	

	Uart_Init();

	RS_String("Uart Initializing...... \n");

	delay(50000);

	RS_String("Uart Initializing Finished !! \n");

	delay(50000);

	while(1)			//主程序循环 	
	{
		AD_Value = ADC0832_Read(0);

		RS_String("The value of AD is : ");
		RS_Byte(AD_Value/100 + '0');
		RS_Byte(AD_Value/10%10 + '0');
		RS_Byte(AD_Value%10 + '0');
		RS_Byte('\n');

		delay(50000);
	}
}

