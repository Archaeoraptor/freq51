#include <reg51.h>
#include <absacc.h>
#include <ctype.h>


//定义按键
sbit KEY2 = P2^0;		//选择1档位
sbit KEY3 = P2^1;		//选择2档位
sbit KEY4 = P2^2;		//选择3档位

sbit LED1 = P0^0;
sbit LED2 = P0^1;
sbit LED3 = P0^2;

//定义显示缓冲区（由定时中断程序自动扫描）
unsigned char DispBuf[8];
unsigned char T0_count=0;	  //计数器T0溢出次数
unsigned char key_num=2;		  //闸门序号
unsigned char dp_num=0;		  //小数点位置
unsigned char k;		//定义键值变量
//extern unsigned char num_long=0;

unsigned char KeyScan()			//键盘扫描
{
	unsigned char k = '\0';
	if ( KEY2== 0 )	 k='1';
	if ( KEY3== 0 )	 k='2';
	if ( KEY4== 0 )	 k='3';
	return k;
}

/*
void reset()				//复位
{
 ((void(code*)(void))0x0000)();
}
*/
void T0INTSVC() interrupt 1
{
	T0_count++;
}


void DispClear()
{
	unsigned char i;
	for ( i=0; i<8; i++ )
	{
		DispBuf[i] = 0x00;
	}
}

/*
函数：DispChar()
功能：在数码管上显示字符
参数：
	x：数码管的坐标位置（0～7）
	c：要显示的字符（仅限16进制数字和减号）
*/

void DispChar(unsigned char x, unsigned char c )
{
	code unsigned char Tab[] =
	{//定义0123456789AbCdEF的数码管字型数据
		0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
		0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71
	};
	unsigned char t;	//临时变量
//防止显示位置超出范围	
    x&=0x07;
	x = 7-x ;
//分析字符c，取得对应的数码管字型数据
	if ( c == '-' )
	{
		t = 0x40;
	}
	else
	{
		t = toint(c);	//toint()为库函数
		if ( t < 16 )	//如果是16进制字符
		{
			t = Tab[t];	//查表，取得数码管字型数据
		}
		else
		{
			t = 0x00;	//如果是其它字符则显示为空白
		}
	}

//检查是否显示小数点
	if ( x==dp_num )
	{
		t |= 0x80;
	}
	else
	{
		t &= 0x7F;
	}
	
//送到显示缓冲区显示
	DispBuf[x] = t;
}

/*
函数：DispStr()
功能：在数码管上显示字符串
参数：
	x：显示的起始位置（0～7）
	*s：要显示的字符串（内容仅限16进制数字和减号）
*/

void DispStr(unsigned char x, unsigned char idata *s)
{
	unsigned char c;
	for (;;)
	{
		c = *s++;
		if ( c == '\0' ) break;
		if ( ((c=='0') && ((++c)!='0')) )
		{DispChar(x++,'+');continue;} 		//消影，清除无意义的零
		//{x++;continue;}
		DispChar(x++,c);
	}
}

/*
函数：ByteToStr()
功能：字节型变量c转换为十进制字符串
*/

void ByteToStr(unsigned char idata *s, unsigned long c)
{
	code unsigned long Tab[] = {10000000,1000000,100000,10000,1000,100,10};
	unsigned char i;
	unsigned char t;
	for ( i=0; i<7; i++ )
	{
		t = c / Tab[i];
		*s++ = '0' + t;
		c -= t * Tab[i];
	}
	*s++ = '0' + c;
	*s = '\0';
}

void DispInit()
{
	DispClear();	//初始为全灭
	EA = 0;
	TMOD = 0x15;
    TH1 = 0xFA;
	TL1 = 0x24;
	TR1 = 1;
	ET0 = 1;
	EA = 1;
}


void Delay(unsigned int t)
{
	do
	{
		code unsigned char com[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
		static unsigned char n = 0;
		TH1 = 0xFA;
		TL1 = 0x24;
		TR1 = 1;
	    while ( !TF1 );
		TR1 = 0;
		TF1=0;
	    TH1 = 0xFA;
		TL1 = 0x24;
		TR1 = 1;
		XBYTE[0x7800] = 0xFF;		//暂停显示
		XBYTE[0x7801] = ~DispBuf[n];	//更新扫描数据
		XBYTE[0x7800] = ~com[n];	//重新显示
		n++;
	    n&=0x07;	
	} while ( --t != 0 );
}


/*
函数：SysInit()
功能：系统初始化
*/
void SysInit()
{

	TMOD = 0x15;	//设置定时器T1为16位定时器 T0为计数器
	DispInit();		//数码管扫描显示初始化
}




void Gate_select(unsigned char k)
{
	switch ( k )		//判断键值，执行具体功能
		{
			case '1':
			    LED1=0;					//1s 1hz
				LED2=1;
				LED3=1;
				TR0 = 1;
				TH0 = 0x00;
			    TL0 = 0x00;
				Delay(1000);
			  	TR0 = 0;
				break;
	
			case '2':
		    	LED1=1;					//100ms
				LED2=0;
				LED3=1;
				TR0 = 1;
				TH0 = 0x00;
			    TL0 = 0x00;			
			  	Delay(100);			
			    TR0 = 0;
				break;
	
			case '3':
			    LED1=1;					//10ms
				LED2=1;
				LED3=0;
				TR0 = 1;
				TH0 = 0x00;
			    TL0 = 0x00;
				Delay(10);
				TR0 = 0; 
				break;
	
			default:
				break;
		}	
	
}

void Dp_select()
{
	switch ( k )		//判断键值，执行具体功能
		{
			case '1':
				dp_num=3;
				break;
	
			case '2':
				dp_num=2;
				break;
	
			case '3':
				dp_num=1;
				break;
	
			default:
				break;
		}
}

unsigned long Frequency_calculate()
{
	unsigned long T0_final;	//定义T0计数器计算值
	unsigned long Frequency;		//定义频率变量
    T0_final=TH0*256+TL0+T0_count*65536;  //计算脉冲数
	T0_final=T0_final-T0_final/1000*4;	  //误差修正
	//T0_final=T0_final-T0_final/100000;	  //误差修正


	Frequency=T0_final;
	Dp_select();
	
	T0_count=0;		//T0 置零，准备重新开始下一次计数
	return Frequency;
}

void main()
{
	unsigned char idata s[16];
	unsigned long Frequency = 5555;		//定义频率变量
	SysInit();				//系统初始化
	ByteToStr(s,Frequency);
	DispStr(8,s);		//显示初始值
	Delay(200);
	for (;;)
	{
		for (;;)
		{
			Delay(10);		
			k = KeyScan();	//键盘扫描
			if ( k != '\0' ) break;		//如果有键按下，退出循环
		}  

			Gate_select(k);	  //选择闸门
			Frequency = Frequency_calculate();//计算频率
			Dp_select();				
			ByteToStr(s,Frequency);
			DispStr(8,s);	//显示计数器值

		
		for (;;)
		{
			Delay(10);		
			if ( KeyScan() == '\0' ) break;	//如果按键抬起，退出循环
		}
	}		  

}

