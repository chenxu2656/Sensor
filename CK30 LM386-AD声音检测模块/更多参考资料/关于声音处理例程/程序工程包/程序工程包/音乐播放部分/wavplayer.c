#include <12c56.H>

#define uint8 unsigned char 
#define uint16 unsigned int
#define uint32 unsigned long

sbit SDCS = P1^4;
sbit LAST = P3^3;
sbit NEXT = P3^4;
sbit STOP = P2^5;

uint32 played;

volatile uint32 start_offset;

volatile uint16 tptr=0,bdat=0;

//	播放   	插卡 曲目	 立体声
bit PLAY=0, CK=1  ,QM=0, STEREO=1;

xdata uint8 buf0[512];

void delay(uint16 dly)
{
	uint16 i;
	uint8  j;

	for(i=0;i<dly;i++)
		for(j=0;j<10;j++)
			;
}


void T1ISR() interrupt 3
{
	static uint8 x=0;

 	if(!PLAY)
	{
		x++;
		if(x==4)x=0;
	
		if(!x)
		{
			if(CK)
			{
				if(tptr==1920)
				{
					tptr=0;
					TR1=0;
				}
				else
			  		tptr++;
			}
			else if(QM)
			{
				if(tptr==1376)
				{
					tptr=0;
					TR1=0;
				}
				else
					tptr++;
			}
			else
			{
		 		ISP_ADDRH=(tptr>>8)&0xFF;
				ISP_ADDRL=(tptr   )&0xFF;
			
				ISP_CONTR=0x80;
				ISP_CMD=1;
				ISP_TRIG=0x46;
				ISP_TRIG=0xB9;
			
				tptr++;
		
				if(tptr==0x0480)TR1=0;	//容量
				if(tptr==0x0780)TR1=0;	//0
				if(tptr==0x0a80)TR1=0;	//1
				if(tptr==0x0d80)TR1=0;	//2
				if(tptr==0x1080)TR1=0;	//3
				if(tptr==0x1380)TR1=0;	//4
				if(tptr==0x1680)TR1=0;	//5
				if(tptr==0x1980)TR1=0;	//6
				if(tptr==0x1c80)TR1=0;	//7
				if(tptr==0x1f80)TR1=0;	//8
				if(tptr==0x2280)TR1=0;	//9
				if(tptr==0x2580)TR1=0;	//千
				if(tptr==0x2880)TR1=0;	//百
				if(tptr==0x2b80)TR1=0;	//十
				if(tptr==0x2e80)TR1=0;	//兆
			}
		}
		else
		{
		  ;
		}
	}
	else
	{
		CCAP0H=buf0[tptr++];
		bdat--;
		if(STEREO)
		{
			CCAP1H=buf0[tptr++];
			bdat--;
		}
		else
		{
		 	CCAP1H=CCAP0H;
		}
		if(tptr>=512)
		{
			tptr=0;
		}
	}
}

void SPI_INIT()
{
 	SPCTL=0xd0;
}

void SPI_INIT_LOW()
{
 	SPCTL=0xd1;
}

uint8  SPI_COMM(uint8 dat)
{
 	SDCS=0;
	SPDAT=dat;
	while((SPSTAT&0x80)==0);
	SPSTAT=0x80;
	SDCS=1;
	return(SPDAT);
}

uint8 SD_SendCommand(uint8 cmd, uint32 arg)
{
	uint8 r1;
	uint8 retry=0;
	
	SPI_COMM(0xff);
	SDCS=0;
	
	SPI_COMM(cmd | 0x40);//分别写入命令	//send command
	SPI_COMM(arg>>24);
	SPI_COMM(arg>>16);
	SPI_COMM(arg>>8);
	SPI_COMM(arg);
	SPI_COMM(0x95);
	
	while((r1 = SPI_COMM(0xff)) == 0xff)//等待响应，	//wait response
		if(retry++ > 20) break;//超时退出					//time out error

	SDCS=1;

	return r1;//返回状态值					//return state
}

uint8  InitSD()
{
	uint8 tmp,ret;
	
	for(tmp=0;tmp<0x0f;tmp++)
		SPI_COMM(0xff);
	
	SDCS=0;
	SPI_COMM(0x40);
	SPI_COMM(0x00);
	SPI_COMM(0x00);
	SPI_COMM(0x00);
	SPI_COMM(0x00);
	SPI_COMM(0x95);
	
	for(tmp=0;tmp<0x7f;tmp++)
	{
		ret=SPI_COMM(0xff);
		
		if(ret==0x01)
		{			
			uint8 retry=0,r1;
			
			do
			{
				r1 = SD_SendCommand(1, 0);//发active命令	//send active command
				retry++;
				if(retry>100) return 1;//超时退出		//time out
			} while(r1);
			
			//SPI_High();
			r1 = SD_SendCommand(59, 0);//关crc		//disable CRC

			r1 = SD_SendCommand(16, 512);//设扇区大小512	//set sector size to 512
		
			return 0;
		}
	}
	
	SDCS=1;
	return 1;
}

//读一个扇区		//read one sector
uint8 SD_ReadSector(uint32 sector, uint8* buffer)
{
	uint8 r1;
	uint16 i;
	uint16 retry=0;

	r1 = SD_SendCommand(17, (start_offset+sector)<<9);//读命令	//read command
	
	if(r1 != 0x00)
		return r1;

	SDCS=0;
	
	//等数据的开始	//wait to start recieve data
	while(SPI_COMM(0xff) != 0xfe)
		if(retry++ > 1000)
		{
			SDCS=1;
			return 1;
		}

	for(i=0; i<512; i++)//读512个数据
	{
		bdat++;
		while(bdat>=512);
		*buffer++ = SPI_COMM(0xff);
	}


	SPI_COMM(0xff);//伪crc
	SPI_COMM(0xff);
	
	SDCS=1;

	return 0;
}


void TIMER0_INIT()
{
	TMOD=0x22;			
	TH0=TL0=0xFF;
	TR0=1;
	ET0=0;
}

void TIMER1_INIT(uint16 hz)
{
	if(hz>=8000)
	{
		CLK_DIV=0x00;
		TH1=TL1=(uint8)(256-((uint32)1040000/hz));//0x7e;
	}
	else
	{
		CLK_DIV=0x02;
		TH1=TL1=(uint8)(256-((uint32)260000/hz));//0x7e;
	}
	TR1=0;
	EA=1;
	ET1=1;
}

void CCP_INIT()
{
	AUXR|=0x80;
	CMOD=0x04;
	CL=0x00;
	CH=0x00;
	CCAP0L=0x80;
	CCAP0H=0x80;
	CCAPM0=0x42;
	CCAP1L=0x80;
	CCAP1H=0x80;
	CCAPM1=0x42;
	CR=1;
}


void main()//主函数
{
	uint8 i=0;
	uint32 ptr;

	uint8  FATs;
	uint16 BytesPerSector;
	uint8  SectorsPerClust;
	uint8  FirstFATSector;
	uint16 RootDirCount;
	uint16 RootDirSectors;
	uint16 FirstDirSector;
	uint16 FirstDataSector;

	uint32 samps;
	uint8  sbits;
	uint32 filelen;

	uint16 sPtr=0,iPtr=0;
	uint8  currTrack=0,Track=1;

	//------ 3.3V I/O口配置 -------
	P1M0=P1M1=0xf0;	//P1.7-P1.4开漏
	//-----------------------------

	TIMER0_INIT();
	CCP_INIT();
	IT0=1;
	EX0=1;


RESTART:

	Track=0;

	TR1=0;

	CK=1;
	tptr=0;
	PLAY=0;
	

	TIMER1_INIT(8000);

	TR1=1;

	while(TR1);

	CK=0;

	SPI_INIT_LOW();

	while(1)
	{
		if(!InitSD())break;
	}

	SPI_INIT();

	delay(1000);


	tptr=0;
	TR1=1;

	while(TR1);

	TR1=1;
	while(TR1);
	delay(5000);
	goto read;

LASTTRACK:
	TR1=0;
	PLAY=0;
	start_offset=0;
	currTrack=0;
	Track--;
	goto read;

NEXTTRACK:
	TR1=0;
	PLAY=0;
	start_offset=0;
	currTrack=0;
	Track++;

read:
	while(1)
	{
		//FAT16文件系统处理

		//判断引导扇区有效性
	bdat=65535;
		if(!SD_ReadSector(0,buf0))
		{
			if((buf0[0]!=0xeb)||(buf0[1]!=0x3c)||(buf0[2]!=0x90))	//如果不是EB 3C 90说明有分区表，继续读取之，否则直接开始播放
			{
			 	start_offset=(buf0[0x01c6]<<0)|(buf0[0x01c7]<<8)|(buf0[0x01c8]<<16)|(buf0[0x01c9]<<24);
	bdat=65535;
				SD_ReadSector(0,buf0);
			}
			if((buf0[510]==0x55)&&(buf0[511]==0xaa))
			{
				;
			}
			else
			{
				goto RESTART;
			}
		}
		else
		{
			goto RESTART;
		}

		//读FAT表信息
		FATs=buf0[0x10];
		BytesPerSector=buf0[0x0B]|(buf0[0x0C]<<8);
		SectorsPerClust=buf0[0x0D];
		FirstFATSector=buf0[0x0E];
		RootDirCount=buf0[0x11]|(buf0[0x12]<<8);
		RootDirSectors	= (RootDirCount>>4);//根目录占用的扇区数
		FirstDirSector=FirstFATSector+FATs*(buf0[0x16]|(buf0[0x17]<<8));
		FirstDataSector	= FirstDirSector+RootDirSectors;//第一个数据扇区
		//搜索WAV文件			
				for(sPtr=0;sPtr<RootDirSectors;sPtr++)
				{
					bdat=65535;	//-1(允许读卡一次读完512字节)
					SD_ReadSector(FirstDirSector+sPtr,buf0);

					for(iPtr=0;iPtr<512;iPtr+=32)
					{
						if((buf0[iPtr]!='.')&&(buf0[iPtr]!=0x00)&&(buf0[iPtr]!=0xe5) & (buf0[iPtr+0x0B]!=0x0f))
							if((buf0[iPtr+0x08]=='W')&&(buf0[iPtr+0x09]=='A')&&(buf0[iPtr+0x0a]=='V'))
							{
								ptr=(uint32)FirstDataSector+(uint32)SectorsPerClust*((buf0[iPtr+0x1A]|(buf0[iPtr+0x1B]<<8))-2);
								filelen=(((uint32)buf0[iPtr+0x1c]|(((uint32)buf0[iPtr+0x1d])<<8)|(((uint32)buf0[iPtr+0x1e])<<16)|((uint32)(buf0[iPtr+0x1f])<<24))/512)-2;
								currTrack++;
								if(currTrack==Track)break;
							}
					}
					if(currTrack==Track)break;
				}
			
			
			if(currTrack!=Track)
			{
				Track=0;
				goto NEXTTRACK;
			}
			
			bdat=65535;
			SD_ReadSector(ptr++,buf0);	

			samps=(buf0[0x18]|(buf0[0x19]<<8)|(buf0[0x1A]<<16)|(buf0[0x1B]<<24));
			sbits=buf0[0x22];

			if(buf0[0x16]==2)
				STEREO=1;
			else
				STEREO=0;


			if(sbits!=8)
			{
			goto NEXTTRACK;
			 }


			if(samps>48000)
			{
				goto NEXTTRACK;
			}


	TIMER1_INIT(8000);
	QM=1;
	tptr=0;
	TR1=1;
	while(TR1);
	QM=0;

	TIMER1_INIT(samps/2);//(24000);

	bdat=65535;

	SD_ReadSector(ptr++,buf0);

	tptr=0;
	played=0;

	PLAY=1;
	TR1=1;

	while(1)
	{
		
		if(SD_ReadSector(ptr++,buf0))goto RESTART;
		if(!LAST)goto LASTTRACK;
		if(!NEXT)goto NEXTTRACK;
		if(!STOP)PCON=PCON^0xfe;
		if((played++)>filelen)goto NEXTTRACK;
	
	}



	}
} 

void exint0() interrupt 0
{	
      ;
}