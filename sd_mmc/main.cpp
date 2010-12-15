/*
 * main.cpp
 *
 *  Created on: 2010-12-05
 *      Author: antoine
 */

#include "diskio.h"
#include "ff.h"
#include "serial/HardwareSerial.h"
#include <avr/delay.h>
#include <avr_pal.h>

FATFS fatfs;
FIL file;

int main(void)
{
	init();
	Serial.begin(9600);
	Serial.println("start");
	Serial.println(f_mount(0, &fatfs), HEX);
	Serial.println(f_open(&file, "/PRINT00.TXT", FA_READ | FA_OPEN_EXISTING), HEX);

	Serial.println("end");
	for(;;)
	{

		_delay_ms(1000);
	}
}
