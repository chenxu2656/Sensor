#include<reg52.h>
#include<intrins.h>
#include<math.h>
#define uint unsigned int
#define uchar unsigned char
uint d,DB;
uchar flag=0;
uchar flag1=0;
sbit lcden=P3^4;
sbit lcdrs=P3^5;

sbit dula=P2^6;
sbit wela=P2^7;

sbit wr=P3^6;
sbit rd=P3^7;
sbit beep=P2^3;
code char SIN_TAB[128]={0,6,12,18,24,30,36,42,48,54,59,65,70,75,80,85,89,94,98,102, 
                        105,108,112,114,117,119,121,123,124,125,126,126,126,126,126,
						125,124,123,121,119,117,114,112,108,105,102,98,94,89,85,80,75,
						70,65,59,54,48,42,36,30,24,18,12,6,0,-6,-12,-18,-24,-30,-36,-42,
						-48,-54,-59,-65,-70,-75,-80,-85,-89,-94,-98,-102,-105,-108,-112,
						-114,-117,-119,-121,-123,-124,-125,-126,-126,-126,-126,-126,-125,
						-124,-123,-121,-119,-117,-114,-112,-108,-105,-102,-98,-94,-89,-85,
						-80,-75,-70,-65,-59,-54,-48,-42,-36,-30,-24,-18,-12,-6};

//放大128倍后的cos整数表（128）
code char COS_TAB[128]={127,126,126,125,124,123,121,119,117,114,112,108,105,102,98,94, 
                        89,85,80,75,70,65,59,54,48,42,36,30,24,18,12,6,0,-6,-12,-18,-24,
						-30,-36,-42,-48,-54,-59,-65,-70,-75,-80,-85,-89,-94,-98,-102,
						-105,-108,-112,-114,-117,-119,-121,-123,-124,-125,-126,-126,-126,
						-126,-126,-125,-124,-123,-121,-119,-117,-114,-112,-108,-105,-102,
						-98,-94,-89,-85,-80,-75,-70,-65,-59,-54,-48,-42,-36,-30,-24,-18,
						-12,-6,0,6,12,18,24,30,36,42,48,54,59,65,70,75,80,85,89,94,98,102,
						105,108,112,114,117,119,121,123,124,125,126,126};

//采样存储序列表
code char LIST_TAB[128] = {0,64,32,96,16,80,48,112,8,72,40,104,24,88,56,120,4,68,36,100,20,84,52,116,
                           12,76,44,108,28,92,60,124,2,66,34,98,18,82,50,114,10,74,42,106,26,90,58,
						   122,6,70,38,102,22,86,54,118,14,78,46,110,30,94,62,126,1,65,33,97,17,81,49,
						   113,9,73,41,105,25,89,57,121,5,69,37,101,21,85,53,117,13,77,45,109,29,93,61,
						   125,3,67,35,99,19,83,51,115,11,75,43,107,27,91,59,123,7,71,39,103,23,87,55,
						   119,15,79,47,111,31,95,63,127};
code char T[7]={2,3,5,11,18,26,56};
uchar COUNT=15,COUNT1=0,ADC_Count=0,LINE=15,G=0;
uint i,j,k,b,p,anum,led=0;                 
int Temp_Real,Temp_Imag,temp;                // 中间临时变量  
uint TEMP1,max=0;	
int xdata Fft_Real[128]; 
int xdata Fft_Image[128];               // fft的虚部
 
				

void delay1uS(unsigned int i)//延时1微秒
{
   while(i--);
}


void delay(uint z)
{
 uint a,b;
 for(a=z;a>0;a--)
    for(b=110;b>0;b--);
}

void FFT()//
{                 
    for( i=1; i<=7; i++)                            /* for(1)  N=7 */
    { 
        b=1;
        b <<=(i-1);                                 //碟式运算，用于计算隔多少行计算例如 第一极 1和2行计算
        for( j=0; j<=b-1; j++)                      /* for (2) */
        { 
            p=1;
            p <<= (7-i);            
            p = p*j;
            for( k=j; k<128; k=k+2*b)               /* for (3) 基2fft */
            { 
                Temp_Real=Fft_Real[k];
				Temp_Imag=Fft_Image[k];
				temp=Fft_Real[k+b];
                Fft_Real[k]=Fft_Real[k]+((Fft_Real[k+b]*COS_TAB[p])>>7)+((Fft_Image[k+b]*SIN_TAB[p])>>7);
                Fft_Image[k]=Fft_Image[k]-((Fft_Real[k+b]*SIN_TAB[p])>>7)+((Fft_Image[k+b]*COS_TAB[p])>>7);
                Fft_Real[k+b]=Temp_Real-((Fft_Real[k+b]*COS_TAB[p])>>7)-((Fft_Image[k+b]*SIN_TAB[p])>>7);
                Fft_Image[k+b]=Temp_Imag+((temp*SIN_TAB[p])>>7)-((Fft_Image[k+b]*COS_TAB[p])>>7);          // 移位.防止溢出. 结果已经是本值的 1/64               
                Fft_Real[k] >>= 1;             
                Fft_Image[k] >>= 1; 
                Fft_Real[k+b] >>= 1;                 
                Fft_Image[k+b] >>= 1; 
                                                                               
            }     
        } 
    } 
 for(k=0;k<128;k++){Fft_Real[k]=Fft_Real[k]*128;Fft_Image[k]= Fft_Image[k]*128;}
 	     
}

void write_com(uchar com)
{
 lcdrs=0;
 P0=com;
 delay(1);
 lcden=1;
 delay(1);
 lcden=0;
}

void write_data(uchar date)
{
 lcdrs=1;
 P0=date;
 delay(1);
 lcden=1;
 delay(1);
 lcden=0;
}

void lcdinit()
{ 
  dula=0;
  wela=0;
  lcden=0;
  write_com(0x38);
  write_com(0x0c);
  write_com(0x06);
  write_com(0x01);
  write_com(0x80);
}

void init()
{
 TMOD=0x01;
 ET0=1;
 EA=1;
 TH0=(65536-500)/256;
 TL0=(65536-500)%256;
 
}

void ADC()		//采样一次的数据
{
 
  wr=1;
  _nop_();
  wr=0;
  _nop_();
  wr=1;
   P1=0xff;
   rd=1;
   _nop_();
   rd=0;
   _nop_();
   d=P1;
  delay1uS(120);	  //延时120US ADC的转换时间是100US
  if(d>=128){d=d-128;d=d*2;}
  else {d=128-d;d=d*2;}	   //AD出来的是-2.5到2.5的幅度的值
   rd=1;   
}
   void deal()
   {
	uchar n;
    float data SPL[7]={0,0,0,0,0,0,0};	
    for	(n=1;n<3;n++)
    SPL[0]=SPL[0]+((Fft_Real[n]*Fft_Real[n])+(Fft_Image[n]*Fft_Image[n]));
	for	(n=2;n<5;n++)
    SPL[1]=SPL[1]+((Fft_Real[n]*Fft_Real[n])+(Fft_Image[n]*Fft_Image[n]));
	for	(n=4;n<9;n++)
    SPL[2]=SPL[2]+((Fft_Real[n]*Fft_Real[n])+(Fft_Image[n]*Fft_Image[n]));
	for	(n=8;n<19;n++)
    SPL[3]=SPL[3]+((Fft_Real[n]*Fft_Real[n])+(Fft_Image[n]*Fft_Image[n]));
	for	(n=18;n<36;n++)
    SPL[4]=SPL[4]+((Fft_Real[n]*Fft_Real[n])+(Fft_Image[n]*Fft_Image[n]));
	for	(n=35;n<72;n++)
    SPL[5]=SPL[5]+((Fft_Real[n]*Fft_Real[n])+(Fft_Image[n]*Fft_Image[n]));
	for	(n=71;n<128;n++)
    SPL[6]=SPL[6]+((Fft_Real[n]*Fft_Real[n])+(Fft_Image[n]*Fft_Image[n]));
	for(n=0;n<7;n++)
	{SPL[n]=10*log10 (SPL[n]/T[n]);};
	for(n=0;n<7;n++)
	{SPL[n]=SPL[n]/10;};
	for(n=0;n<7;n++)
	{SPL[n]=pow(10,SPL[n]);};
	DB=SPL[0]+SPL[1]+SPL[2]+SPL[3]+SPL[4]+SPL[5]+SPL[6];
	DB=10*log10(DB);
	
	
   }
			
  void main()
{
 init();
lcdinit();
while(1)
{
 TR0=1;
 flag=1;
 wela=1;
 P0=0x7f;
 wela=0;
 if(flag==1)
 { 
 if(flag1==1)
  {
  ADC();
  flag1=0; 
  }
  Fft_Real[LIST_TAB[ADC_Count]]=d;//按LIST_TAB表里的顺序，进行存储采样值
  if(ADC_Count<=127){ADC_Count++;} 
  else {wela=1;P0=0xff;wela=0;TR0=0;flag=0;}    
 }	
    FFT();  
   deal();
     
   write_com(0x80+0x0a);
   write_data(0x30+DB/10000);
   delay(1);
   write_data(0x30+DB%10000/1000);
   delay(1);
   write_data(0x30+DB%1000/100);
   delay(1);
   write_data(0x30+DB%100/10);
   delay(1);
   write_data(0x30+DB%10);
   delay(1);
   delay(1000);
   }  
}
 

void t0() interrupt 1
{
  uchar t;
  TH0=(65536-50)/256;
  TL0=(65536-50)%256; 
  t++;
  if(t==4)
  {t=0;flag1=1;}
}  