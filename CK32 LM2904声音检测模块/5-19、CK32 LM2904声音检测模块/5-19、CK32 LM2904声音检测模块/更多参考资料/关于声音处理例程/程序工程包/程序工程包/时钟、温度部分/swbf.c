#include<reg51.h>
#include "INTRINS.H"
#include"timer.h"
#include"DS18b20.h"
main()
{   TMOD=0x11;//��ʱ����ʼ��
	TH0=0xfe;
	TL0=0x70;
    TH1=0xfc;
	TL1=0x18;
	ET0=1;
	ET1=1;
	TR0=1;
	TR1=1;
    EA=1;
	AM=0;//ȷ��������Ч
	AMN=0;//
while(1)
   {     
           if(x==0)     
              {
			   TempH=temp>>4;//������λ
		       ih=(TempH%100)/10; //ʮλ�¶�
               il=(TempH%100)%10; //��λ�¶�
			  }
   }		  
}
   


void timer0(void) interrupt 1 using 0 //��ʱ��0�ж�200΢��ɨ��һ��
{ 	
    TH0=0xfe;
	TL0=0x70;
	display();
    x++;
    if(x==5000)x=0,l++,e+=8;//ɳ©ÿ������һ�У��¶�ÿ��ɨ��һ��
    if(e>24)e=0;//ɳ©��λ
    if(l==60)l=0,h++;//�ּ�  
    display();//ɨ����
	if(l==10)temp=ReadTemperature();//һ���ȡһ���¶�
	display();	 	 
}  


void timer1(void) interrupt 3 using 1 //��ʱ��1�ж�500΢��ɨ��һ��
{ 
   TH1=0xfc;
   TL1=0x18;
   if(CS==0)  //�����⵽�͵�ƽ��˵����������
    {
	 delay500us(); //��ʱȥ��
     if(!CS)     //�ٴ�ȷ�ϰ����Ƿ��£�û�а������˳�
	   {
        while(!CS);//���ȷ�ϰ��°����ȴ������ͷţ�û���ͷ���һֱ�ȴ�
	       {
	         if(w==0)d++;
			 else n++;
	 		}
	   }
	}
   
  
   if(CF==0)
   	{
	 delay500us(); //��ʱȥ��
     if(!CF)     //�ٴ�ȷ�ϰ����Ƿ��£�û�а������˳�
	   {
        while(!CF);//���ȷ�ϰ��°����ȴ������ͷţ�û���ͷ���һֱ�ȴ�
	       {
	         if(w==0)h++;
			 else p++;
	 		}
	   }
	}

   if((m==c)&&(n==d)&&(o==f)&&(p==h)&&(v==g)&&(l==0)){AM=1,delay(),AM=0;}//���忪ʼ����
   if((m==c)&&(n==d)&&(o==f)&&(p==h)&&(v==g)&&(l==59)){AMN=1,delay(),AMN=0;}//����رտ���   
   
   if(SW==0)  //�����⵽�͵�ƽ��˵����������
    {
	 delay500us(); //��ʱȥ��
     if(!SW)     //�ٴ�ȷ�ϰ����Ƿ��£�û�а������˳�
	   {
        while(!SW);//���ȷ�ϰ��°����ȴ������ͷţ�û���ͷ���һֱ�ȴ�
	       {
	         w=~w;
	 		}
	   }
	}

 	
}  

