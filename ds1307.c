//Initializing the ds1307
void init_DS1307(void)
{
	Setting TWBR and TWSR to work with ds1307 in 100 kHz
	TWBR = 32;
	TWSR = (0 << TWPS1)|(0 << TWPS0);
}

//Reading data from ds1307
uint8_t read_DS1307(uint8_t addr) 
{
	uint8_t time;
	TWCR |= (1<<TWEN);//Enabling TWI 
	while(!(TWCR&(1<<TWEN)));//Setting START condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	TWDR = 0xd0; //Sending the ds1307 address and writing bit
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	TWDR = addr; //Sending the address to read
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));//Repeating the start condition to read from data ds1307
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	TWDR = 0xd1; //Sending the ds1307 address and reading bit
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));//Reading data
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	time = TWDR;
	time = (((time & 0xF0) >> 4)*10)+(time & 0x0F);
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//Setting STOP condition
	while(TWCR&(1<<TWSTO));
	TWCR &= (1<<TWEN);//Disabling TWI for correct working with LCD
	_delay_us(50);
	TWCR &= (1<<TWINT);
	return time;
}

//Функция записи данных в DS1307
void write_DS1307 (uint8_t reg, uint8_t time)
{
	TWCR |= (1<<TWEN);//Setting START condition
	while(!(TWCR&(1<<TWEN)));
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	TWDR = 0xd0; //Sending the ds1307 address and writing bit
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	TWDR = reg;//Sending the address to read
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	time = ((time/10)<<4) + time%10;
	TWDR = time; //Writing data into ds1307
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);//Setting STOP condition
	while(TWCR&(1<<TWSTO));
	TWCR &= (1<<TWEN); //Disabling TWI for correct working with LCD
	_delay_us(50);
	TWCR &= (1<<TWINT);
}