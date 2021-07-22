#include<alphabet.c>

#define LCD_DATA_PORT PORTC 
#define LCD_DATA_DDR DDRC
#define LCD_DATA_PIN PINC
#define CS_PORT PORTB //cs1\cs2 port
#define CS_DDR DDRB
#define RSRW_PORT PORTA // RS\RW port
#define RSRW_DDR DDRA// DI\RW port
#define ERST_PORT PORTD // RST and E port
#define ERST_DDR DDRD
#define RS 2 // Data/Command
#define RW 3 // Reading/Writing
#define EN 6 
#define CS1 0 // Chip-selector 1
#define CS2 1 // Chip-selector 2
#define RST 7 // Reset address

void LCD_Command(char Command);//Sending a command to LCD
void LCD_Data(char Data);//Sending a data to LCD
void LCD_Init();//Intializing the LCD
void LCD_Clear();//Clear the screen
void LCD_GotoXY(uint8_t x, uint8_t y);//Setting the selected position
void LCD_drawByte(uint8_t x, uint8_t y, unsigned char byte);//Displaying a byte on the screen in the selected position
void LCD_Char(uint8_t  x1, uint8_t y1, char code);//Displaying a char on the screen in the selected position
void LCD_String(uint8_t x1, uint8_t y1, char *string);//Displaying a string on the screen in the selected position
