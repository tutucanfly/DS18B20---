#include "stc15.h"
#include "onewire.h"
#include "ds1302.h"

unsigned long temp = 0;
unsigned long tempList[10] = {0,0,0,0,0,0,0,0,0,0};
unsigned char index = 0;
unsigned char flagSwitch = 0;
code unsigned char SMG_NoDot[16] = {
0xc0, //0
0xf9, //1
0xa4, //2
0xb0, //3
0x99, //4
0x92, //5
0x82, //6
0xf8, //7
0x80, //8
0x90, //9
0x88, //A
0x83, //b
0xc6, //C
0xa1, //d
0x86, //E
0x8e //F
};

code unsigned char SMG_Dot[16] = {
0x40, //0
0x79, //1
0x24, //2
0x30, //3
0x19, //4
0x12, //5
0x02, //6
0x78, //7
0x00, //8
0x10, //9
0x08, //A
0x03, //b
0x46, //C
0x21, //d
0x06, //E
0x0e //F
};

sbit R1 = P3^0;
sbit R2 = P3^1;
sbit R3 = P3^2;
sbit R4 = P3^3;

code unsigned char DS1302_Write[7] = {0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};
code unsigned char DS1302_Read[7] = {0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};
unsigned char time[7] = {0x50,0x59,0x23,0x01,0x01,0x01,0x01};
unsigned char timeMonitor = 0;
unsigned char timeMonitorCount = 0;
unsigned char countTen = 0;

unsigned char spaceTime = 1,spaceTimeTemp = 1;

unsigned char workingMode = 0;

//定时器0使用
unsigned char timer0Count = 0;

//闪烁标志
unsigned char smg_ = 0;

unsigned char firstRun = 0;

void delay(unsigned int n)
{
	while(n--);
}

void InitTimer0()
{
	TMOD = 0x01;
	TH0 = (65535 - 50000)/256;
	TL0 = (65535 - 50000)%256;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}

void Timer0() interrupt 1
{
	TH0 = (65535 - 50000)/256;
	TL0 = (65535 - 50000)%256;
	timer0Count ++;
	if(timer0Count >= 10)
	{
		timer0Count = 0;
		if(smg_ == 0)
		{
			smg_ = 1;
		}
		else
		{
			smg_ = 0;
		}
	}
	
}
void SelectHC573(unsigned char n)
{
	switch(n)
	{
		case 4:
			P2 = (P2 & 0X1F)|0x80;
		break;
		case 5:
			P2 = (P2 & 0X1F)|0XA0;
		break;
		case 6:
			P2 = (P2 & 0X1F)|0XC0;
		break;
		case 7:
			P2 = (P2 & 0X1F)|0XE0;
		break;
	}
}

void InitDS1302()
{
	unsigned char n = 0;
	Write_Ds1302_Byte(0x8e,0x00);//允许写入时钟
	for(n = 0;n<=7;n++)
	{
		Write_Ds1302_Byte(DS1302_Write[n],time[n]);
	}
	Write_Ds1302_Byte(0x8e,0x80);//拒绝写入
}

void ReadDS1302()
{
	unsigned char n = 0;
	for(n = 0;n <= 7;n++)
	{
		time[n] = Read_Ds1302_Byte(DS1302_Read[n]);
	}
}

void smgDisplay(unsigned char m,unsigned char n)
{
	SelectHC573(6);
	P0 = 0x01 << m;
	SelectHC573(7);
	P0 = n;
}

void ReadTemp()
{
	unsigned char LSB,MSB;
	unsigned long t;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	delay(6000);
	init_ds18b20();
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xBE);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	init_ds18b20();
	
	t = MSB;
	t = (t << 8) | LSB;
	if(!(MSB > 0x1f))
	{
		t *= 625;
		temp = t;
	}
}

void Display_timeSpace()
{
	P0 = 0xff;
	smgDisplay(0,0xff);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(1,0xff);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(2,0xff);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(3,0xff);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(4,0xff);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(5,0xbf);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(6,SMG_NoDot[spaceTimeTemp/10]);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(7,SMG_NoDot[spaceTimeTemp%10]);
	delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	P0 = 0xff;
}

void Display_time()
{
	P0 = 0xff;
	smgDisplay(0,SMG_NoDot[time[2]/16]);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(1,SMG_NoDot[time[2]%16]);
	delay(500);
	
	P0 = 0xff;
	if(smg_ )
		smgDisplay(2,0xbf);
	else
		smgDisplay(2,0xff);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(3,SMG_NoDot[time[1]/16]);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(4,SMG_NoDot[time[1]%16]);
	delay(500);
	
	P0 = 0xff;
	if(smg_ )
		smgDisplay(5,0xbf);
	else
		smgDisplay(5,0xff);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(6,SMG_NoDot[time[0]/16]);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(7,SMG_NoDot[time[0]%16]);
	delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	P0 = 0xff;
}

void Display_temp()
{
	P0 = 0xff;
	smgDisplay(0,0xbf);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(1,SMG_NoDot[0]);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(2,SMG_NoDot[index]);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(3,0xff);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(4,0xff);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(5,0xbf);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(6,SMG_NoDot[tempList[index]/100000%10]);
	delay(500);
	
	P0 = 0xff;
	smgDisplay(7,SMG_NoDot[tempList[index]/10000%10]);
	delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	P0 = 0xff;
}

void buttonMode1()
{
	
	if(R4 == 0)
	{
		while(R4 == 0)
		{
			Display_timeSpace();
		}
		
		if(spaceTimeTemp == 1)
		{
			spaceTimeTemp = 5;
		}
		else if(spaceTimeTemp ==5)
		{
			spaceTimeTemp = 30;
		}
		else if(spaceTimeTemp == 30)
		{
			spaceTimeTemp = 60;
		}
		else 
		{
			spaceTimeTemp = 1;
		}
	}
	
	if(R3 == 0)
	{
		while(R3 == 0)
		{
			Display_timeSpace();
		}
		spaceTime = spaceTimeTemp;
		workingMode = 1;
	}
}

void buttonMode2()
{
	unsigned char i;
	if(R2 == 0)
	{
		while(R2 == 0)
		{
			Display_temp();
		}
		if(flagSwitch == 0)
		{
			flagSwitch = 1;
		}
		else
		{
			index++;
			if(index >= 10)
			{
				index = 0;
			}
		}
	}
	if(R1 == 0)
	{
		while(R1 == 0)
		{
			Display_temp();
		}
		workingMode = 0;
		for(i = 0;i<10;i++)
		{
			tempList[i] = 0;
		}
		index = 0;
		flagSwitch = 0;
		timeMonitor = 0;
		timeMonitorCount = 0;
		countTen = 0;
		spaceTime = 1,spaceTimeTemp = 1;
	}
}

void TimeChangeMonitor()
{
	if(timeMonitor != time[0])
	{
		timeMonitor = time[0];
		timeMonitorCount ++;
	}
}

void InitSystem()
{
	SelectHC573(4);
	P0 = 0xff;
	SelectHC573(5);
	P0 = 0x00;
	timeMonitor = time[0];
	InitTimer0();
}

void main()
{
	unsigned char memory;
	InitSystem();
	while(1)
	{
		if(workingMode == 0)
		{
			buttonMode1();
			Display_timeSpace();
		}
		else if(workingMode == 1)
		{
			if(firstRun == 0)
			{
				InitDS1302();
				firstRun = 1;
			}

			if(countTen >= 10)
			{
				//亮灯
				if(smg_ && flagSwitch == 0)
				{
					memory = P2;
					SelectHC573(4);
					P0 = P0 & 0xfe;
					P2 = memory;
				}
				else
				{
					memory = P2;
					SelectHC573(4);
					P0 = (P0 & 0xfe)|0x01;
					P2 = memory;
				}
				Display_temp();
				buttonMode2();
			}
			else
			{
				if(timeMonitorCount >= spaceTime)
				{
					timeMonitorCount = 0;
					//存储温度
					tempList[countTen] = temp;
					countTen ++;
				}
				else
				{
					TimeChangeMonitor();
				}
				Display_time();
				ReadDS1302();
				ReadTemp();
			}
		}
	}
}