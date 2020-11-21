#ifndef __timer_H__
#define __timer_H__
typedef unsigned char uint8;
typedef unsigned int  uint16;
sbit YS=P1^1;//74HC154组切换
sbit CS=P1^5;//小时调整
sbit CF=P1^6;//分调整
sbit SW=P1^7;//时钟、闹铃切换
sbit AM=P1^2;//闹铃位
sbit AMN=P1^3;
code uint8 DMSG[3]={0x10,0x90,0xd0};//小时十位数组
code uint8 DMSS[10]={0x00,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff,0xff};//小时个位数组
code uint8 DMFG[6]={0x02,0x82,0xc2,0xe2,0xf2,0xfa};//分钟十位数组
code uint8 DMFS1[10]={0x00,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff,0xff};//分钟个位数组
code uint8 DMFS2[2]={0x20,0xa0};//分钟个位数组大于8部分
code uint8 DMX[14]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x1e,0x3e,0x7c,0x3e,0x1e,0x0c};//心形图案数组
code uint8 DMW[8]={0x00,0x00,0x00,0x00,0x00,0x08,0x0E,0x0F};//闹铃图案数组
code uint8 LED[32]={0x81,0x43,0x27,0x1F,0x1F,0x27,0x43,0x81,//沙漏图案数组
                    0x81,0xC2,0xA6,0x9E,0x9E,0xA6,0xC2,0x81,
                    0x81,0xC2,0xE4,0xDC,0xDC,0xE4,0xC2,0x81,
                    0x81,0xC2,0xE4,0xF8,0xF8,0xE4,0xC2,0x81};
code uint8 TEPM[10]={0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff,0xff,0xff};//温度数组
code uint8 BZ[6]={0x88,0x9c,0x88,0x80,0x80,0x80};
uint8 a,b=7,c=1,d=2,e=0,f=0,g=0,h=0,ih=0,il=0,j=0,l=0,m=1,n=2,o=3,p=0,q,r,s,t,u,v=0,w=0,y=0;//各变量
uint16 x=0,temp,TempH,TempL;// 

void delay(void)//100微秒延时
{ uint8 a,b;
    for(b=1;b>0;b--)
        for(a=47;a>0;a--);

}


void delay500us(void)   //误差 0us
{
    unsigned char a,b;
    for(b=71;b>0;b--)
        for(a=2;a>0;a--);
}



void time(void)//时间进制控制
{    
     if(h==9)g=1;
	 if(h>9)g=0,h=0,f++;
	 if(f>5)f=0,d++;
	 if((c<2)&&(d==10)){d=0,c++;}
	 if((c>1)&&(d==4))d=0,c=0;
}

void Wtime(void)//闹铃进制控制
{    
     if(p==9)v=1;
	 if(p>9)v=0,p=0,o++;
	 if(o>5)o=0,n++;
	 if((m<2)&&(n==10)){n=0,m++;}
	 if((m>1)&&(n==4))n=0,m=0;
}

void display(void)//显示函数
{  
   if(w==0)//时钟闹铃切换控制
        {q=c,r=d,s=f,t=h,u=g,a=0;}
   else 
        {q=m,r=n,s=o,t=p,u=v,a=5;}
  switch(j)//32列LED动态扫描
    {
     case 0:P0=0x00,P3=0x00,YS=1,P2=0x00,delay(); break;
     case 1:P0=DMW[a+0],P3=DMSG[q],P2=0x01,delay();break;
     case 2:P0=DMW[a+1],P3=DMSG[q],P2=0x02,delay();break;
	 case 3:P0=DMW[a+2],P3=0x00,P2=0x03,delay();break;
	 case 4:if(a==0)if(r==9)P0=0xa0;else P0=0x20;if(a==5)if(r==9)P0=0xae;else P0=0x2e;P3=DMSS[r],P2=0x04,delay();break;
	 case 5:if(a==0)if(r==9)P0=0xa0;else P0=0x20;if(a==5)if(r==9)P0=0xa8;else P0=0x28;P3=DMSS[r],P2=0x05,delay();break;
	 case 6:P0=0x00,P3=0x00,P2=0x06,delay();break;
	 case 7:P0=DMX[b+0],P3=0x00,P2=0x07,delay();break;
	 case 8:P0=DMX[b+1];P3=0x60,P2=0x08,delay();break;
	 case 9:P0=DMX[b+2];P3=0x60,P2=0x09,delay();break;
	 case 10:P0=DMX[b+3],P3=0x00,P2=0x0a,delay();break;
	 case 11:P0=DMX[b+4],P3=0x00,P2=0x0b,delay();break;
	 case 12:P0=DMX[b+5],P3=DMFG[s],P2=0x0c,delay();break;
     case 13:P0=DMX[b+6],P3=DMFG[s],P2=0x0d,delay();break;
	 case 14:P0=0x00,P3=0x00,P2=0x0e,delay();break;
	 case 15:P0=DMFS2[u],P3=DMFS1[t],P2=0x0f,delay();break;
     case 16:P0=DMFS2[u],P3=DMFS1[t],YS=0,P2=0x00,delay();break;
	 case 17:P0=0x00,P3=0x00,P2=0x10,delay();break;
	 case 18:P0=0x00,P3=0x00,P2=0x20,delay();break;
	 case 19:P0=0x8e,P3=0x00,P2=0x30,delay();break;
	 case 20:P0=0x8a,P3=0x00,P2=0x40,delay();break;
	 case 21:P0=0x81,P3=0xff,P2=0x50,delay();break;
	 case 22:P0=BZ[y+0],P3=0x00,P2=0x60,delay();break;
	 case 23:P0=BZ[y+1],P3=0x00,P2=0x70,delay();break;
	 case 24:P0=BZ[y+2],P3=LED[e],P2=0x80,delay();break;
	 case 25:P0=0x80,P3=LED[e+1],P2=0x90,delay();break;
	 case 26:P0=TEPM[ih],P3=LED[e+2],P2=0xa0,delay();break;
	 case 27:P0=TEPM[ih],P3=LED[e+3],P2=0xb0,delay();break;
	 case 28:P0=0x80,P3=LED[e+4],P2=0xc0,delay();break;
	 case 29:P0=0x80,P3=LED[e+5],P2=0xd0,delay();break;
	 case 30:P0=TEPM[il],P3=LED[e+6],P2=0xe0,delay();break;
	 case 31:P0=TEPM[il],P3=LED[e+7],P2=0xf0,delay();break;
	 default:break;
	 }
   j++;if(j==32)j=0;//扫描循环控制
   if(w==0){time();}//时间进制控制
   else{Wtime();}//闹铃进制控制
   
   
}

#endif