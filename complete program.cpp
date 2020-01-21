#include "reg52.h"			
typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;
//数码管的位定义
sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;
//--定义使用的IO口--//
#define GPIO_DIG   P0
#define GPIO_TRAFFIC P1
sbit RED10   = P1^0;   //东西人行道红灯
sbit GREEN10 = P1^1;   //东西人行道绿灯
sbit RED11   = P1^2;   //东西红灯
sbit YELLOW11= P1^3;   //东西黄灯
sbit GREEN11 = P1^4;   //东西绿灯
sbit RED00   = P3^0;	//南北人行道红灯
sbit GREEN00 = P3^1;	//南北人行道绿灯
sbit RED01   = P1^5;	//南北红灯
sbit YELLOW01= P1^6;	//南北黄灯
sbit GREEN01 = P1^7;	//南北绿灯
sbit k3     =  P3^3;	//南北路开关
sbit k2     =  P3^2;	//东西路开关
u8 code smgduan[16]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};//显示0~F的值
u8 DisplayData[8];//保存动态数码管显示时间的数组
u8 Second=0;	  //全局变量
/******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
***********************************************************************/
void delay(u16 i)
{
	while(i--);	
}
/*******************************************************************************
* 函 数 名         : DigDisplay
* 函数功能		   : 数码管动态扫描函数，循环扫描8个数码管显示
*****************************************************************************/
void DigDisplay()
{
	u8 i;
	for(i=0;i<8;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;//显示第0位
			case(1):
				LSA=1;LSB=0;LSC=0; break;//显示第1位
			case(2):
				LSA=0;LSB=1;LSC=0; break;//显示第2位
			case(3):											  
				LSA=1;LSB=1;LSC=0; break;//显示第3位
			case(4):
				LSA=0;LSB=0;LSC=1; break;//显示第4位
			case(5):
				LSA=1;LSB=0;LSC=1; break;//显示第5位
			case(6):
				LSA=0;LSB=1;LSC=1; break;//显示第6位
			case(7):
				LSA=1;LSB=1;LSC=1; break;//显示第7位	
		}
		GPIO_DIG=DisplayData[i];//发送段码
		delay(100); //间隔一段时间扫描	
		GPIO_DIG=0x00;//消隐
	}
}
/******************************************************************************
* 函 数 名         : void Timer0() interrupt 1
* 函数功能		   : 定时器0中断函数
* 输    入         : 无
* 输    出         : 无
******************************************************************************/
void Timer0() interrupt 1
{
	static u16 i;
	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;
	i++;
	if(i==1000)
	{
		i=0;
		Second++;
	}	
}
/******************************************************************************
* 函 数 名         : Timer0Init
* 函数功能		   : 定时器0初始化
* 输    入         : 无
* 输    出         : 无
/*****************************************************************************
void Timer0Init()
{
	TMOD|=0X01;//选择为定时器0模式，工作方式1，仅用TR0打开启动。

	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;	
	ET0=1;//打开定时器0中断允许
	EA=1;//打开总中断
	TR0=1;//打开定时器			
}
/******************************************************************************
* 函 数 名         : Int0Init()
* 函数功能		   : 设置外部中断0
* 输    入         : 无
* 输    出         : 无
/******************************************************************************
void Int0Init()
{
	//设置INT0
	IT0=1;//跳变沿触发方式（下降沿）
	EX0=1;//打开INT0的中断允许。	
	EA=1;//打开总中断	
}
/******************************************************************************
* 函 数 名         : Int0()	interrupt 0
* 函数功能		   : 外部中断0的中断函数
* 输    入         : 无
* 输    出         : 无
/******************************************************************************
void Int0()	interrupt 0		//外部中断1的中断函数	东西路上的红灯变成绿灯
{
	delay(1000);	 //延时消抖
	if(k2==0)
	{
		  YELLOW01=0;
		  delay(100000);
		  YELLOW01=1;
		  delay(100000);
		  YELLOW01=0;
		  delay(100000);
		  YELLOW01=1;
		  Second=1;
			if(Second < 31)
		{
			//--东西通行--//
			if(Second<20)
			{
			   		DisplayData[0] = 0x00;
					DisplayData[1] = 0x00;
					DisplayData[2] = 0x00;
					DisplayData[3] = 0x00;
					DisplayData[4] = 0x00;
					DisplayData[5] = 0x00;
					DisplayData[6] = DisplayData[2];
					DisplayData[7] = DisplayData[3];
					DigDisplay();
			}
			GPIO_TRAFFIC = 0xFF;  //将所有的灯熄灭
			RED00 = 1;
			GREEN00 = 1;
			GREEN11 = 0;    //东西绿灯亮			
			GREEN10	= 0;    //东西人行道绿灯亮
			RED01 = 0;      //南北红灯亮
			RED00 = 0;      //南北人行道红灯亮
				if(Second > 19)
				{
					DisplayData[0] = 0x00;
					DisplayData[1] = 0x00;
					DisplayData[2] = smgduan[(30 - Second) % 100 / 10];
					DisplayData[3] = smgduan[(30 - Second) %10];
					DisplayData[4] = 0x00;
					DisplayData[5] = 0x00;
					DisplayData[6] = DisplayData[2];
					DisplayData[7] = DisplayData[3];
					DigDisplay();
				}
				if(Second > 27)
				{
					GREEN11 = 0;    			
					GREEN10	= 0;
					delay(500000);
					GREEN11 = 1;    			
					GREEN10	= 1;
					delay(500000);
					GREEN11 = 0;    			
					GREEN10	= 0;
					delay(500000);
				}
		}
		//--黄灯等待切换状态，5秒--//
		else if(Second < 36) 
		{
			//--黄灯阶段--//
			GPIO_TRAFFIC = 0xFF;  //将所有的灯熄灭
			RED00 = 1;
			GREEN00 = 1;
			YELLOW11 = 0;    //东西路黄灯亮			
			RED10	= 0;     //东西路人行道红灯亮
			RED01 = 0;    //南北路红灯亮
			RED00 = 0;       //南北路人行道红灯亮
		}
	}
}
/*******************************************************************************
* 函 数 名         : Int1Init()
* 函数功能		   : 设置外部中断1
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Int1Init()
{
	//设置INT0
	IT1=1;//跳变沿出发方式（下降沿）
	EX1=1;//打开INT0的中断允许。	
	EA=1;//打开总中断	
}
/*******************************************************************************
* 函 数 名         : Int1()	interrupt 2
* 函数功能		   : 外部中断1的中断函数
* 输    入         : 无
* 输    出         : 无
/*****************************************************************************
void Int1()	interrupt 2		//外部中断1的中断函数	东西路上的红灯变成绿灯
{
	delay(1000);	 //延时消抖
	if(k3==0)
	{
		  	YELLOW11=0;
			delay(100000);
		 	YELLOW11=1;
			delay(100000);
		  	YELLOW11=0;
		 	delay(100000);
		 	YELLOW11=1;
			 Second=36;
		   if(Second < 66) 
		{
			GPIO_TRAFFIC = 0xFF;  //将所有的灯熄灭
			RED00 = 1;
			GREEN00 = 1;
			RED11 = 0;       //东西路红灯亮			
			RED10 = 0;       //东西路人行道红灯亮
			GREEN01 = 0;     //南北路绿灯亮
			GREEN00 = 0;     //南北前进路人行道绿灯亮
				if(Second > 56)
				{
					DisplayData[0] = 0x00;
					DisplayData[1] = 0x00;
					DisplayData[2] = smgduan[(67 - Second) % 100 / 10];
					DisplayData[3] = smgduan[(67 - Second) %10];
					DisplayData[4] = 0x00;
					DisplayData[5] = 0x00;
					DisplayData[6] = DisplayData[2];
					DisplayData[7] = DisplayData[3];
					DigDisplay();
				}
				if(Second > 62)
				{		
					GREEN01 = 0;   			
					GREEN00	= 0;
					delay(100000);
					GREEN01 = 1;    			
					GREEN00	= 1;
					delay(100000);
					GREEN01 = 0;   			
					GREEN00	= 0;
					delay(100000);
				}	
		//--黄灯等待切换状态，5秒--//
		else 
		{		
			DisplayData[0] = 0x00;
		    DisplayData[1] = 0x00;
			DisplayData[2] = 0x00;
			DisplayData[3] = 0x00;
			DisplayData[4] = 0x00;
			DisplayData[5] = 0x00;
			DisplayData[6] = 0x00;
			DisplayData[7] = 0x00;
			DigDisplay();
			//--黄灯阶段--//
			GPIO_TRAFFIC = 0xFF;  //将所有的灯熄灭
			RED00 = 1;
			GREEN00 = 1;
			YELLOW01 = 0;    //南北路黄灯亮			
			RED00	= 0;     //南北路人行道红灯亮
			RED11 = 0;    //东西路红灯亮
			RED10 = 0;     //东西路人行道红灯亮
	     }		  
       }
  }	 
}

/*****************************************************************************
* 函 数 名       : main
* 函数功能		 : 主函数
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void main()
{	
	Second = 0;

	Timer0Init();

	Int0Init();

	Int1Init();  //	设置外部中断1

	while(1)
	{
		if(Second == 70)
		{
			Second = 0;
		}

		//--东西通行，30秒--//
		if(Second < 31)
		{
			//--东西通行--//
			if(Second<20)
			{
			   		DisplayData[0] = 0x00;
					DisplayData[1] = 0x00;
					DisplayData[2] = 0x00;
					DisplayData[3] = 0x00;
					DisplayData[4] = 0x00;
					DisplayData[5] = 0x00;
					DisplayData[6] = DisplayData[2];
					DisplayData[7] = DisplayData[3];
					DigDisplay();
			}
			GPIO_TRAFFIC = 0xFF;  //将所有的灯熄灭
			RED00 = 1;
			GREEN00 = 1;

			GREEN11 = 0;    //东西绿灯亮			
			GREEN10	= 0;    //东西人行道绿灯亮

			RED01 = 0;      //南北红灯亮
			RED00 = 0;      //南北人行道红灯亮
				if(Second > 19)
				{
					DisplayData[0] = 0x00;
					DisplayData[1] = 0x00;
					DisplayData[2] = smgduan[(30 - Second) % 100 / 10];
					DisplayData[3] = smgduan[(30 - Second) %10];
					DisplayData[4] = 0x00;
					DisplayData[5] = 0x00;
					DisplayData[6] = DisplayData[2];
					DisplayData[7] = DisplayData[3];
					DigDisplay();
				}
				if(Second > 27)
				{
					GREEN11 = 0;    			
					GREEN10	= 0;
					delay(100000);
					GREEN11 = 1;    			
					GREEN10	= 1;
					delay(100000);
					GREEN11 = 0;    			
					GREEN10	= 0;
					delay(100000);
				}
		}
		//--黄灯等待切换状态，5秒--//
		else if(Second < 36) 
		{
			//--黄灯阶段--//

			GPIO_TRAFFIC = 0xFF;  //将所有的灯熄灭
			RED00 = 1;
			GREEN00 = 1;

			YELLOW11 = 0;    //东西路黄灯亮			
			RED10	= 0;     //东西路人行道红灯亮

			RED01 = 0;    //南北路红灯亮
			RED00 = 0;       //南北路人行道红灯亮
		}

		//--南北路通行--//
		else if(Second < 66) 
		{
			GPIO_TRAFFIC = 0xFF;  //将所有的灯熄灭
			RED00 = 1;
			GREEN00 = 1;

			RED11 = 0;       //东西路红灯亮			
			RED10 = 0;       //东西路人行道红灯亮

			GREEN01 = 0;     //南北路绿灯亮
			GREEN00 = 0;     //南北前进路人行道绿灯亮
				if(Second > 56)
				{
					DisplayData[0] = 0x00;
					DisplayData[1] = 0x00;
					DisplayData[2] = smgduan[(67 - Second) % 100 / 10];
					DisplayData[3] = smgduan[(67 - Second) %10];
					DisplayData[4] = 0x00;
					DisplayData[5] = 0x00;
					DisplayData[6] = DisplayData[2];
					DisplayData[7] = DisplayData[3];
					DigDisplay();
				}
				if(Second > 62)
				{
					GREEN01 = 0;   			
					GREEN00	= 0;
					delay(100000);
					GREEN01 = 1;    			
					GREEN00	= 1;
					delay(100000);
					GREEN01 = 0;   			
					GREEN00	= 0;
					delay(100000);
				}	
		}
		//--黄灯等待切换状态，5秒--//
		else 
		{		
			DisplayData[0] = 0x00;
		    DisplayData[1] = 0x00;
			DisplayData[2] = 0x00;
			DisplayData[3] = 0x00;
			DisplayData[4] = 0x00;
			DisplayData[5] = 0x00;
			DisplayData[6] = 0x00;
			DisplayData[7] = 0x00;
			DigDisplay();

			//--黄灯阶段--//
			GPIO_TRAFFIC = 0xFF;  //将所有的灯熄灭
			RED00 = 1;
			GREEN00 = 1;
			YELLOW01 = 0;    //南北路黄灯亮			
			RED00	= 0;     //南北路人行道红灯亮
			RED11 = 0;    //东西路红灯亮
			RED10 = 0;       //东西路人行道红灯亮
		}
	}					
}
