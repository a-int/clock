#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#define LCD_DATA_PORT PORTC 
#define LCD_DATA_DDR DDRC
#define LCD_DATA_PIN PINC
#define CS_PORT PORTB //���� cs1/cs2
#define CS_DDR DDRB
#define RSRW_PORT PORTA //���� rs � rw
#define RSRW_DDR DDRA
#define ERST_PORT PORTD //���� E � RST
#define ERST_DDR DDRD
#define RS 2 // ������/�������
#define RW 3 // ������/������
#define EN 6 // �����
#define CS1 0 // ���������� 1
#define CS2 1 // ���������� 2
#define RST 7 // ����� ������

void Time_1_init(); //������������� �������/�������� 1
void Timer0_init();
void mainMenu(void);
void getCurrentTime(void); //������� ��� ��������� �������� �������
void getCurrentDate(void);//������� ��� ��������� ������� ����
void printStartMenu(); //������� ��������� ���������� ����
void printMainMenu(); //������� ��������� �������� ����
void printStopwatchMenu(); //������� ��������� ���� �����������
void printTimerMenu(); //������� ��������� ���� �������
void printSettings(); //������� ��������� ���� ��������

void LCD_Command(char Command);//�������� ������� ���
void LCD_Data(char Data);//�������� ������ ���
void LCD_Init();//������� ������������� ���
void LCD_Clear();//������� ������� �������
void LCD_GotoXY(uint8_t x, uint8_t y);//������� ��������� ��������� ������� �� ������
void LCD_drawByte(uint8_t x, uint8_t y, unsigned char byte);//������ ������� � ���������� x  � y
void LCD_Char(uint8_t  x1, uint8_t y1, char code);//������� ��� ����������� ������� �� ������ � ����������� � � �
void LCD_String(uint8_t x1, uint8_t y1, char *string);//������� ��� ����������� ������ �� ������ � ������� � � �

void init_DS1307(void);  //������������� �����
uint8_t read_DS1307(uint8_t addr);//��������� �������� �� �����

void init_adc(void); //������������� ���

char symbol[][5] =
{ 
	//������������ ������� ���������� � 0�20
	{0x00,0x00,0x00,0x00,0x00},                    //������
	{0x00,0x00,0x4f,0x00,0x00},                    //!
	{0x00,0x07,0x00,0x07,0x00},                    //"
	{0x14,0x7f,0x14,0x7f,0x14},                    //#
	{0x24,0x2a,0x7f,0x2a,0x12},                    //$
	{0x23,0x13,0x08,0x64,0x62},                    //%
	{0x36,0x49,0x55,0x22,0x40},                    //&
	{0x00,0x05,0x03,0x00,0x00},                    //,
	{0x00,0x1c,0x22,0x41,0x00},                    //(
	{0x00,0x41,0x22,0x1c,0x00},                    //)
	{0x14,0x08,0x3E,0x08,0x14},                    //*
	{0x08,0x08,0x3E,0x08,0x08},                    //+
	{0x00,0x50,0x30,0x00,0x00},                    //,
	{0x08,0x08,0x08,0x08,0x08},                    //-
	{0x00,0x60,0x60,0x00,0x00},                    //.
	{0x20,0x10,0x08,0x04,0x02},                    ///
	
	{0x3e,0x51,0x49,0x45,0x3e},                    //0
	{0x00,0x42,0x7f,0x40,0x00},                    //1
	{0x42,0x61,0x51,0x49,0x46},                    //2
	{0x21,0x41,0x45,0x4b,0x31},                    //3
	{0x18,0x14,0x12,0x7f,0x10},                    //4
	{0x27,0x45,0x45,0x45,0x39},                    //5
	{0x3c,0x4a,0x49,0x49,0x30},                    //6
	{0x01,0x71,0x09,0x05,0x03},                    //7
	{0x36,0x49,0x49,0x49,0x36},                    //8
	{0x06,0x49,0x49,0x29,0x1e},                    //9
	{0x00,0x36,0x36,0x00,0x00},                    //:
	{0x00,0x56,0x36,0x00,0x00},                    //;
	{0x08,0x14,0x22,0x41,0x00},                    //<
	{0x14,0x14,0x14,0x14,0x14},                    //=
	{0x00,0x41,0x22,0x14,0x08},                    //>
	{0x02,0x01,0x51,0x09,0x06},                    //?
	
	{0x32,0x49,0x71,0x41,0x3e},                    //@
	{0x7e,0x11,0x11,0x11,0x7e},                    //A
	{0x7f,0x49,0x49,0x49,0x36},                    //B
	{0x3e,0x41,0x41,0x41,0x22},                    //C
	{0x7f,0x41,0x41,0x22,0x1c},                    //D
	{0x7f,0x49,0x49,0x49,0x41},                    //E
	{0x7f,0x09,0x09,0x09,0x01},                    //F
	{0x3e,0x41,0x49,0x49,0x3a},                    //G
	{0x7f,0x08,0x08,0x08,0x7f},                    //H
	{0x00,0x41,0x7f,0x41,0x00},                    //I
	{0x20,0x40,0x41,0x3f,0x01},                    //J
	{0x7f,0x08,0x14,0x22,0x41},                    //K
	{0x7f,0x40,0x40,0x40,0x40},                    //L
	{0x7f,0x02,0x0c,0x02,0x7f},                    //M
	{0x7f,0x04,0x08,0x10,0x7f},                    //N
	{0x3e,0x41,0x41,0x41,0x3e},                    //O
	
	{0x7f,0x09,0x09,0x09,0x06},                    //P
	{0x3e,0x41,0x51,0x21,0x5e},                    //Q
	{0x7f,0x09,0x19,0x29,0x46},                    //R
	{0x46,0x49,0x49,0x49,0x31},                    //S
	{0x01,0x01,0x7f,0x01,0x01},                    //T
	{0x3f,0x40,0x40,0x40,0x3f},                    //U
	{0x1f,0x20,0x40,0x20,0x1f},                    //V
	{0x3f,0x40,0x70,0x40,0x3f},                    //W
	{0x63,0x14,0x08,0x14,0x63},                    //X
	{0x07,0x08,0x70,0x08,0x07},                    //Y
	{0x61,0x51,0x49,0x45,0x43},                    //Z
	{0x00,0x7F,0x41,0x41,0x00},                    //[
	{0x02,0x04,0x08,0x10,0x20},                    // - �������� ����
	{0x00,0x41,0x41,0x7F,0x00},                    //]
	{0x04,0x02,0x01,0x02,0x04},                    //^
	{0x40,0x40,0x40,0x40,0x40},                    //_

	{0x00,0x01,0x02,0x04,0x00},                    //'
	{0x20,0x54,0x54,0x54,0x78},                    //a
	{0x7F,0x48,0x44,0x44,0x38},                    //b
	{0x38,0x44,0x44,0x44,0x28},                    //c
	{0x38,0x44,0x44,0x48,0x7F},                    //d
	{0x38,0x54,0x54,0x54,0x18},                    //e
	{0x08,0x7E,0x09,0x01,0x02},                    //f
	{0x0C,0x52,0x52,0x52,0x3E},                    //g
	{0x7F,0x08,0x04,0x04,0x78},                    //h
	{0x00,0x44,0x7D,0x40,0x00},                    //i
	{0x20,0x40,0x44,0x3D,0x00},                    //j
	{0x7F,0x10,0x28,0x44,0x00},                    //k
	{0x00,0x41,0x7F,0x40,0x00},                    //l
	{0x7C,0x04,0x18,0x04,0x78},                    //m
	{0x7C,0x08,0x04,0x04,0x78},                    //n
	{0x38,0x44,0x44,0x44,0x38},                    //o
	
	{0x7C,0x14,0x14,0x14,0x08},                    //p
	{0x08,0x14,0x14,0x18,0x7C},                    //q
	{0x7C,0x08,0x04,0x04,0x08},                    //r
	{0x48,0x54,0x54,0x54,0x20},                    //s
	{0x04,0x3F,0x44,0x40,0x20},                    //t
	{0x3C,0x40,0x40,0x20,0x7C},                    //u
	{0x1C,0x20,0x40,0x20,0x1C},                    //v
	{0x3C,0x40,0x30,0x40,0x3C},                    //w
	{0x44,0x28,0x10,0x28,0x44},                    //x
	{0x0C,0x50,0x50,0x50,0x3C},                    //y
	{0x44,0x64,0x54,0x4C,0x44},                    //z
	{0x00,0x08,0x36,0x41,0x00},                    //{
	{0x00,0x00,0x7f,0x00,0x00},                    //|
	{0x00,0x41,0x36,0x08,0x00},                    //}
	{0x02,0x01,0x02,0x02,0x01},                    //~
	{0x00,0x00,0x00,0x00,0x00},                    //������
	// 0x7f
	//0xc0 � ����� ������� (0x60-0xa0)
	{0x7e,0x11,0x11,0x11,0x7e},                    //A
	{0x7f,0x49,0x49,0x49,0x33},                    //�
	{0x7f,0x49,0x49,0x49,0x36},                    //�
	{0x7f,0x01,0x01,0x01,0x03},                    //�
	{0xe0,0x51,0x4f,0x41,0xff},                    //�
	{0x7f,0x49,0x49,0x49,0x41},                    //E
	{0x77,0x08,0x7f,0x08,0x77},                    //�
	{0x41,0x49,0x49,0x49,0x36},                    //�
	{0x7f,0x10,0x08,0x04,0x7f},                    //�
	{0x7c,0x21,0x12,0x09,0x7c},                    //�
	{0x7f,0x08,0x14,0x22,0x41},                    //K
	{0x20,0x41,0x3f,0x01,0x7f},                    //�
	{0x7f,0x02,0x0c,0x02,0x7f},                    //M
	{0x7f,0x08,0x08,0x08,0x7f},                    //H
	{0x3e,0x41,0x41,0x41,0x3e},                    //O
	{0x7f,0x01,0x01,0x01,0x7f},                    //�
	
	{0x7f,0x09,0x09,0x09,0x06},                    //P
	{0x3e,0x41,0x41,0x41,0x22},                    //C
	{0x01,0x01,0x7f,0x01,0x01},                    //T
	{0x47,0x28,0x10,0x08,0x07},                    //�
	{0x1c,0x22,0x7f,0x22,0x1c},                    //�
	{0x63,0x14,0x08,0x14,0x63},                    //X
	{0x7f,0x40,0x40,0x40,0xff},                    //�
	{0x07,0x08,0x08,0x08,0x7f},                    //�
	{0x7f,0x40,0x7f,0x40,0x7f},                    //�
	{0x7f,0x40,0x7f,0x40,0xff},                    //�
	{0x01,0x7f,0x48,0x48,0x30},                    //�
	{0x7f,0x48,0x30,0x00,0x7f},                    //�
	{0x00,0x7f,0x48,0x48,0x30},                    //�
	{0x22,0x41,0x49,0x49,0x3e},                    //�
	{0x7f,0x08,0x3e,0x41,0x3e},                    //�
	{0x46,0x29,0x19,0x09,0x7f},                    //�
	
	{0x20,0x54,0x54,0x54,0x78},                    //a
	{0x3c,0x4a,0x4a,0x49,0x31},                    //�
	{0x7c,0x54,0x54,0x28,0x00},                    //�
	{0x7c,0x04,0x04,0x04,0x0c},                    //�
	{0xe0,0x54,0x4c,0x44,0xfc},                    //�
	{0x38,0x54,0x54,0x54,0x18},                    //e
	{0x6c,0x10,0x7c,0x10,0x6c},                    //�
	{0x44,0x44,0x54,0x54,0x28},                    //�
	{0x7c,0x20,0x10,0x08,0x7c},                    //�
	{0x7c,0x41,0x22,0x11,0x7c},                    //�
	{0x7c,0x10,0x28,0x44,0x00},                    //�
	{0x20,0x44,0x3c,0x04,0x7c},                    //�
	{0x7c,0x08,0x10,0x08,0x7c},                    //�
	{0x7c,0x10,0x10,0x10,0x7c},                    //�
	{0x38,0x44,0x44,0x44,0x38},                    //o
	{0x7c,0x04,0x04,0x04,0x7c},                    //�
	
	{0x7C,0x14,0x14,0x14,0x08},                    //p
	{0x38,0x44,0x44,0x44,0x28},                    //c
	{0x04,0x04,0x7c,0x04,0x04},                    //�
	{0x0C,0x50,0x50,0x50,0x3C},                    //�
	{0x30,0x48,0xfc,0x48,0x30},                    //�
	{0x44,0x28,0x10,0x28,0x44},                    //x
	{0x7c,0x40,0x40,0x40,0xfc},                    //�
	{0x0c,0x10,0x10,0x10,0x7c},                    //�
	{0x7c,0x40,0x7c,0x40,0x7c},                    //�
	{0x7c,0x40,0x7c,0x40,0xfc},                    //�
	{0x04,0x7c,0x50,0x50,0x20},                    //�
	{0x7c,0x50,0x50,0x20,0x7c},                    //�
	{0x7c,0x50,0x50,0x20,0x00},                    //�
	{0x28,0x44,0x54,0x54,0x38},                    //�
	{0x7c,0x10,0x38,0x44,0x38},                    //�
	{0x08,0x54,0x34,0x14,0x7c},                    //�
	//0xff
	
};

char settingsImage[] = {0x2A,0x2F,0x2A, 0x2A, 0x7A, 0x2A, 0x2A, 0x3E, 0x2A};
//���������� ���������� �� ����� ������ �  ���� ��������
int scene = 0;
char dataStyle = 0, language = 0, startScreenScene = 0;

//���������� ��� ������� � ����
int hour = 0, minutes = 0, day = 1, weekDay = 0, month = 1, year = 21;

//���������� ����������� � �������
int stopwatchTime[3] = {0,0,0}, isStartedSWT = 0, resetSWT = 0;
int timerTime[3] = {0,0,0}, isStartedTMR = 0, setTMR = 1;

//���������� ��� ���� ������ �� ������� � ���������� ������
char *DaysRU[7] = {"�����������", "�����������", "�������", "�����", "�������", "�������", "�������"};
char *DaysENG[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//���������� ��� ������� �� ������� � ���������� ������
char *monthsRU[12] = {"������", "�������", "�����", "������", "���", "����", "����", "�������", "��������", "�������", "������", "�������"};
char *monthsENG[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

volatile unsigned char sec_flag=0;
volatile unsigned int ms_counter=0;


uint16_t temp_code_ADC;
volatile unsigned long long int code_ADC;
volatile unsigned char temp_ADC;
uint16_t x,y;
int main(void)
{
	//������������� ���, ������-�������� 1, ��� � ������� ����� ��������� ������� DS1307
	LCD_Init();
	Time_1_init();
	init_adc();
	Timer0_init();
	init_DS1307();
	LCD_Clear();
	//��������������� �������, ��� ���������� ����������
	asm("sei");
	while (1)
	{
		mainMenu();
	}
}

//��������� ������-�������� 1
void Time_1_init()
{
	TCNT1=0x0000;
	OCR1A=31250;
	// ��������� ����� WGM10 � WGM11 ��� ������ ������ ���
	TCCR1A &= ~((1<<WGM11)|(1<<WGM10));
	// ��������� 1 � ��� WGM12 ��� ��������� ������ ��� � ��������� �������� ������������ � �������� 256
	TCCR1B |= (1<<WGM12)|(1<<CS12);
	//���� ��������� �������� TCNT1 � ��������� OCR1A
	TIFR |= (1<<OCF1A); 
	//���������� ���������� �� ��������� � ��������� OCR1A
	TIMSK |= (1<<OCIE1A); 
}
//������� ���������� ������-�������� 1
ISR (TIMER1_COMPA_vect)
{
	//����������� ������ ������� ���������� stopwatchTime[0]
	if(isStartedSWT) stopwatchTime[0]++; 
	//��������� ������ ������� ���������� timerTime[0]
	if(isStartedTMR) timerTime[0]--; 
	/*if(scene == 1) 
	{
		//�������� ������� �����
		getCurrentTime(); 
		//�������� �������� ����
		getCurrentDate(); 
	}
	*/
}

//������������� ���
void init_adc(void)
{
	DDRA|=0b00001100;
	PORTA=0b00000000;
	ADMUX=0b00000000;
	ADCSRA=(0b10000111|0b01000000);
}

void Timer0_init()
{
	TCNT0=0b00000000;
	OCR0=249;
	TCCR0=0b00001100;
	TIMSK|=0b00000010;
	TIFR|=0b00000011;
}
ISR (TIMER0_COMP_vect)
{
	ms_counter++;
	if(ms_counter==10)
	{
		ms_counter=0;
		sec_flag=1;
		
		PORTA=0b00000100;
		_delay_ms(2);
		ADMUX=0b00000000;
		ADCSRA=0b11000111;
		while((ADCSRA&0b01000000)!=0){}
		x=ADCL|(ADCH<<8);
		
		//////
		PORTA=0b00001000;
		_delay_ms(2);
		ADMUX=0b00000001;
		ADCSRA=0b11000111 ;
		while((ADCSRA&0b01000000)!=0){}
		y=ADCL|(ADCH<<8);
		
	}
		if (scene == 0) //�������� ������� ������ ��� ���������� ����
		{
			if (startScreenScene == 0)
			{
				if((x>=120)&&(x<=340)&&(y>=380)&&(y<=480))
				{
					language = 0; //������ ������� ����
					startScreenScene = 1;
					LCD_Clear();	
				}
				else if((x>=530)&&(x<=760)&&(y>=380)&&(y<=480))
				{
					language = 1; //������ ���������� ����
					startScreenScene = 1;
				}
			}
			if (startScreenScene == 1)
			{
				if ((x>=80)&&(x<=130)&&(y>=680)&&(y<=730)) if(hour < 23) hour++;	//������ ������ ���� ��� �����
				else if ((x>=78)&&(x<=150)&&(y>=310)&&(y<=370)) if (hour>0) hour--; //������ ������ ����� ��� �����
				
				if ((x>=190)&&(x<=260)&&(y>=640)&&(y<=720)) if(minutes <59 ) minutes++;	//������ ������ ���� ��� �����
				else if ((x>=185)&&(x<=275)&&(y>=320)&&(y<=390)) if (minutes>0) minutes--;//������ ������ ����� ��� �����
				
				if ((x>=410)&&(x<=510)&&(y>=635)&&(y<=710)) //������ ������ ���� ��� ���
				{
					if ((month == 1) && (month == 3) && (month == 5) && (month == 7) && (month == 8) && (month == 10) && (month == 12) && day<31)
					{
						day++;	
					}
					else if( month == 1 && month % 4 == 0 && day<29) day++;
					else if( month == 1 && month % 4 != 0 && day<28) day++;
					else if(day<30) day++;
				}
				else if ((x>=420)&&(x<=520)&&(y>=300)&&(y<=360)) if (day>1) day--;	//������ ������ ����� ��� ���
				
				if ((x>=550)&&(x<=650)&&(y>=615)&&(y<=690)) if(month < 12) month++;	//������ ������ ���� ��� ������
				if ((x>=570)&&(x<=670)&&(y>=260)&&(y<=400)) if (month>1) month--;//������ ������ ����� ��� ������
				
				if ((x>=700)&&(x<=790)&&(y>=600)&&(y<=670)) year++;	//������ ������ ���� ��� ����
				if ((x>=730)&&(x<=815)&&(y>=300)&&(y<=360)) if (year>0) year--;//������ ������ ����� ��� ����
				
				if ((x>=505)&&(x<=556)&&(y>=132)&&(y<=215)) if(weekDay<7) weekDay++; //������ ������ + ��� ������
				if ((x>=72)&&(x<=106)&&(y>=140)&&(y<=215)) if(weekDay>0) weekDay--; 
				
				if ((x>=725)&&(x<=825)&&(y>=120)&&(y<=205)) //������ ������ OK
				{
					//������ ������������� ������ � ������ ����� ��������� ������� DS1307
					/*
					write_DS1307(0x00, 0);
					_delay_us(50);
					write_DS1307(0x01, minutes);
					_delay_us(50);
					write_DS1307(0x02, hour);
					_delay_us(50);
					write_DS1307(0x03, weekDay);
					_delay_us(50);
					write_DS1307(0x04, day);
					_delay_us(50);
					write_DS1307(0x05, month);
					_delay_ms(3);
					//������� �� ����� �������� ����
					*/
					scene = 1;
				}
			}
		}
		else if (scene == 1)	//�������� ������� ������ ��� �������� ����
		{
			if ((x>=725)&&(x<=800)&&(y>=670)&&(y<=730)) scene = 4; //������ ������ ���������
			else if ((x>=100)&&(x<=440)&&(y>=190)&&(y<=305)) scene = 2; //������ ������ �����������
			if ((x>=530)&&(x<=750)&&(y>=190)&&(y<=290)) scene = 3;	//������ ������ �������
		}
		else if (scene == 2) //�������� ������� ������ ��� �����������
		{
			if ((x>=75)&&(x<=210)&&(y>=718)&&(y<=790)) scene = 1;			//������ ������ �����
			if ((x>=430)&&(x<=610)&&(y>=275)&&(y<=375)) isStartedSWT = 1;	//������ ������ �����
			if ((x>=195)&&(x<=360)&&(y>=290)&&(y<=390)) isStartedSWT = 0;	//������ ������ ����
			if ((x>=305)&&(x<=510)&&(y>=120)&&(y<=220))						//������ ������ �����
			{
				isStartedSWT = 0;
				stopwatchTime[0] = 0;
				stopwatchTime[1] = 0;
				stopwatchTime[2] = 0;
			}
		}
		else if (scene == 3) //�������� ������� ������ ��� �������
		{
			if (!isStartedTMR)
			{
				if ((x>=75)&&(x<=210)&&(y>=718)&&(y<=790)) 
				{
					scene = 1;	//������ ������ �����

				}
				
				
				else if ((x>=430)&&(x<=570)&&(y>=680)&&(y<=760)) timerTime[0]++;	//������ ������ +1 ������
				else if ((x>=590)&&(x<=770)&&(y>=670)&&(y<=740)) timerTime[0]+=10;	//������ ������ +10 ������
				else if ((x>=430)&&(x<=580)&&(y>=605)&&(y<=700)) timerTime[1]++;	//������ ������ +1 ���
				else if ((x>=605)&&(x<=775)&&(y>=580)&&(y<=690)) timerTime[1]+=10;	//������ ������ +10 �����
				else if ((x>=450)&&(x<=590)&&(y>=525)&&(y<=620)) timerTime[2]++;	//������ ������ +1 ���
				else if ((x>=620)&&(x<=770)&&(y>=500)&&(y<=600)) timerTime[2]+=5;	//������ ������ +5 �����
				
				else if ((x>=445)&&(x<=600)&&(y>=360)&&(y<=470)) timerTime[0]--;	//������ ������ -1 ������
				else if ((x>=600)&&(x<=790)&&(y>=350)&&(y<=450)) timerTime[0]-=10;	//������ ������ -10 ������
				else if ((x>=450)&&(x<=600)&&(y>=270)&&(y<=370)) timerTime[1]--;	//������ ������ -1 ���
				else if ((x>=600)&&(x<=790)&&(y>=260)&&(y<=360)) timerTime[1]-=10;	//������ ������ -10 �����
				else if ((x>=450)&&(x<=600)&&(y>=190)&&(y<=290)) timerTime[2]--;	//������ ������ -1 ���
				else if ((x>=600)&&(x<=790)&&(y>=170)&&(y<=270)) timerTime[2]-=5;	//������ ������ -5 �����
				
				else if ((x>=100)&&(x<=325)&&(y>=265)&&(y<=415))	//������ ������ �����
				{
					setTMR = 0;
					isStartedTMR = 1;
				}
			}
			else if ((x>=300)&&(x<=550)&&(y>=190)&&(y<=330)) //������ ������ �����
			{
				setTMR = 1;
				isStartedTMR = 0;
			}
			
		}
		else if (scene == 4) //�������� ������� ������ ��� ��������
		{
			if ((x>=75)&&(x<=210)&&(y>=718)&&(y<=790)) scene = 1;	//������ ������ �����
			else if ((x>=155)&&(x<=360)&&(y>=475)&&(y<=535)) dataStyle = 0;	//������ ������ 1 ���
			else if ((x>=155)&&(x<=360)&&(y>=380)&&(y<=475)) dataStyle = 1;	//������ ������ 01.05
			else if ((x>=155)&&(x<=360)&&(y>=280)&&(y<=380)) dataStyle = 2;	//������ ������ 05.01
			else if ((x>=590)&&(x<=680)&&(y>=130)&&(y<=215)) language = 0;	//������ ������ ��
			else if ((x>=710)&&(x<=825)&&(y>=130)&&(y<=215)) language = 1;	//������ ������ ��
		}
}


//������� ������ �������� ����
void mainMenu()
{
	if (scene == 0)		//��������� ����
	{
		
		printStartMenu();
	}
	else if(scene == 1) //������� ����
	{
		
		printMainMenu();
	}
	else if (scene == 2) //���� �����������
	{
		printStopwatchMenu();
	}
	else if(scene == 3) //���� �������
	{
		printTimerMenu();
	}
	else if(scene == 4) //���� ��������
	{
		printSettings();
	}
}

//������� ��������� ���������� ����
void printStartMenu()
{	
	//��������� ���������� ���� �� ������ �����
	if (startScreenScene == 0)
	{
		LCD_String(4, 10, "�������");
		LCD_String(4, 78, "English");
	}
	//��������� ���������� ���� ����� ������ �����
	if (startScreenScene == 1)
	{
		//����������� ������ ����������� � ��������� ������� ��� ��������� �������
		LCD_Char(1,4, '+');
		LCD_Char(5,4, '-');
		LCD_Char(1,26, '+');
		LCD_Char(5,26, '-');
		LCD_Char(3,0, hour/10 + '0');
		LCD_Char(3,8, hour%10 + '0');
		LCD_Char(3,16, ':');
		LCD_Char(3,24, minutes/10 + '0');
		LCD_Char(3,32, minutes%10 + '0');
		
		//����������� ������ ����������� � ��������� ��� ��������� ����
		LCD_Char(1,67, '+');
		LCD_Char(5,67, '-');
		LCD_Char(1,91, '+');
		LCD_Char(5,91, '-');
		LCD_Char(1, 115, '+');
		LCD_Char(5,115, '-');
		LCD_Char(3,63, day/10 + '0');
		LCD_Char(3,71, day%10 + '0');
		LCD_Char(3,79, '.');
		LCD_Char(3,87, month/10 + '0');
		LCD_Char(3,95, month%10 + '0');
		LCD_Char(3,103, '.');
		LCD_Char(3,111, year/10 + '0');
		LCD_Char(3,119, year%10 + '0');
		
		//����������� ��� ������ � ������ ����� ��� ������ �� ������� �����
		if (!language) 
		{
			LCD_Char(7, 0, '-');
			LCD_String(7,8, DaysRU[weekDay]);
			LCD_Char(7, strlen(DaysRU[weekDay])*6+10, '+');
		}
		//����������� ��� ������ � ������ ����� ��� ������ �� ���������� �����
		else
		{
			LCD_Char(7, 0, '-');
			LCD_String(7,8, DaysENG[weekDay]);
			LCD_Char(7, strlen(DaysENG[weekDay])*6+10, '+');
		}
		//����������� ������ ��������� ��������� ��������
		LCD_String(7, 114, "OK");
	}
	
}

//������� ��������� �������� �������
void getCurrentTime()
{
	minutes = read_DS1307(0x01);
	hour = read_DS1307(0x02);
}

//������� ��������� ������� ����, ��� ������ � ������
void getCurrentDate()
{
	weekDay = read_DS1307(0x03);
	day = read_DS1307(0x04);
	month = read_DS1307(0x05);
}
//������� ��������� �������� ����
void printMainMenu()
{
	//����� �������
	LCD_Char(3,42, hour/10 + '0');
	LCD_Char(3,51, hour%10 + '0');
	LCD_Char(3,60, ':');
	LCD_Char(3,68, minutes/10 + '0');
	LCD_Char(3,76, minutes%10 + '0');
	
	//����� ���� � ����� "1 ���"/"May 1st"
	if(dataStyle == 0)
	{
		//����� ���� �� ������� �����
		if(!language)
		{
			LCD_Char(0,0, day/10+'0');
			LCD_Char(0,6, day%10+'0');
			LCD_String(0,12, monthsRU[month-1]);
			LCD_String(1,0,DaysRU[weekDay]);
		}
		
		//����� ���� �� ���������� �����
		else
		{
			LCD_String(0,0, monthsENG[month-1]);
			LCD_Char(0,strlen(monthsENG[month-1])*6+3, day/10+'0');
			LCD_Char(0,strlen(monthsENG[month-1])*6 + 9, day%10+'0');
			LCD_String(1,0,DaysENG[weekDay-1]);
			
			//�������� ������������ ��������� ��� ����
			if ((day%10 == 1) && (day != 11))
			{
				LCD_Char(0, strlen(monthsENG[month-1])*6 + 15, 's');
				LCD_Char(0, strlen(monthsENG[month-1])*6 + 21, 't');
			}
			else if ((day%10 == 2) && (day != 12))
			{
				LCD_Char(0, strlen(monthsENG[month-1])*6 + 15, 'n');
				LCD_Char(0, strlen(monthsENG[month-1])*6 + 21, 'd');
			}
			else if((day%10 == 3) && (day != 13))
			{
				LCD_Char(0, strlen(monthsENG[month-1])*6 + 15, 'r');
				LCD_Char(0, strlen(monthsENG[month-1])*6 + 21, 'd');
			}
			else
			{
				LCD_Char(0, strlen(monthsENG[month-1])*6 + 15, 't');
				LCD_Char(0, strlen(monthsENG[month-1])*6 + 21, 'h');
			}
		}
	}
	//����� ���� � ����� "01.05"
	else if(dataStyle == 1)
	{
		LCD_Char(1,0, day/10+'0');
		LCD_Char(1,6, day%10+'0');
		LCD_Char(1,12,'.');
		LCD_Char(1,18, month/10+'0');
		LCD_Char(1,24, month%10+'0');
		if(!language) 	LCD_String(0,0,DaysRU[weekDay]);
		else LCD_String(0,0,DaysENG[weekDay]);
	}
	//����� ���� � ����� "05.01"
	else if (dataStyle == 2)
	{
		LCD_Char(1,0, month/10+'0');
		LCD_Char(1,6, month%10+'0');
		LCD_Char(1,12,'.');
		LCD_Char(1,18, day/10+'0');
		LCD_Char(1,24, day%10+'0');
		if(!language) 	LCD_String(0,0,DaysRU[weekDay]);
		else LCD_String(0,0,DaysENG[weekDay]);
	}
	//����� ������ �������� ����������� � ������� � ������������ � �����
	if (!language)
	{
		LCD_String(6,8,"����������"); //����� ������ �����������
		LCD_String(6,80,"������"); //����� ������ �������
	}
	else
	{
		LCD_String(6,8,"Stopwatch"); //����� ������ �����������
		LCD_String(6,80,"Timer"); //����� ������ �������
	}
	//����� ����� ��������
	for (int i = 0; i<10; i++)
	{
		LCD_drawByte(0, 117+i, settingsImage[i]);
	}
}

//������� ��������� ���� �����������
void printStopwatchMenu()
{
	//�������� ����������� ������� �����������
	if (stopwatchTime[0] == 60)
	{
		stopwatchTime[0] = 0;
		stopwatchTime[1]++;
		if(stopwatchTime[1] == 60)
		{
			stopwatchTime[1] = 0;
			stopwatchTime[2]++;
		}
	}
	//��������� ������� �����������
	LCD_Char(3,75,stopwatchTime[0]/10+'0');
	LCD_Char(3,82,stopwatchTime[0]%10+'0');
	LCD_Char(3,68,':');
	LCD_Char(3,54,stopwatchTime[1]/10+'0');
	LCD_Char(3,61,stopwatchTime[1]%10+'0');
	LCD_Char(3,33,stopwatchTime[2]/10+'0');
	LCD_Char(3,40,stopwatchTime[2]%10+'0');
	LCD_Char(3,47,':');
	//��������� ����������� ������ �����, ����� � ���� �� ������� �����
	if (!language)
	{
		LCD_String(0,0,"�����");
		LCD_String(5,27,"����");
		LCD_String(5,64,"�����");
		if (isStartedSWT)
		{
			LCD_String(7,45, "�����");
		}
	}
	//��������� ����������� ������ �����, ����� � ���� �� ���������� �����
	else
	{
		LCD_String(0,0,"back");
		LCD_String(5,24,"Stop");
		LCD_String(5,64,"Start");
		if (isStartedSWT)
		{
			LCD_String(7,45, "Reset");
		}
	}
	
}

//������� ��������� ���� �������
void printTimerMenu()
{
	//�������� ������������ ������� �������
	if (timerTime[0] == -1)
	{
		timerTime[0] = 59;
		timerTime[1]--;
		if(timerTime[1] == -1)
		{
			timerTime[1] = 59;
			timerTime[2]--;
		}
	}
	
	//�������� ������� ��� ��������� �������
	if (isStartedTMR && (timerTime[2] == 0) && (timerTime[1] == 0) && (timerTime[0] == 0))
	{
		LCD_Clear();
		isStartedTMR = 0;
		timerTime[0] = 0;
		timerTime[1] = 0;
		timerTime[2] = 0;
	}
	
	//��������� ���� �������
	if (setTMR)
	{
		//��������� ���� ������� �� ������� �� ������� �����
		if (!language)
		{
			LCD_String(0,0,"�����");
			LCD_Char(3,2, timerTime[2]/10+'0');
			LCD_Char(3,9,timerTime[2]%10+'0');
			LCD_Char(3,16,':');
			LCD_Char(3,23, timerTime[1]/10+'0');
			LCD_Char(3,30,timerTime[1]%10+'0');
			LCD_Char(3,37,':');
			LCD_Char(3,44, timerTime[0]/10+'0');
			LCD_Char(3,51,timerTime[0]%10+'0');
			
			LCD_String(0,69,"+1�");
			LCD_String(0,95,"+10�");
			LCD_String(1,69,"+1�");
			LCD_String(1,95,"+10�");
			LCD_String(2,69,"+1�");
			LCD_String(2,101,"+5�");
			
			LCD_String(4,69,"-1�");
			LCD_String(4,95,"-10�");
			LCD_String(5,69,"-1�");
			LCD_String(5,95,"-10�");
			LCD_String(6,69,"-1�");
			LCD_String(6,101,"-5�");
			
			LCD_String(5,13,"�����");
		}
		//��������� ���� ������� �� ������� �� ���������� �����
		else
		{
			LCD_String(0,0,"back");
			LCD_Char(3,2, timerTime[2]/10+'0');
			LCD_Char(3,9,timerTime[2]%10+'0');
			LCD_Char(3,16,':');
			LCD_Char(3,23, timerTime[1]/10+'0');
			LCD_Char(3,30,timerTime[1]%10+'0');
			LCD_Char(3,37,':');
			LCD_Char(3,44, timerTime[0]/10+'0');
			LCD_Char(3,51,timerTime[0]%10+'0');
			
			LCD_String(0,69,"+1s");
			LCD_String(0,95,"+10s");
			LCD_String(1,69,"+1m");
			LCD_String(1,95,"+10m");
			LCD_String(2,69,"+1h");
			LCD_String(2,101,"+5h");
			
			LCD_String(4,69,"-1s");
			LCD_String(4,95,"-10s");
			LCD_String(5,69,"-1m");
			LCD_String(5,95,"-10m");
			LCD_String(6,69,"-1h");
			LCD_String(6,101,"-5h");
			
			LCD_String(5,13,"Start");
		}
	}
	else
	{
		LCD_Char(3,75,timerTime[0]/10+'0');
		LCD_Char(3,82,timerTime[0]%10+'0');
		LCD_Char(3,68,':');
		LCD_Char(3,54,timerTime[1]/10+'0');
		LCD_Char(3,61,timerTime[1]%10+'0');
		LCD_Char(3,33,timerTime[2]/10+'0');
		LCD_Char(3,40,timerTime[2]%10+'0');
		LCD_Char(3,47,':');
		
		if (!language) LCD_String(6,45,"�����");
		else LCD_String(6,45,"Reset");
	}
}
//������� ��������� ���� ��������
void printSettings()
{
	if (!language)
	{
		LCD_String(0,0,"�����");
		
		LCD_String(2,0,"����� ����:");
		LCD_String(3,10, "- 1 ���");
		LCD_String(4,10, "- 01.05");
		LCD_String(5,10, "- 05.01");
		
		LCD_String(7,0, "���� �������");
		LCD_String(7, 92, "��");
		LCD_String(7, 110, "ENG");
	}
	else
	{
		LCD_String(0,0,"back");
		
		LCD_String(2,0,"Data style:");
		LCD_String(3,10, "- May 1st");
		LCD_String(4,10, "- 01.05");
		LCD_String(5,10, "- 05.01");
		
		LCD_String(7,0, "System");
		LCD_String(7, 92, "��");
		LCD_String(7, 110, "ENG");
	}
}


void trigger()
{
	ERST_PORT |= (1<<EN);
	_delay_us(50);
	ERST_PORT &= ~(1<<EN);
	_delay_us(50);
}
//������� ��� �������� ������
void LCD_Command(char Command)
{
	//���������� � �������� RS ��� 0 ��� ��������� �������� �������
	RSRW_PORT &= ~(1 << RS);		
	//��������� ��������� ������ �������� ��� RW � 1	
	RSRW_PORT &= ~(1 << RW);
	//�������� ������� � ���� ��� ������ ���			
	LCD_DATA_PORT = Command;	
	//���������� ��� EN � 1 ��� ������������ ��������� ������ �������� �������	
	trigger(); 
}
//������� ��� �������� ������
void LCD_Data(char Data)
{
	//���������� ��� RS � 1 ��� ������������ ��������� �������� ������
	RSRW_PORT |=  (1 << RS);	
	//���������� ��� RW � 0 ��� ������������ ��������� ������ � ���	
	RSRW_PORT &= ~(1 << RW);	
	//�������� ������ � ���� ������ ���	
	LCD_DATA_PORT = Data;		
	trigger();
}

//������� ������������ �������
void LCD_Init()
{
	//����������� ��� ����� �� �����
	LCD_DATA_DDR = 0xFF;
	RSRW_DDR |= (1<<RS)|(1<<RW);
	ERST_DDR |= (1<<EN)|(1<<RST);
	CS_DDR |= (1<<CS1)|(1<<CS2);
	//�������� ��� �����������
	CS_PORT &= ~((1 << CS1) | (1 << CS2)); 
	ERST_PORT |= (1 << RST);
	_delay_us(20);
	//�������� ������� ����
	LCD_Command(0x3E);	
	//��������� ��������� �� ��� Y=0	
	LCD_Command(0x40);	
	//��������� ��������� �� ��� X=0	
	LCD_Command(0xB8);	
	//��������� ��������� �� ��� Z=0	
	LCD_Command(0xC0);
	//�������� ������� ���		
	LCD_Command(0x3F);		
}

//������� ������� �������
void LCD_Clear()
{
	//�������� ������ �������� �������
	CS_PORT &= ~(1 << CS1);
	CS_PORT |= (1 << CS2);
	for(int i = 0; i < 8; i++)
	{
		
		LCD_Command((0xB8) + i);
		for(int j = 0; j < 64; j++)
		{
			LCD_Data(0); //������� ������ ������� ������ �������� �������
		}
	}
	//�������� ������ �������� �������
	CS_PORT |= (1 << CS1);
	CS_PORT &= ~(1 << CS2);
	for(int i = 0; i < 8; i++)
	{
		
		LCD_Command((0xB8) + i);
		for(int j = 0; j < 64; j++)
		{
			LCD_Data(0);//������� ������ ������� ������ �������� �������
		}
	}
	//��������� ������� � ������� (0;0)
	LCD_Command(0x40);
	LCD_Command(0xB8);
}

// ���������� ������ � ������� x, y
void LCD_GotoXY(uint8_t x, uint8_t y)
{
	//���������� ����������� �� ������� ��������� ���� ����������
	if(y<64)
	{
		//����� ������ ����������� ��� ����������� ������
		CS_PORT &= ~(1<<CS1);
		CS_PORT |= (1<<CS2);
	}
	else
	{
		//����� ������� �����������, ��� ����������� ������
		CS_PORT &= ~(1<<CS2);
		CS_PORT |= 1<<CS1;
		y=y-64;
	}
	//��������� ������� � ��������� ��������� �� ��� �
	LCD_Command(0xB8+x);
	//��������� ������� � ��������� ��������� �� ��� Y
	LCD_Command(0x40+y);
	//�������� ������� ���
	LCD_Command(0x3F);
	LCD_Command(0xC0);
}

//������� ����������� ����� ������ �� ������ � ������� X � Y
void LCD_drawByte(uint8_t x, uint8_t y, unsigned char byte)
{
	LCD_GotoXY(x,y);
	LCD_Data(byte);
}

//������� ��� ����������� ������� �� ������ � ����������� � � �
void LCD_Char(uint8_t  x1, uint8_t y1, char code)
{   //�������������� ���� ������� � ������������ � ���������������� ���������������� ���������
	if((code >= 0x20) && (code < 0x80)) code -= 32;
	else  code -= 96;
	
	for(uint8_t i = 0; i < 5; i++)
	{
		LCD_drawByte(x1, y1 + i, symbol[code][i]);
	}
}

//������� ��� ����������� ������ �� ������ � ������� � � �
void LCD_String(uint8_t x1, uint8_t y1, char *string)
{
	while(*string)
	{
		//������������ ����������� ������� ������� ������
		LCD_Char(x1, y1, *string++);
		y1+=6;
	}
}

//������� ������������� ������� ����� ��������� �������
void init_DS1307(void)
{
	TWBR = 32;
	TWSR = (0 << TWPS1)|(0 << TWPS0);
}

//������� ������ ������ �� DS1307
uint8_t read_DS1307(uint8_t addr) //�������� ����� ��������
{
	uint8_t time;
	TWCR |= (1<<TWEN);
	//��������� ��������� �����
	while(!(TWCR&(1<<TWEN)));
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	//�������� ����� � ��� ���������� ����� ������
	TWDR = 0xd0; 
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	// ������������� ����������� ����� ��� ������ ������
	TWDR = addr; 
	TWCR = (1<<TWINT)|(1<<TWEN);
	//��������� �������� ������ ������, ����� ������� ������ � ������������ ��������
	while (!(TWCR & (1<<TWINT)));	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	//�������� ����� � ���, ���������� ����� ������
	TWDR = 0xd1; 
	TWCR = (1<<TWINT)|(1<<TWEN);
	//��������� ������
	while (!(TWCR & (1<<TWINT)));	
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	time = TWDR;
	time = (((time & 0xF0) >> 4)*10)+(time & 0x0F);
	//��������� ��������� ����
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
	while(TWCR&(1<<TWSTO));
	//��������� ���� TWI, ��� ���������� ������ ������ � ������ �
	TWCR &= (1<<TWEN); 
	_delay_us(50);
	TWCR &= (1<<TWINT);
	return time;
}

//������� ������ ������ � DS1307
void write_DS1307 (uint8_t reg, uint8_t time)
{
	//��������� ��������� �����
	TWCR |= (1<<TWEN);
	while(!(TWCR&(1<<TWEN)));
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	//�������� ����� � ��� ���������� ����� ������
	TWDR = 0xd0; 
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	// ������������� ����������� ����� ��� ������ ������
	TWDR = reg;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	time = ((time/10)<<4) + time%10;
	//���������� ������
	TWDR = time; 
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
	while(TWCR&(1<<TWSTO));
	//��������� ���� TWI, ��� ���������� ������ ������ � ������ �
	TWCR &= (1<<TWEN); 
	_delay_us(50);
	TWCR &= (1<<TWINT);
}