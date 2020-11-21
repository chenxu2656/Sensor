//接线方式如下图:不同的电路需要选择不同的板卡和窗口通道请根据需要执行调整

//GND 	接GND
//V	接	接5V
//S	接	A3

//这里利用A3模拟信号接口检测传感器信号

#define  AO     A3


unsigned int Get_db_value(void);
unsigned int Get_ad_value(void);

void setup() 
{

  pinMode(AO, INPUT);
  
  Serial.begin(9600);   //打开串口

  delay(2000);          //第一次通电后延时，目的为给一定时间模块初始化
}

unsigned char num;
unsigned int dat;
float db_value,temp_db;

void loop() 
{
  dat = Get_db_value();
  db_value = (dat*120.0/1024.0*2.2);  //120是最大分贝值，1024是AD值，根据单片机需要修改，2.2，是AD参考电压为5V，而模块最大输出值是2.8，所以5/2.8=2.2，还原最大值。
  if(db_value < 50)
  { 
    db_value += 40;                   //DB小于50后，加上40，用于显示最小DB值，目的是模仿分贝仪最小值，无意义。
  }
  if(temp_db == 0)
  {
    temp_db = db_value;
  }
  
  if((db_value - temp_db) < 20)
  {
    if(db_value > temp_db)
    {
      db_value = temp_db++;
    }
    else
    {
      db_value = temp_db--;
    }
  }
  
  Serial.print("db:  ");
  Serial.println(db_value);
  
  delay(1);
}

//-------------------------db 值优化-------------------------//

#define  Len_db      20
#define  Min_Len_db  5
#define  Max_Len_db  5

unsigned int DB_TAB[Len_db];
unsigned int Get_db_value(void)
{
  unsigned char a,b,c;
  unsigned int temp;
  unsigned long int mindat=0,maxdat=0,sumdat=0;
  
  for(a=0;a<Len_db;a++)
  {
    DB_TAB[a] = Get_ad_value();
    sumdat = sumdat + DB_TAB[a];
  }

  for(b=0;b<Len_db;b++)            //按从小到大排列
  {
    for(c=0;c<Len_db-b;c++)
    {
      if(DB_TAB[c] > DB_TAB[c+1])
      {
        temp = DB_TAB[c+1];
        DB_TAB[c+1] = DB_TAB[c];
        DB_TAB[c] = temp;
      }
    }
  }
  for(a=0;a<Min_Len_db;a++)    //删除最小的数据
  {
    mindat = mindat + DB_TAB[a];
  }
  
  for(a=(unsigned int)(Len_db-Max_Len_db);a<Len_db;a++)    //删除最大的数据
  {
    maxdat = maxdat + DB_TAB[a];
  }
  
  temp = ((unsigned int)(sumdat - mindat - maxdat)/(unsigned int)(Len_db - Min_Len_db - Max_Len_db));   //求平均值
  return  temp;
}
//-----------------------------------------------------------//

//-------------------------ad 值优化-------------------------//

#define  Len      100
#define  Max_Len  70

unsigned int AD_TAB[Len];
unsigned int Get_ad_value(void)
{
  unsigned char a,b,c;
  unsigned int temp;
  unsigned long int mindat=0,maxdat=0,sumdat=0;
  
  for(a=0;a<Len;a++)
  {
    AD_TAB[a] = analogRead(3);
    sumdat = sumdat + AD_TAB[a];
  }

  for(b=0;b<Len;b++)    //按从小到大排列
  {
    for(c=0;c<Len-b;c++)
    {
      if(AD_TAB[c] > AD_TAB[c+1])
      {
        temp = AD_TAB[c+1];
        AD_TAB[c+1] = AD_TAB[c];
        AD_TAB[c] = temp;
      }
    }
  }
  
  for(a=(unsigned int)(Len-Max_Len);a<Len;a++)    //删除最大的数据
  {
    maxdat = maxdat + AD_TAB[a];
  }
  
  temp = ((unsigned int)(sumdat - maxdat)/(unsigned int)(Len-Max_Len));    //求平均值
  return  temp;
}
//-----------------------------------------------------------//




