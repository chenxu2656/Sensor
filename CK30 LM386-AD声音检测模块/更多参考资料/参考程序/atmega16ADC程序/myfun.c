#include "myfun.h"
#include "string.h"

/******************************************************************
 - 功能描述：将一个32位的变量dat转为字符串，比如把1234转为"1234"
 - 隶属模块：公开函数模块
 - 函数属性：外部，用户可调用
 - 参数说明：dat:带转的long型的变量
             str:指向字符数组的指针，转换后的字节串放在其中           
 - 返回说明：无
 ******************************************************************/

void u32tostr(unsigned long dat,char *str) 
{
 char temp[20];
 unsigned char i=0,j=0;
 i=0;
 while(dat)
 {
  temp[i]=dat%10+0x30;
  i++;
  dat/=10;
 }
 j=i;
 for(i=0;i<j;i++)
 {
  str[i]=temp[j-i-1];
 }
 if(!i) {str[i++]='0';}
 str[i]=0;
}

/******************************************************************
 - 功能描述：将一个字符串转为32位的变量，比如"1234"转为1234
 - 隶属模块：公开函数模块
 - 函数属性：外部，用户可调用
 - 参数说明：str:指向待转换的字符串           
 - 返回说明：转换后的数值
 ******************************************************************/

unsigned long strtou32(char *str) 
{
 unsigned long temp=0;
 unsigned long fact=1;
 unsigned char len=strlen(str);
 unsigned char i;
 for(i=len;i>0;i--)
 {
  temp+=((str[i-1]-0x30)*fact);
  fact*=10;
 }
 return temp;
}