#include<STC12C5A60S2.H>
typedef unsigned char uint8;
typedef unsigned int  uint16; 
#define  channel  0x01			  //设置AD通道为 P1.0
#define ADC_POWER   (1<<7) 
#define ADC_SPEEDHH (0x03<<5) 
#define ADC_START   (1<<3) 
#define PADCH       (1<<5) 
#define ADC_FLAG    (1<<4)
#define P_3 P3 //32列扫描控制
sbit sw=P1^2;//74HC154组切换控制位
//放大128倍后的sin整数表（128）	
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

uint8 LED[32]={0x80,0x00,0xC0,0x00,0xE0,0x00,0xF0,0x00,
               0xF8,0x00,0xFC,0x00,0xFE,0x00,0xFF,0x00,
               0xff,0x80,0xff,0xC0,0xff,0xE0,0xff,0xF0,
			   0xff,0xF8,0xff,0xFC,0xff,0xFE,0xff,0xFF,};
uint8 COUNT=15,COUNT1=0,ADC_Count=0,LINE=15,G=0,T;
uint8 i,j,k,b,p,anum,led=0;                 
int Temp_Real,Temp_Imag,temp;                // 中间临时变量  
uint16 TEMP1,max=0;	
int xdata Fft_Real[128]; 
int xdata Fft_Image[128];               // fft的虚部 
uint8 xdata LED_TAB2[16];				//记录 漂浮物 是否需要 停顿一下
uint8 xdata LED_TAB[16];				//记录红色柱状 
uint8 xdata LED_TAB1[16];				//记录 漂浮点
/////////////shop60610364.taobao.com/////////
void FFT()//基2fft
{                 
    for( i=1; i<=7; i++)                            /* for(1) */
    { 
        b=1;
        b <<=(i-1);                                 //碟式运算，用于计算隔多少行计算例如 第一极 1和2行计算
        for( j=0; j<=b-1; j++)                      /* for (2) */
        { 
            p=1;
            p <<= (7-i);            
            p = p*j;
            for( k=j; k<128; k=k+2*b)               /* for (3)  */
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

	for(j=0;j<16;j++)//16分频											 
	{																			  
		TEMP1=((((Fft_Real[j+1]* Fft_Real[j+1]))+((Fft_Image[j+1]*Fft_Image[j+1])))>>1);//求各频段幅值
		if(TEMP1<1)TEMP1=0; 
		LED_TAB[j]=TEMP1;	
		if(LED_TAB[j]>max)max=LED_TAB[j];
	}
	if(max>16)//分级量化
	{
	  max/=16;
	  for(j=0;j<16;j++)LED_TAB[j]/=max;
	}       
}


void delay2us(void)   //2us延时
{
    unsigned char a,b;
    for(b=1;b>0;b--)
        for(a=3;a>0;a--);
}



void Init()
{  
	
//-----------------------------------------------------------------------------------

     P1ASF = 0x02;                 //0000,0010, 将 P1.1 置成模拟口
     AUXR1 &=0xFB;                 //1111,1011, 令 ADRJ=0
	 EADC=1;					   //AD中断打开
	 ADC_CONTR = ADC_POWER | ADC_SPEEDHH | ADC_START | channel;//1110 1001   1打开 A/D （ADC_POWER）转换电源；11速度为90周期一次；
	    		                                               //0中断标志清零；1启动adc(ADC_START);001AD通道打开（这里为P1.1）;
//-----------------------------------------------------------------------------------
	 
	 P2M0=1;
     TMOD=0X12;					   
	 TH0=0xb0;                                 				  
	 TL0=0xb0;
	 TH1=0xfe;												   
	 TL1=0Xd4;
	 ET0=1;					       //定时器0 打开
	 TR0=0;					   	   //关闭定时器
	 ET1=1;
	 TR1=1;
	 PT1=0;
	 PT0=1;
     IPH=PADCH;
	 IP=PADC;					   //中断优先级
	 EA=1;						   //总中断打开 
}

//----------------------------------------------------------------------------------------
 
 void ADC_Finish() interrupt 5
 {	   ADC_CONTR &= !ADC_FLAG; 
 	   Fft_Real[LIST_TAB[ADC_Count]]=(int)((ADC_RES)<<1)+(ADC_RESL>>1)*2;//按LIST_TAB表里的顺序，进行存储采样值
	  if(ADC_Count<=127)ADC_Count++;
	  else {EADC=0;TR0=0;}	  													 
 }
//---------------------------------------------------------------------------------------
 void LED_Display() interrupt 3					 //中断一次显示一列
 {	 	 
 	TH1=0xfe;												 
	TL1=0Xd4;						
    G++;						  	
    if(G==33)G=1;
	switch(G)								     //往点阵屏填充一列的数据
	  {
		case 1:sw=1;anum=LED_TAB[0];P_3=0x00;break;
		case 2:anum=LED_TAB[0];P_3=0x01;break;
		case 3:anum=LED_TAB[1];P_3=0x02;break;
		case 4:anum=LED_TAB[1];P_3=0x03;break;
		case 5:anum=LED_TAB[2];P_3=0x04;break;
		case 6:anum=LED_TAB[2];P_3=0x05;break;
		case 7:anum=LED_TAB[3];P_3=0x06;break;
		case 8:anum=LED_TAB[3];P_3=0x07;break;
		case 9:anum=LED_TAB[4];P_3=0x08;break;
		case 10:anum=LED_TAB[4];P_3=0x09;break;
		case 11:anum=LED_TAB[5];P_3=0x0a;break;
		case 12:anum=LED_TAB[5];P_3=0x0b;break;
		case 13:anum=LED_TAB[6];P_3=0x0c;break;
		case 14:anum=LED_TAB[6];P_3=0x0d;break;
		case 15:anum=LED_TAB[7];P_3=0x0e;break;
		case 16:anum=LED_TAB[7];P_3=0x0f;break;
		case 17:sw=0;anum=LED_TAB[8];P_3=0x00;break;
		case 18:anum=LED_TAB[8];P_3=0x10;break;
		case 19:anum=LED_TAB[9];P_3=0x20;break;
		case 20:anum=LED_TAB[9];P_3=0x30;break;
		case 21:anum=LED_TAB[10];P_3=0x40;break;
		case 22:anum=LED_TAB[10];P_3=0x50;break;
		case 23:anum=LED_TAB[11];P_3=0x60;break;
		case 24:anum=LED_TAB[11];P_3=0x70;break;
		case 25:anum=LED_TAB[12];P_3=0x80;break;
		case 26:anum=LED_TAB[12];P_3=0x90;break;
		case 27:anum=LED_TAB[13];P_3=0xa0;break;
		case 28:anum=LED_TAB[13];P_3=0xb0;break;
		case 29:anum=LED_TAB[14];P_3=0xc0;break;
		case 30:anum=LED_TAB[14];P_3=0xd0;break;
		case 31:anum=LED_TAB[15];P_3=0xe0;break;
		case 32:anum=LED_TAB[15];P_3=0xf0;break;
		    
		}

	   for(led=0;led<=anum;led++)				 //递增
	     { P0=LED[2*led+1];
		   P2=LED[2*led];
		   delay2us();
		   
		 }										 //递减
	   for(;led>=0;led--)
	     { P0=LED[2*led+1];
		   P2=LED[2*led];
		   delay2us();
		 }
 }
//------------------------------------------------------------------------------------------------------------

void Ad_Control() interrupt 1					 //控制采样率
{
 ADC_CONTR = ADC_POWER | ADC_SPEEDHH| ADC_START | channel;	 //开始AD采集
} 
									  
//===============================================================================================================

 void main()
 {
    uint8 i;
 	Init();
	while(1)
	{		
		ADC_Count=0;
		TR0=1;
		EADC=1;	//开启定时器中断0，开启ADC 
		while(ADC_Count<128); 
        for(i=0;i<128;i++)
        {
         Fft_Image[i]=0;
        }
		FFT();//FFT运算并转换为各频段幅值						   
		 
	}
 }