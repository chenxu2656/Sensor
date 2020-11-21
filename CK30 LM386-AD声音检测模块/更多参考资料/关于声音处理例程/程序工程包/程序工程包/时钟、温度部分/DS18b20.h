#ifndef __ds18B20_H__
#define __ds18B20_H__
sbit DQ = P1^4;        //DS18B20的数据口位P1.4

void DelayUs2x(unsigned char t)
{   
 while(--t);
}


void DelayMs(unsigned char t)
{
     
 while(t--)//大致延时1mS
 {	 
     //DelayUs2x(245);
	 //DelayUs2x(245);
	 display();
	 display();//代替延时，顺便扫描
	 display();
	 display();
 }
}


bit Init_DS18B20(void)
{
 bit dat=0;
 DQ = 1;    //DQ复位
 DelayUs2x(10);   //稍做延时
 DQ = 0;         //单片机将DQ拉低
 //DelayUs2x(200); //精确延时 大于 480us 小于960us
 display();//代替延时，顺便扫描
 display();//代替延时，顺便扫描
 display();
 display();
 display();
 //DelayUs2x(200);
 DQ = 1;        //拉高总线
 //DelayUs2x(40); //15~60us 后 接收60-240us的存在脉冲
 display();
 display();
 //DelayUs2x(40);
 dat=DQ;        //如果x=0则初始化成功, x=1则初始化失败
 //DelayUs2x(40); //稍作延时返回
 display();
 display();
 return dat;
}

/*------------------------------------------------
                    读取一个字节
------------------------------------------------*/
unsigned char ReadOneChar(void)
{
unsigned char i=0;
unsigned char dat = 0;
for (i=8;i>0;i--)
 {display();
  DQ = 0; // 给脉冲信号
  dat>>=1;
  DQ = 1; // 给脉冲信号
  if(DQ)
   dat|=0x80;
  //DelayUs2x(40);
  display();
  display();
 }
 return(dat);
}
/*------------------------------------------------
                    写入一个字节
------------------------------------------------*/
void WriteOneChar(unsigned char dat)
{
 unsigned char i=0;
 for (i=8; i>0; i--)
 {display();
  DQ = 0;
  DQ = dat&0x01;
  //DelayUs2x(40);
  display();
  display();
  DQ = 1;
  dat>>=1;
 }
 //DelayUs2x(40);
 display();
 display();
}

/*------------------------------------------------
                    读取温度
------------------------------------------------*/
unsigned int ReadTemperature(void)
{
unsigned char a=0;
unsigned int b=0;
unsigned int t=0;
Init_DS18B20();
WriteOneChar(0xCC); // 跳过读序号列号的操作
WriteOneChar(0x44); // 启动温度转换
DelayMs(40);
Init_DS18B20();
WriteOneChar(0xCC); //跳过读序号列号的操作 
WriteOneChar(0xBE); //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
a=ReadOneChar();   //低位
b=ReadOneChar();   //高位
b<<=8;
t=a+b;display();
return(t);
}

#endif
