#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <lcd.h>
#include <ds1307.h>

void Time_1_init(); //Initializating the timer-counter 1
void Timer0_init();//Initializating the timer-counter 0
void mainMenu(void);
void getCurrentTime(void); //Getting the current time
void getCurrentDate(void);//Getting the current date
void printStartMenu(); //Displaying the start menu
void printMainMenu(); //Displaying the main menu
void printStopwatchMenu(); //Displaying the stopwatch menu
void printTimerMenu(); //Displaying the timer menu
void printSettings(); //Displaying the settings

void init_adc(void); //Initializating of the ADC

char settingsImage[] = {0x2A,0x2F,0x2A, 0x2A, 0x7A, 0x2A, 0x2A, 0x3E, 0x2A};//Bytes to display settings icon

//Servise variables
int scene = 0, isSceneChanged = 0;
char dataStyle = 0, language = 0, startScreenScene = 0;

//Date and time variables
int hour = 0, minutes = 0, day = 1, weekDay = 0, month = 1, year = 21;

//Stopwatch's and Timer's variables
int stopwatchTime[3] = {0,0,0}, isStartedSWT = 0, resetSWT = 0;
int timerTime[3] = {0,0,0}, isStartedTMR = 0, setTMR = 1;

//Days of the week in Russian and English
char *DaysRU[7] = {"воскресенье", "понедельник", "вторник", "среда", "четверг", "пятница", "суббота"};
char *DaysENG[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Months of the year in Russian and English
char *monthsRU[12] = {"января", "февраля", "марта", "апреля", "мая", "июня", "июля", "августа", "сентября", "октября", "ноября", "декабря"};
char *monthsENG[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

volatile unsigned char sec_flag=0;
volatile unsigned int ms_counter=0;


uint16_t temp_code_ADC;
volatile unsigned long long int code_ADC;
volatile unsigned char temp_ADC;
uint16_t x,y;
int main(void)
{
	//Initialization of the LCD, timer-counter 1, ADC and real-time clock sensor DS1307
	LCD_Init();
	Time_1_init();
	init_adc();
	Timer0_init();
	init_DS1307();
	LCD_Clear();
	asm("sei");
	while (1)
	{
		mainMenu();
	}
}

//Setting up the timer-counter 1
void Time_1_init()
{
	TCNT1=0x0000;
	OCR1A=31250;
	//Setting CTC mode
	TCCR1A &= ~((1<<WGM11)|(1<<WGM10));
	TCCR1B |= (1<<WGM12)|(1<<CS12);
	TIFR |= (1<<OCF1A); //Enabling the comparation with TCNT
	TIMSK |= (1<<OCIE1A); //Enabling interaptions
}
//Timer-counter 1 interrupt function
ISR (TIMER1_COMPA_vect)
{
	if(isStartedSWT) stopwatchTime[0]++; //Increasing the stopwatch time every second
	if(isStartedTMR) timerTime[0]--; //Decreasing the stopwatch time every second
	/*if(scene == 1) 
	{
		//Getting current time
		getCurrentTime(); 
		//Getting current date
		getCurrentDate(); 
	}
	*/
}

//ADC initialization
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
		
		PORTA=0b00001000;
		_delay_ms(2);
		ADMUX=0b00000001;
		ADCSRA=0b11000111 ;
		while((ADCSRA&0b01000000)!=0){}
		y=ADCL|(ADCH<<8);
		
	}
		if (scene == 0) //Checking the button presses for the start menu
		{
			if (startScreenScene == 0)
			{
				if((x>=120)&&(x<=340)&&(y>=380)&&(y<=480))
				{
					language = 0; //Selected Russian language
					startScreenScene = 1;//changing the screen to select date and time
					isSceneChanged = 1;
					LCD_Clear()
				}
				else if((x>=530)&&(x<=760)&&(y>=380)&&(y<=480))
				{
					language = 1; //Selected Russian language
					startScreenScene = 1;//changing the screen to select date and time
					isSceneChanged = 1;
					LCD_Clear()
				}
			}
			if (startScreenScene == 1)
			{
				if ((x>=80)&&(x<=130)&&(y>=680)&&(y<=730)) if(hour < 23) hour++;	//The plus button for the hours is pressed
				else if ((x>=78)&&(x<=150)&&(y>=310)&&(y<=370)) if (hour>0) hour--; //The minus button for the hours is pressed
				
				if ((x>=190)&&(x<=260)&&(y>=640)&&(y<=720)) if(minutes <59 ) minutes++;	//The plus button for the minues is pressed
				else if ((x>=185)&&(x<=275)&&(y>=320)&&(y<=390)) if (minutes>0) minutes--;//The minus button for the minues is pressed
				
				if ((x>=410)&&(x<=510)&&(y>=635)&&(y<=710)) //The plus button for the days is pressed
				{
					if ((month == 1) && (month == 3) && (month == 5) && (month == 7) && (month == 8) && (month == 10) && (month == 12) && day<31)
					{
						day++;	
					}
					else if( month == 1 && month % 4 == 0 && day<29) day++;
					else if( month == 1 && month % 4 != 0 && day<28) day++;
					else if(day<30) day++;
				}
				else if ((x>=420)&&(x<=520)&&(y>=300)&&(y<=360)) if (day>1) day--;	//The minus button for the days is pressed
				
				if ((x>=550)&&(x<=650)&&(y>=615)&&(y<=690)) if(month < 12) month++;//The plus button for the months is pressed
				if ((x>=570)&&(x<=670)&&(y>=260)&&(y<=400)) if (month>1) month--;//The minus button for the days is pressed
				
				if ((x>=700)&&(x<=790)&&(y>=600)&&(y<=670)) year++;//The plus button for the years is pressed
				if ((x>=730)&&(x<=815)&&(y>=300)&&(y<=360)) if (year>0) year--;//The minus button for the years is pressed
				
				if ((x>=505)&&(x<=556)&&(y>=132)&&(y<=215)) if(weekDay<7) weekDay++; //The plus button for the days of the weerk is pressed
				if ((x>=72)&&(x<=106)&&(y>=140)&&(y<=215)) if(weekDay>0) weekDay--; //The minus button for the days of the weerk is pressed
				
				if ((x>=725)&&(x<=825)&&(y>=120)&&(y<=205)) //The OK button is pressed
				{
					//Writing data into the  DS1307
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
					//Changing the scene to the main menu
					scene = 1;
					isSceneChanged = 1;
				}
			}
		}
		else if (scene == 1)//Checking the button presses for the main menu
		{
			if ((x>=725)&&(x<=800)&&(y>=670)&&(y<=730)) scene = 4; //The settings button is pressed
			else if ((x>=100)&&(x<=440)&&(y>=190)&&(y<=305)) scene = 2; //The stopwatch button is pressed
			if ((x>=530)&&(x<=750)&&(y>=190)&&(y<=290)) scene = 3;//The timer button is pressed
		}
		else if (scene == 2) //Checking the button presses for the stopwatch menu
		{
			if ((x>=75)&&(x<=210)&&(y>=718)&&(y<=790)) scene = 1;//The back button is pressed
			if ((x>=430)&&(x<=610)&&(y>=275)&&(y<=375)) isStartedSWT = 1;//The start button is pressed
			if ((x>=195)&&(x<=360)&&(y>=290)&&(y<=390)) isStartedSWT = 0;//The stop button is pressed
			if ((x>=305)&&(x<=510)&&(y>=120)&&(y<=220))//The reset button is pressed
			{
				isStartedSWT = 0;
				stopwatchTime[0] = 0;
				stopwatchTime[1] = 0;
				stopwatchTime[2] = 0;
			}
		}
		else if (scene == 3) //Checking the button presses for the timer menu
		{
			if (!isStartedTMR)
			{
				if ((x>=75)&&(x<=210)&&(y>=718)&&(y<=790)) 
				{
					scene = 1;//The back button is pressed
					isSceneChanged = 1;
				}
				
				
				else if ((x>=430)&&(x<=570)&&(y>=680)&&(y<=760)) timerTime[0]++;	//The "+1s" button is pressed
				else if ((x>=590)&&(x<=770)&&(y>=670)&&(y<=740)) timerTime[0]+=10;//The "+10s" button is pressed
				else if ((x>=430)&&(x<=580)&&(y>=605)&&(y<=700)) timerTime[1]++;	//The "+1m" button is pressed
				else if ((x>=605)&&(x<=775)&&(y>=580)&&(y<=690)) timerTime[1]+=10;//The "+10m" button is pressed
				else if ((x>=450)&&(x<=590)&&(y>=525)&&(y<=620)) timerTime[2]++;	//The "+1h" button is pressed
				else if ((x>=620)&&(x<=770)&&(y>=500)&&(y<=600)) timerTime[2]+=5;//The "+5h" button is pressed
				
				else if ((x>=445)&&(x<=600)&&(y>=360)&&(y<=470)) timerTime[0]--;//The "-1s" button is pressed
				else if ((x>=600)&&(x<=790)&&(y>=350)&&(y<=450)) timerTime[0]-=10;//The "-10s" button is pressed
				else if ((x>=450)&&(x<=600)&&(y>=270)&&(y<=370)) timerTime[1]--;//The "-1m" button is pressed
				else if ((x>=600)&&(x<=790)&&(y>=260)&&(y<=360)) timerTime[1]-=10;//The "-10m" button is pressed
				else if ((x>=450)&&(x<=600)&&(y>=190)&&(y<=290)) timerTime[2]--;//The "-1h" button is pressed
				else if ((x>=600)&&(x<=790)&&(y>=170)&&(y<=270)) timerTime[2]-=5;//The "-5h" button is pressed
				
				else if ((x>=100)&&(x<=325)&&(y>=265)&&(y<=415))//The start button is pressed
				{
					setTMR = 0;
					isStartedTMR = 1;
				}
			}
			else if ((x>=300)&&(x<=550)&&(y>=190)&&(y<=330))//The reset button is pressed
			{
				setTMR = 1;
				isStartedTMR = 0;
			}
			
		}
		else if (scene == 4) //Checking the button presses for the settings menu
		{
			if ((x>=75)&&(x<=210)&&(y>=718)&&(y<=790)) //The back button is pressed
			{
				scene = 1;
				isSceneChanged = 1;
			}
			else if ((x>=155)&&(x<=360)&&(y>=475)&&(y<=535)) dataStyle = 0;//The "May 1" button is pressed
			else if ((x>=155)&&(x<=360)&&(y>=380)&&(y<=475)) dataStyle = 1;//The "01.05" button is pressed
			else if ((x>=155)&&(x<=360)&&(y>=280)&&(y<=380)) dataStyle = 2;//The "05.01" button is pressed
			else if ((x>=590)&&(x<=680)&&(y>=130)&&(y<=215)) language = 0;//The "RU" button is pressed
			else if ((x>=710)&&(x<=825)&&(y>=130)&&(y<=215)) language = 1;//The "ENG" button is pressed
		}
}


//Displaying the menu
void mainMenu()
{
	if(isSceneChanged == 1) //Clearing screen if scene is changed
	{
		LCD_Clear();
		isSceneChanged = 0;
	}
	
	//Selecting the appropriate scene to display
	if (scene == 0)//Start menu
	{
		
		printStartMenu();
	}
	else if(scene == 1) //Main menu
	{
		
		printMainMenu();
	}
	else if (scene == 2) //Stopwatch menu
	{
		printStopwatchMenu();
	}
	else if(scene == 3) //Timer menu
	{
		printTimerMenu();
	}
	else if(scene == 4) //Settings menu
	{
		printSettings();
	}
}

//Start menu rendering function
void printStartMenu()
{	
	//Displaying the start menu with language selection
	if (startScreenScene == 0)
	{
		LCD_String(4, 10, "Русский");
		LCD_String(4, 78, "English");
	}
	//Displaying the start menu with date and time selection
	if (startScreenScene == 1)
	{
		//Displaying buttons to increase and decrease the time
		LCD_Char(1,4, '+');
		LCD_Char(5,4, '-');
		LCD_Char(1,26, '+');
		LCD_Char(5,26, '-');
		LCD_Char(3,0, hour/10 + '0');
		LCD_Char(3,8, hour%10 + '0');
		LCD_Char(3,16, ':');
		LCD_Char(3,24, minutes/10 + '0');
		LCD_Char(3,32, minutes%10 + '0');
		
		//Displaying buttons to increase and decrease the date
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
		
		//Display of the day of the week and the buttons for changing the day of the week in Russian
		if (!language) 
		{
			LCD_Char(7, 0, '-');
			LCD_String(7,8, DaysRU[weekDay]);
			LCD_Char(7, strlen(DaysRU[weekDay])*6+10, '+');
		}
		//Display of the day of the week and the buttons for changing the day of the week in English
		else
		{
			LCD_Char(7, 0, '-');
			LCD_String(7,8, DaysENG[weekDay]);
			LCD_Char(7, strlen(DaysENG[weekDay])*6+10, '+');
		}
		//Displaying the end button of the start settings
		LCD_String(7, 114, "OK");
	}
	
}

//Getting the current time
void getCurrentTime()
{
	minutes = read_DS1307(0x01);
	hour = read_DS1307(0x02);
}

//Getting the current day and month
void getCurrentDate()
{
	weekDay = read_DS1307(0x03);
	day = read_DS1307(0x04);
	month = read_DS1307(0x05);
}
//Main menu rendering function
void printMainMenu()
{
	//Setting time 
	LCD_Char(3,42, hour/10 + '0');
	LCD_Char(3,51, hour%10 + '0');
	LCD_Char(3,60, ':');
	LCD_Char(3,68, minutes/10 + '0');
	LCD_Char(3,76, minutes%10 + '0');
	
	if(dataStyle == 0)//Displaying the date in the "May 1" style"
	{
		//Displaying date in Russia
		if(!language)
		{
			LCD_Char(0,0, day/10+'0');
			LCD_Char(0,6, day%10+'0');
			LCD_String(0,12, monthsRU[month-1]);
			LCD_String(1,0,DaysRU[weekDay]);
		}
		
		//Displaying date in English
		else
		{
			LCD_String(0,0, monthsENG[month-1]);
			LCD_Char(0,strlen(monthsENG[month-1])*6+3, day/10+'0');
			LCD_Char(0,strlen(monthsENG[month-1])*6 + 9, day%10+'0');
			LCD_String(1,0,DaysENG[weekDay-1]);
		}
	}
	else if(dataStyle == 1)//Displaying the date in the "01.05" style
	{
		LCD_Char(1,0, day/10+'0');
		LCD_Char(1,6, day%10+'0');
		LCD_Char(1,12,'.');
		LCD_Char(1,18, month/10+'0');
		LCD_Char(1,24, month%10+'0');
		if(!language) LCD_String(0,0,DaysRU[weekDay]);
		else LCD_String(0,0,DaysENG[weekDay]);
	}
	else if (dataStyle == 2)//Displaying the date in the "05.01" style
	{
		LCD_Char(1,0, month/10+'0');
		LCD_Char(1,6, month%10+'0');
		LCD_Char(1,12,'.');
		LCD_Char(1,18, day/10+'0');
		LCD_Char(1,24, day%10+'0');
		if(!language) LCD_String(0,0,DaysRU[weekDay]);
		else LCD_String(0,0,DaysENG[weekDay]);
	}
	//Displaying the stopwatch and timer settings buttons according to the language
	if (!language)
	{
		LCD_String(6,8,"Секундомер"); 
		LCD_String(6,80,"Таймер");
	}
	else
	{
		LCD_String(6,8,"Stopwatch"); ////Displaying the stopwatch button
		LCD_String(6,80,"Timer"); //Displaying the timer button
	}
	for (int i = 0; i<10; i++)//Displaying the settings button
	{
		LCD_drawByte(0, 117+i, settingsImage[i]);
	}
}

//Stopwatch menu rendering function
void printStopwatchMenu()
{
	if (stopwatchTime[0] == 60) //Checking the correctness of the stopwatch time
	{
		stopwatchTime[0] = 0;
		stopwatchTime[1]++;
		if(stopwatchTime[1] == 60)
		{
			stopwatchTime[1] = 0;
			stopwatchTime[2]++;
		}
	}

	//Rendering the stopwatch time
	LCD_Char(3,75,stopwatchTime[0]/10+'0');
	LCD_Char(3,82,stopwatchTime[0]%10+'0');
	LCD_Char(3,68,':');
	LCD_Char(3,54,stopwatchTime[1]/10+'0');
	LCD_Char(3,61,stopwatchTime[1]%10+'0');
	LCD_Char(3,33,stopwatchTime[2]/10+'0');
	LCD_Char(3,40,stopwatchTime[2]%10+'0');
	LCD_Char(3,47,':');

	if (!language) //Drawing of the control buttons back, start and stop in Russian
	{
		LCD_String(0,0,"назад");
		LCD_String(5,27,"Стоп");
		LCD_String(5,64,"Старт");
		if (isStartedSWT)
		{
			LCD_String(7,45, "Сброс");
		}
	}
	else //Drawing of the control buttons back, start and stop in Russian
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

//Timer menu rendering function
void printTimerMenu()
{
	if (timerTime[0] == -1) //Checking the correctness of the timer time
	{
		timerTime[0] = 59;
		timerTime[1]--;
		if(timerTime[1] == -1)
		{
			timerTime[1] = 59;
			timerTime[2]--;
		}
	}
	
	//Checking the condition for stopping the timer
	if (isStartedTMR && (timerTime[2] == 0) && (timerTime[1] == 0) && (timerTime[0] == 0))
	{
		LCD_Clear();
		isStartedTMR = 0;
		timerTime[0] = 0;
		timerTime[1] = 0;
		timerTime[2] = 0;
	}
	
	//Rendering the timer menu
	if (setTMR)
	{
		if (!language)//Rendering the timer menu before starting in Russian
		{
			LCD_String(0,0,"назад");
			LCD_Char(3,2, timerTime[2]/10+'0');
			LCD_Char(3,9,timerTime[2]%10+'0');
			LCD_Char(3,16,':');
			LCD_Char(3,23, timerTime[1]/10+'0');
			LCD_Char(3,30,timerTime[1]%10+'0');
			LCD_Char(3,37,':');
			LCD_Char(3,44, timerTime[0]/10+'0');
			LCD_Char(3,51,timerTime[0]%10+'0');
			
			LCD_String(0,69,"+1с");
			LCD_String(0,95,"+10с");
			LCD_String(1,69,"+1м");
			LCD_String(1,95,"+10м");
			LCD_String(2,69,"+1ч");
			LCD_String(2,101,"+5ч");
			
			LCD_String(4,69,"-1с");
			LCD_String(4,95,"-10с");
			LCD_String(5,69,"-1м");
			LCD_String(5,95,"-10м");
			LCD_String(6,69,"-1ч");
			LCD_String(6,101,"-5ч");
			
			LCD_String(5,13,"Старт");
		}
		else //Rendering the timer menu before starting in Russian
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
	else //Rendering the timer menu after starting accordig to language
	{
		LCD_Char(3,75,timerTime[0]/10+'0');
		LCD_Char(3,82,timerTime[0]%10+'0');
		LCD_Char(3,68,':');
		LCD_Char(3,54,timerTime[1]/10+'0');
		LCD_Char(3,61,timerTime[1]%10+'0');
		LCD_Char(3,33,timerTime[2]/10+'0');
		LCD_Char(3,40,timerTime[2]%10+'0');
		LCD_Char(3,47,':');
		
		if (!language) LCD_String(6,45,"Сброс");
		else LCD_String(6,45,"Reset");
	}
}
//Settings menu rendering function
void printSettings()
{
	if (!language)//Displaying all buttons in Russian
	{
		LCD_String(0,0,"назад");
		
		LCD_String(2,0,"Стиль даты:");
		LCD_String(3,10, "- 1 Мая");
		LCD_String(4,10, "- 01.05");
		LCD_String(5,10, "- 05.01");
		
		LCD_String(7,0, "Язык системы");
		LCD_String(7, 92, "РУ");
		LCD_String(7, 110, "ENG");
	}
	else //Displaying all buttons in English
	{
		LCD_String(0,0,"back"); 
		
		LCD_String(2,0,"Data style:");
		LCD_String(3,10, "- May 1st");
		LCD_String(4,10, "- 01.05");
		LCD_String(5,10, "- 05.01");
		
		LCD_String(7,0, "System");
		LCD_String(7, 92, "РУ");
		LCD_String(7, 110, "ENG");
	}
}