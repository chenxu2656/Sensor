#ifndef __ds18B20_H__
#define __ds18B20_H__
sbit DQ = P1^4;        //DS18B20�����ݿ�λP1.4

void DelayUs2x(unsigned char t)
{   
 while(--t);
}


void DelayMs(unsigned char t)
{
     
 while(t--)//������ʱ1mS
 {	 
     //DelayUs2x(245);
	 //DelayUs2x(245);
	 display();
	 display();//������ʱ��˳��ɨ��
	 display();
	 display();
 }
}


bit Init_DS18B20(void)
{
 bit dat=0;
 DQ = 1;    //DQ��λ
 DelayUs2x(10);   //������ʱ
 DQ = 0;         //��Ƭ����DQ����
 //DelayUs2x(200); //��ȷ��ʱ ���� 480us С��960us
 display();//������ʱ��˳��ɨ��
 display();//������ʱ��˳��ɨ��
 display();
 display();
 display();
 //DelayUs2x(200);
 DQ = 1;        //��������
 //DelayUs2x(40); //15~60us �� ����60-240us�Ĵ�������
 display();
 display();
 //DelayUs2x(40);
 dat=DQ;        //���x=0���ʼ���ɹ�, x=1���ʼ��ʧ��
 //DelayUs2x(40); //������ʱ����
 display();
 display();
 return dat;
}

/*------------------------------------------------
                    ��ȡһ���ֽ�
------------------------------------------------*/
unsigned char ReadOneChar(void)
{
unsigned char i=0;
unsigned char dat = 0;
for (i=8;i>0;i--)
 {display();
  DQ = 0; // �������ź�
  dat>>=1;
  DQ = 1; // �������ź�
  if(DQ)
   dat|=0x80;
  //DelayUs2x(40);
  display();
  display();
 }
 return(dat);
}
/*------------------------------------------------
                    д��һ���ֽ�
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
                    ��ȡ�¶�
------------------------------------------------*/
unsigned int ReadTemperature(void)
{
unsigned char a=0;
unsigned int b=0;
unsigned int t=0;
Init_DS18B20();
WriteOneChar(0xCC); // ����������кŵĲ���
WriteOneChar(0x44); // �����¶�ת��
DelayMs(40);
Init_DS18B20();
WriteOneChar(0xCC); //����������кŵĲ��� 
WriteOneChar(0xBE); //��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�
a=ReadOneChar();   //��λ
b=ReadOneChar();   //��λ
b<<=8;
t=a+b;display();
return(t);
}

#endif
