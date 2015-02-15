/* sd_mmc.cpp - Source file for the SDMMC class.
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
	// Configure the card detect pin.
	CARD_DETECT_DDR &= ~_BV(CARD_DETECT_PIN); // Set the card detect pin to input mode.
	CARD_DETECT_PORT |= _BV(CARD_DETECT_PIN); //Turns on the pull-up for CARD_DETECT_PIN

	/* run() is in charge of periodically checking for disk presence and
	initialization so calling it will take care of booting our disk.*/
	run();
}

Response::status_code SDMMC::process(Request* request, Response* response)
{
	Response::status_code sc; // The response status code.

	/* Whether the request is a destination or not is not checked because
	 * whatever resource is requested will necessarily have to be on the file
	 * system and accessed using a path.*/

	if(!request->is_method(Request::GET)) // If this is not GET request.
	{
		return Response::NOT_IMPLEMENTED_501; // Method not implemented.
	}

	/** TODO when this is the destination of the request, return an OK to indicate
	 * the presence of a file system. */

	/// TODO possible optimization: pass the url object to the FATFile object directly.

	// Extract the file path from the url.

	uint8_t dst = request->to_destination(); // The number of resources to destination.
	uint8_t len  = 0; // The length of the path string.

	// Get the length of the path string.
	while(request->to_destination())
	{
		len += strlen(request->current()) + 1; // for '/'
		request->next(); // Go to the next resource.
	}

	// Rewind the resource path to were it was.
	while(request->to_destination() != dst)
	{
		request->previous();
	}

	// Allocate memory for a string to store the path. + 1 for null character.
	/// TODO null character is not needed because a space for the / was added previously.
	char* path = (char*)ts_malloc(len + 1);

	if(!path) // If the string could not be allocated.
	{
		// Critical error, there is no memory left.
		return Response::SERVICE_UNAVAILABLE_503;
	}

	// Build the path string.
	for(uint8_t pos = 0; request->to_destination(); request->next())
	{
		/// TODO use realloc().

		path[pos++] = '/'; // Start with a /.

		const char* res = request->current(); // Get the current resource.

		// Copy the resource string.
		/// TODO use strcpy() instead.
		memcpy((void*)(path  + pos), res, strlen(res));

		pos += strlen(res); // Move the pointer to the end of the current resource.
	}

	path[len] = '\0'; // End the path with null character.

	if(Stat) // If the stat structure has not been initialized.
	{
		/** TODO move this block to the top of the method to avoid loosing time
		 * processing a path when there is no disk. */

		//No disk present, disk not initialized of failed to initialize.
		ts_free(path);

		/* The path requested was not found because there is no accessible disk.
		 * However, it might become available in the future. */
		sc = Response::NOT_FOUND_404;
	}
	else
	{
		/* Encapsulate the file with a FATFile object. The path string is now
		in the possession of the object and will be freed by it.*/
		FATFile* file = new FATFile(path);

		VERBOSE_PRINT_P("Fetching ");
		VERBOSE_PRINTLN(path);

		if(!file) // If a file object could not be created.
		{
			// Not enough resources to process the request.
			sc = Response::SERVICE_UNAVAILABLE_503;
			ts_free(path);
		}
		// If the file was successfully fetched.
		else if(file->last_op_result == FR_OK)
		{
			response->set_body(file, NULL);
			sc = Response::OK_200;
		}
		else // There was an issue with the file.
		{
			// If the file or path is invalid.
			if(file->last_op_result == FR_NO_FILE || file->last_op_result == FR_NO_PATH)
			{
				sc =Response:: NOT_FOUND_404;
			}
			else // There was an error opening the file.
			{
				ERROR_PRINT_P("error opening file ");
				ERROR_TPRINTLN((uint8_t)file->last_op_result, DEC);

				sc = Response::INTERNAL_SERVER_ERROR_500;
			}

			delete file;
		}
	}

	return sc;
}

void SDMMC::run(void)
{
	// If there is a disk in the socket.
	if(!(CARD_DETECT_PINREG & _BV(CARD_DETECT_PIN)))
	{
		// If the disk in the socket has not been initialized.
		if(Stat & (STA_NOINIT))
		{
			// If there was previously no disk in the socket.
			if(Stat & STA_NODISK)
			{
				/*Do nothing. With the setting of STA_NODISK below and the scheduling
				 * of the next run in 100ms, this will effectively provide a debouncing
				 * delay.*/

				Stat &= ~STA_NODISK; // Indicate the presence of disk in the socket.
				VERBOSE_PRINTLN_P("Disk detected");
			}
			// If there is a disk and it has been debounced, initialize it.
			else if(!disk_initialize())
			{
				// STA_NOINIT was cleared in disk_initialized because it succeeded.

				f_mount(0, &fatfs); // Mounts the disk.
				VERBOSE_PRINTLN_P("Disk initialized");

			}
			else
			{
				// Initialization will be attempted again in 100ms if it failed.
				ERROR_PRINTLN_P("Disk fail");
			}
		}

		schedule(100); // Run again in 100 ms.
	}
	else //If there is no disk in the socket
	{
		if(!(Stat & STA_NOINIT)) //If a disk has been initialized
		{
			VERBOSE_PRINTLN_P("Disk removed");
			f_mount(0, NULL); // Unmount the disk.
			power_off(); //Power it off.
		}

		// Indicate there is no disk and it has not been initialized.
		Stat = STA_NODISK + STA_NOINIT;

		schedule(1000); // Check again for the presence of a disk in 1 second.
	}
}
