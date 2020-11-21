#include<reg51.h>
#include "INTRINS.H"
#include"timer.h"
#include"DS18b20.h"
main()
{   TMOD=0x11;//定时器初始化
	TH0=0xfe;
	TL0=0x70;
    TH1=0xfc;
	TL1=0x18;
	ET0=1;
	ET1=1;
	TR0=1;
	TR1=1;
    EA=1;
	AM=0;//确保闹铃有效
	AMN=0;//
while(1)
   {     
           if(x==0)     
              {
			   TempH=temp>>4;//右移四位
		       ih=(TempH%100)/10; //十位温度
               il=(TempH%100)%10; //个位温度
			  }
   }		  
}
   


void timer0(void) interrupt 1 using 0 //定时器0中断200微妙扫描一次
{ 	
    TH0=0xfe;
	TL0=0x70;
	display();
    x++;
    if(x==5000)x=0,l++,e+=8;//沙漏每秒下落一行，温度每秒扫描一次
    if(e>24)e=0;//沙漏复位
    if(l==60)l=0,h++;//分加  
    display();//扫描屏
	if(l==10)temp=ReadTemperature();//一秒读取一次温度
	display();	 	 
}  


void timer1(void) interrupt 3 using 1 //定时器1中断500微妙扫描一次
{ 
   TH1=0xfc;
   TL1=0x18;
   if(CS==0)  //如果检测到低电平，说明按键按下
    {
	 delay500us(); //延时去抖
     if(!CS)     //再次确认按键是否按下，没有按下则退出
	   {
        while(!CS);//如果确认按下按键等待按键释放，没有释放则一直等待
	       {
	         if(w==0)d++;
			 else n++;
	 		}
	   }
	}
   
  
   if(CF==0)
   	{
	 delay500us(); //延时去抖
     if(!CF)     //再次确认按键是否按下，没有按下则退出
	   {
        while(!CF);//如果确认按下按键等待按键释放，没有释放则一直等待
	       {
	         if(w==0)h++;
			 else p++;
	 		}
	   }
	}

   if((m==c)&&(n==d)&&(o==f)&&(p==h)&&(v==g)&&(l==0)){AM=1,delay(),AM=0;}//闹铃开始控制
   if((m==c)&&(n==d)&&(o==f)&&(p==h)&&(v==g)&&(l==59)){AMN=1,delay(),AMN=0;}//闹铃关闭控制   
   
   if(SW==0)  //如果检测到低电平，说明按键按下
    {
	 delay500us(); //延时去抖
     if(!SW)     //再次确认按键是否按下，没有按下则退出
	   {
        while(!SW);//如果确认按下按键等待按键释放，没有释放则一直等待
	       {
	         w=~w;
	 		}
	   }
	}

 	
}  

