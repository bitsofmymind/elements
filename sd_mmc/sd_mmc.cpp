/* sd_mmc.cpp - Implements a resource to interface with FAT on SD/MMC cards
 * Copyright (C) 2011 Antoine Mercier-Linteau
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sd_mmc.h"
#include "fat_file.h"
#include <stdlib.h>
#include <string.h>
#include "../avr_pal.h"
#include "diskio.h"
#include "sd_mmc_io.h"

SDMMC::SDMMC(void):
		Resource()
{
	CARD_DETECT_DDR &= ~_BV(CARD_DETECT_PIN);
	CARD_DETECT_PORT |= _BV(CARD_DETECT_PIN); //Turns on the pull-up for CARD_DETECT_PIN
	run(); /*run() is in charge of periodically checking for disk presence and
	initialization so calling it will take care of booting our disk.*/
}

Response::status_code SDMMC::process( Request* request, Response* response )
{
	Response::status_code sc;
	uint8_t len  = 0;

	if(!request->is_method(Request::GET))
	{
		return NOT_IMPLEMENTED_501;
	}
	//possible optimization: pass the url object to FILE_FAT object directly.
	uint8_t dst = request->to_destination();
	while(request->to_destination())
	{
		len += strlen(request->current()) + 1; // for '/'
		request->next();
	}

	while(request->to_destination() != dst)
	{
		request->previous();
	}

	char* path = (char*)ts_malloc(len + 1);

	if(!path)
	{
		//Critical error, there is no memory left.
	}
	for(uint8_t pos = 0; request->to_destination(); request->next())
	{
		path[pos++] = '/';
		const char* res = request->current();
		memcpy((void*)(path  + pos), res, strlen(res));
		pos += strlen(res);
	}

	path[len] = '\0';

	if(Stat)
	{
		//No disk present, disk not initialized of failed to initialize
		ts_free(path);
		sc = INTERNAL_SERVER_ERROR_500;
	}
	else
	{
		//Response* response;
		FATFile* file = new FATFile(path);
		VERBOSE_PRINT_P("Fetching ");
		VERBOSE_PRINTLN(path);
		if(!file)
		{
			sc = INTERNAL_SERVER_ERROR_500;
		}
		else if(file->last_op_result == FR_OK)
		{
			response->set_body(file, NULL);
			sc = OK_200;
		}
		else
		{

			if(file->last_op_result == FR_NO_FILE || file->last_op_result == FR_NO_PATH)
			{
				sc = NOT_FOUND_404;
			}
			else
			{
				ERROR_PRINT_P("error opening file ");
				ERROR_TPRINTLN((uint8_t)file->last_op_result, DEC);

				sc = INTERNAL_SERVER_ERROR_500;
			}
			delete file;
		}

	}

	return sc;
}

void SDMMC::run(void)
{
	if(!(CARD_DETECT_PINREG & _BV(CARD_DETECT_PIN))) //If there is a disk in the socket
	{
		if(Stat & (STA_NOINIT)) //If the disk in the socket has not been initialized
		{
			if(Stat & STA_NODISK) //If there was previously no disk in the socket
			{
				/*Do nothing. With the setting of STA_NODISK below and the scheduling
				 * of the next run in 100ms, this will effectively provide a debouncing
				 * delay.*/
				//Debug::println("Disk inserted");
				Stat &= ~STA_NODISK; //Indicate the presence of disk in the socket
				VERBOSE_PRINTLN_P("Disk detected");
			}
			else if(!disk_initialize()) //If there is a disk and it has been debounced
			{
				//STA_NOINIT was cleared in disk_initialized because it succeeded

				f_mount(0, &fatfs); //Mounts the disk
				VERBOSE_PRINTLN_P("Disk initialized");

			}
			else
			{
				ERROR_PRINTLN_P("Disk fail");
			}
			//Initialization will be attempted again in 100ms if it failed.

		}
		schedule(100);
	}
	else //If there is no disk in the socket
	{
		if(!(Stat & STA_NOINIT)) //If a disk has been initialized
		{
			VERBOSE_PRINTLN_P("Disk removed");
			f_mount(0, NULL); //unmounts the disk
			power_off(); //Power it off
		}
		Stat = STA_NODISK + STA_NOINIT; /*Indicate there is no disk and it has not been
		initialized*/
		schedule(1000);
	}
}
