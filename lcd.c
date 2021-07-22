#include <lcd.h>

void trigger()
{
	ERST_PORT |= (1<<EN);
	_delay_us(50);
	ERST_PORT &= ~(1<<EN);
	_delay_us(50);
}

//Sending a command to LCD
void LCD_Command(char Command)
{
	RSRW_PORT &= ~(1 << RS); //LCD recognise byte as a internal command	
	RSRW_PORT &= ~(1 << RW); //Enable writing to LCD		
	LCD_DATA_PORT = Command; //Put command to data port
	//Выставляем бит EN в 1 для формирования состояния начала отправки команды	
	trigger(); //Formatting the write signal
}

//Sending a data to LCD
void LCD_Data(char Data)
{
	RSRW_PORT |=  (1 << RS);	 //LCD recognise byte as a data	
	RSRW_PORT &= ~(1 << RW); //Enable writing to LCD		
	LCD_DATA_PORT = Data; //Put data to data port	
	trigger(); //Formatting the write signal
}

//Intializing the LCD
void LCD_Init()
{
	//Setting all used port to output
	LCD_DATA_DDR = 0xFF;
	RSRW_DDR |= (1<<RS)|(1<<RW);
	ERST_DDR |= (1<<EN)|(1<<RST);
	CS_DDR |= (1<<CS1)|(1<<CS2);

	CS_PORT &= ~((1 << CS1) | (1 << CS2)); //Enabling both chip selectors
	ERST_PORT |= (1 << RST);
	_delay_us(20);
	LCD_Command(0x3E);//Sending TURN OFF LCD command
	LCD_Command(0x40);//Y=0
	LCD_Command(0xB8);//X=0
	LCD_Command(0xC0);//Z=0	
	LCD_Command(0x3F);//Sending TURN ON LCD command	
}

//Clear the screen
void LCD_Clear()
{
	//Enabling only the left chip selector
	CS_PORT &= ~(1 << CS1);
	CS_PORT |= (1 << CS2);

	for(int i = 0; i < 8; i++)
	{
		
		LCD_Command((0xB8) + i);
		for(int j = 0; j < 64; j++)
		{
			LCD_Data(0); //Clearing the left haf of the screen
		}
	}

	//Enabling only the right chip selector
	CS_PORT |= (1 << CS1);
	CS_PORT &= ~(1 << CS2);
	for(int i = 0; i < 8; i++)
	{
		
		LCD_Command((0xB8) + i);
		for(int j = 0; j < 64; j++)
		{
			LCD_Data(0);//Clearing the left haf of the screen
		}
	}
	//Setting the position (x;y)=(0;0)
	LCD_Command(0x40);
	LCD_Command(0xB8);
}

//Setting the selected position
void LCD_GotoXY(uint8_t x, uint8_t y)
{
	//Determining the half
	if(y<64)
	{
		//Enabling the left chip selector
		CS_PORT &= ~(1<<CS1);
		CS_PORT |= (1<<CS2);
	}
	else
	{
		//Enabling the right chip selector
		CS_PORT &= ~(1<<CS2);
		CS_PORT |= 1<<CS1;
		y=y-64;
	}
	LCD_Command(0xB8+x);//Setting the selected position for X-axis
	LCD_Command(0x40+y);//Setting the selected position for Y-axis
	LCD_Command(0x3F);//TURN ON LCD screen
	LCD_Command(0xC0);
}

//Displaying a byte on the screen in the selected position
void LCD_drawByte(uint8_t x, uint8_t y, unsigned char byte)
{
	LCD_GotoXY(x,y);
	LCD_Data(byte);
}

//Displaying a char on the screen in the selected position
void LCD_Char(uint8_t  x1, uint8_t y1, char code)
{   
	//Converting the character code according to a predefined custom alphabet
	if((code >= 0x20) && (code < 0x80)) code -= 32;
	else  code -= 96;
	
	for(uint8_t i = 0; i < 5; i++)
	{
		LCD_drawByte(x1, y1 + i, symbol[code][i]);
	}
}

//Displaying a string on the screen in the selected position
void LCD_String(uint8_t x1, uint8_t y1, char *string)
{
	while(*string)
	{
		//Character-by-character display of each character of the string
		LCD_Char(x1, y1, *string++);
		y1+=6;
	}
}