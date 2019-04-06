#include <reg51.h>
#include <absacc.h>
#include <ctype.h>


//���尴��
sbit KEY2 = P2^0;		//ѡ��1��λ
sbit KEY3 = P2^1;		//ѡ��2��λ
sbit KEY4 = P2^2;		//ѡ��3��λ

sbit LED1 = P0^0;
sbit LED2 = P0^1;
sbit LED3 = P0^2;

//������ʾ���������ɶ�ʱ�жϳ����Զ�ɨ�裩
unsigned char DispBuf[8];
unsigned char T0_count=0;	  //������T0�������
unsigned char key_num=2;		  //բ�����
unsigned char dp_num=0;		  //С����λ��
unsigned char k;		//�����ֵ����
//extern unsigned char num_long=0;

unsigned char KeyScan()			//����ɨ��
{
	unsigned char k = '\0';
	if ( KEY2== 0 )	 k='1';
	if ( KEY3== 0 )	 k='2';
	if ( KEY4== 0 )	 k='3';
	return k;
}

/*
void reset()				//��λ
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
������DispChar()
���ܣ������������ʾ�ַ�
������
	x������ܵ�����λ�ã�0��7��
	c��Ҫ��ʾ���ַ�������16�������ֺͼ��ţ�
*/

void DispChar(unsigned char x, unsigned char c )
{
	code unsigned char Tab[] =
	{//����0123456789AbCdEF���������������
		0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
		0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71
	};
	unsigned char t;	//��ʱ����
//��ֹ��ʾλ�ó�����Χ	
    x&=0x07;
	x = 7-x ;
//�����ַ�c��ȡ�ö�Ӧ���������������
	if ( c == '-' )
	{
		t = 0x40;
	}
	else
	{
		t = toint(c);	//toint()Ϊ�⺯��
		if ( t < 16 )	//�����16�����ַ�
		{
			t = Tab[t];	//���ȡ���������������
		}
		else
		{
			t = 0x00;	//����������ַ�����ʾΪ�հ�
		}
	}

//����Ƿ���ʾС����
	if ( x==dp_num )
	{
		t |= 0x80;
	}
	else
	{
		t &= 0x7F;
	}
	
//�͵���ʾ��������ʾ
	DispBuf[x] = t;
}

/*
������DispStr()
���ܣ������������ʾ�ַ���
������
	x����ʾ����ʼλ�ã�0��7��
	*s��Ҫ��ʾ���ַ��������ݽ���16�������ֺͼ��ţ�
*/

void DispStr(unsigned char x, unsigned char idata *s)
{
	unsigned char c;
	for (;;)
	{
		c = *s++;
		if ( c == '\0' ) break;
		if ( ((c=='0') && ((++c)!='0')) )
		{DispChar(x++,'+');continue;} 		//��Ӱ��������������
		//{x++;continue;}
		DispChar(x++,c);
	}
}

/*
������ByteToStr()
���ܣ��ֽ��ͱ���cת��Ϊʮ�����ַ���
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
	DispClear();	//��ʼΪȫ��
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
		XBYTE[0x7800] = 0xFF;		//��ͣ��ʾ
		XBYTE[0x7801] = ~DispBuf[n];	//����ɨ������
		XBYTE[0x7800] = ~com[n];	//������ʾ
		n++;
	    n&=0x07;	
	} while ( --t != 0 );
}


/*
������SysInit()
���ܣ�ϵͳ��ʼ��
*/
void SysInit()
{

	TMOD = 0x15;	//���ö�ʱ��T1Ϊ16λ��ʱ�� T0Ϊ������
	DispInit();		//�����ɨ����ʾ��ʼ��
}




void Gate_select(unsigned char k)
{
	switch ( k )		//�жϼ�ֵ��ִ�о��幦��
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
	switch ( k )		//�жϼ�ֵ��ִ�о��幦��
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
	unsigned long T0_final;	//����T0����������ֵ
	unsigned long Frequency;		//����Ƶ�ʱ���
    T0_final=TH0*256+TL0+T0_count*65536;  //����������
	T0_final=T0_final-T0_final/1000*4;	  //�������
	//T0_final=T0_final-T0_final/100000;	  //�������


	Frequency=T0_final;
	Dp_select();
	
	T0_count=0;		//T0 ���㣬׼�����¿�ʼ��һ�μ���
	return Frequency;
}

void main()
{
	unsigned char idata s[16];
	unsigned long Frequency = 5555;		//����Ƶ�ʱ���
	SysInit();				//ϵͳ��ʼ��
	ByteToStr(s,Frequency);
	DispStr(8,s);		//��ʾ��ʼֵ
	Delay(200);
	for (;;)
	{
		for (;;)
		{
			Delay(10);		
			k = KeyScan();	//����ɨ��
			if ( k != '\0' ) break;		//����м����£��˳�ѭ��
		}  

			Gate_select(k);	  //ѡ��բ��
			Frequency = Frequency_calculate();//����Ƶ��
			Dp_select();				
			ByteToStr(s,Frequency);
			DispStr(8,s);	//��ʾ������ֵ

		
		for (;;)
		{
			Delay(10);		
			if ( KeyScan() == '\0' ) break;	//�������̧���˳�ѭ��
		}
	}		  

}

