#include <util/delay.h>
#include <avr/io.h>

void init_DS1307();Initializing the ds1307
void write_DS1307 (uint8_t reg, uint8_t time); //Writing data to ds1307
uint8_t read_DS1307 (uint8_t reg); //Reading data from ds1307