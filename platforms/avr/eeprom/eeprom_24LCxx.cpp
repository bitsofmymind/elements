/* eeprom_24LCxx.cpp - Implements a resource for interfacing with a 24_LCxx eeprom.
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

// Inspired from <joerg@FreeBSD.ORG> 's twitest in avr-libc

#include "eeprom_24LCxx.h"
#include "eeprom_file.h"
#include "../avr_pal.h"
#include <avr/io.h>
#include <util/twi.h>
#include <string.h>
#include "../utils/pgmspace_file.h"
#include <utils/memfile.h>
#include <utils/template.h>
#include <stdlib.h>

// TWI hardware defines

/// The address of the device on the TWI bus.
#define DEVICE_ADDRESS 0b10100000

/// The maximum number of attempts to communicate with the device before a timeout.
#define MAX_ITER 200

/// Success code.
#define SUCCESS 0

/// Bus unresponsive code.
#define BUS_UNRESPONSIVE 1 // Too many arbitration errors.

/// Write protected device code.
#define DEVICE_WRITE_PROTECTED 2

/*
 * The LINEAR FILE SYSTEM
 *
 * Files on the EEPROM are stored using a simple linear file system. By linear,
 * it is meant that whole files are stored in a sequential manner without
 * support for folders. The file system is organized in the following fashion
 * on the EEPROM:
 *
 * EEPROM_START_ADDRESS
 * - file system state structure
 * - file 1 structure
 * - file 1 contents
 * - file 2 structure
 * - file 2 contents
 * (free space)
 * EEPROM_END_ADDRESS
 * */


// File system state structure defines.

/// The size of a file entry.
#define FILE_ENTRY_SIZE 			sizeof(file_entry)

/// The ID of a file system, used to check if an EEPROM is formatted.
#define ID 							0xAB

/// The start of the main file system state structure.
#define FILE_SYSTEM					0

/// The offset of the file system ID.
#define FILE_SYSTEM_ID				FILE_SYSTEM

/// The offset of the last file pointer.
#define LAST_FILE_PTR				FILE_SYSTEM_ID + 1

/// The offset of the used space variable.
#define SPACE_USED					LAST_FILE_PTR + 2

/// The offest of the number of files variable.
#define NUMBER_OF_FILES				SPACE_USED + 2

/// The offset of the first file.
#define FIRST_FILE					FILE_SYSTEM + sizeof(file_system)

// File structure defines.

/// The offset of the file size.
#define FILE_SIZE					0

/// The offset of the file name.
#define FILE_NAME					FILE_SIZE + sizeof(uint16_t)

/// The end of the file structure.
#define END							FILE_NAME + FILE_NAME_MAX_SIZE

EEPROM_24LCXX::EEPROM_24LCXX():
	Resource()
{
	VERBOSE_PRINTLN_P("EEPROM Starting...");
	/* Initialize tje TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1
	 * has prescaler (mega128 & newer) */
	TWSR = 0;

#if UPLOAD_FROM_UART
	// Set the instance so this object can be called from the interrupt routine.
	instance = this;

	// Set the UART protocol state variables.
	uart_pos = 0;
	uart_buffer = NULL;
	uart_state = CMD;
	last_rx = 0;
#endif


#if F_CPU < 3600000UL
	TWBR = 10;			// Smallest TWBR value.
#else
	TWBR = (F_CPU / 100000UL - 16) / 2;
#endif

	// Read the file system state structure from the EEPROM.
	if(!read(FILE_SYSTEM_ID, sizeof(file_system)))
	{
		ERROR_PRINTLN_P("Read operation from EEPROM failed");

		/* TODO It is not possible to recover from this error. Access to this
		 * resource should be prevented. */

		return; // Reading failed.
	}

	// Access the buffer a file system structure.
	file_system* fs = (file_system*)page_buffer;

	if(fs->id != ID) // If no file system has been detected.
	{
		if(format_file_system()) // Format the file system.
		{
			/* TODO It is not possible to recover from this error. Access to this
			 * resource should be prevented. */

			return; // Formatting the file system failed.
		}
	}
	else
	{
		VERBOSE_PRINTLN_P("File system detected");
	}

	// Print out file system statistics.
	VERBOSE_PRINT_P("id: 0x");
	VERBOSE_TPRINTLN(fs->id, HEX);
	VERBOSE_PRINT_P("space used: ");
	VERBOSE_TPRINTLN(fs->space_used, DEC);
	VERBOSE_PRINT_P("number of files: ");
	VERBOSE_TPRINTLN(fs->number_of_files, DEC);
	VERBOSE_PRINT_P("last file: 0x");
	VERBOSE_TPRINTLN(fs->last_file_ptr, HEX);

	VERBOSE_PRINTLN_P("EEPROM ready");
}

uint8_t EEPROM_24LCXX::format_file_system(void)
{
	VERBOSE_PRINTLN_P("Formatting file system.");
	// Set a file system structure in the page buffer.
	page_buffer[0] = ID;
	file_system* fs = (file_system*)(page_buffer);
	fs->last_file_ptr = 0;
	fs->number_of_files = 0;
	fs->space_used = sizeof(file_system);

	// Write the file system structure to the EEPROM.
	if(!write(FILE_SYSTEM, sizeof(file_system)))
	{
		ERROR_PRINTLN_P("Formatting failed");

		return 1; // Formatting failed.
	}

	return 0; // Done formatting.
}

uint8_t EEPROM_24LCXX::create_file(const char* name)
{
	VERBOSE_PRINT_P("Creating file \"");
	VERBOSE_PRINTLN(name);

	// Read the file system state structure into the page buffer.
	read(FILE_SYSTEM , sizeof(file_system));

	file_system* fs = (file_system*)page_buffer;
	uint16_t addr; // The address of the new file.

	if(fs->number_of_files) // If there are files on the file system.
	{
		addr = fs->space_used; // The new file will be at the end.

		/* Check if there is enough space for a new file. This only accounts
		 * for the size of the file structure, not the actual content of the
		 * file. */
		if(EEPROM_SIZE - (addr + FILE_ENTRY_SIZE) < FILE_ENTRY_SIZE)
		{
			ERROR_PRINTLN_P("Not enough space for file");

			return 1; // Failed to create new file.
		}
	}
	else // This is the first file on the file system.
	{
		VERBOSE_PRINTLN_P("First file");
		addr = FIRST_FILE;
	}

	// Update the file system state structure.
	fs->last_file_ptr = addr;
	fs->number_of_files++;
	fs->space_used += FILE_ENTRY_SIZE;
	write(FILE_SYSTEM, sizeof(file_system));

	// Write the new file structure.
	file_entry* fe = (file_entry*)page_buffer;
	fe->size = 0; // The file has no content for now.
	fe->end  = '\0';
	strncpy(fe->name, name, FILE_NAME_MAX_SIZE);
	write(addr, FILE_ENTRY_SIZE);

	VERBOSE_PRINTLN_P("File created");

	return 0; // Done file creation.
}

uint8_t EEPROM_24LCXX::find_file(const char* name, uint16_t* entry_addr)
{
	VERBOSE_PRINT_P("Finding file ");
	VERBOSE_PRINT(name);
	VERBOSE_PRINT_P(" ... ");

	// Read the location of the last file.
	read(LAST_FILE_PTR, 2);
	uint16_t last_file = *((uint16_t*)page_buffer);
	uint16_t addr = FIRST_FILE; // The address from where searching will start.
	uint8_t res; // Reading result.

	*entry_addr = 0;

	if(last_file != 0x0000) // If there are files on the file system.
	{
		// Loops to check every file name sequetially.
		while(true)
		{
			res = read(addr, FILE_ENTRY_SIZE); // Read a file entry.

			/// TODO Should check if res == FILE_ENTRY_SIZE.
			if(!res) // If reading failed.
			{
				/// TODO not useful, an error code should be returned instead.
				return res; // Return the amount of bytes read.
			}

			// If this is the file we are looking for.
			if(!strcmp(name, ((file_entry*)page_buffer)->name))
			{
				*entry_addr = addr; // Set the address of the found file.
				VERBOSE_PRINT_P("Found at 0x");
				VERBOSE_TPRINTLN(addr, HEX);

				return 0; // Found the file.
			}

			/// TODO this should check if addr is equal of greater than last file.
			if(addr == last_file) // If we have reached the end of the files.
			{
				break; // File not found.
			}

			// Go to the next file entry.
			addr += ((file_entry*)page_buffer)->size + FILE_ENTRY_SIZE;
		}
	}

	VERBOSE_PRINTLN_P("Not found");

	return 1; // File not found.
}

uint8_t EEPROM_24LCXX::find_file(uint8_t index, uint16_t* entry_addr)
{
	VERBOSE_PRINT_P("Finding index ");
	VERBOSE_TPRINT(index, DEC);
	VERBOSE_PRINT_P(" ... ");

	// Read the location of the last file.
	read(LAST_FILE_PTR, 2);
	uint16_t last_file = *((uint16_t*)page_buffer);
	uint16_t addr = FIRST_FILE; // The address from where searching will start.
	uint8_t res; // Reading result.

	*entry_addr = 0;

	if(last_file != 0x0000) // If there are files on the file system.
	{
		/* For each file in the file system. The loop will stop once the
		 * wanted index has been reached. */
		for(uint8_t i = 0; i <= index; i++, addr += ((file_entry*)page_buffer)->size + FILE_ENTRY_SIZE)
		{
			res = read(addr, FILE_ENTRY_SIZE);

			/// TODO Should check if res == FILE_ENTRY_SIZE.
			if(!res) // If reading failed.
			{
				/// TODO not useful, an error code should be returned instead.
				return res; // Return the amount of bytes read.
			}

			if(addr > last_file) // If we have gone past the last file.
			{
				VERBOSE_PRINTLN_P("Not found");

				return 1; // File not found.
			}
		}
	}
	else
	{
		VERBOSE_PRINTLN_P("Not found");

		return 1; // File not found.
	}

	*entry_addr = addr; // Set the address of the found file.

	return 0; // Found the file.
}

uint8_t EEPROM_24LCXX::append_to_file(uint16_t addr, File* content)
{
	//VERBOSITY PRITING
	VERBOSE_PRINT_P("Appending to file at 0x");
	VERBOSE_TPRINT(addr, HEX);
	VERBOSE_PRINT_P(" with ");
	VERBOSE_TPRINT((uint16_t)content->size, DEC);
	VERBOSE_PRINTLN_P(" bytes");

	//Reads the file system record to the page buffer.
	read(FILE_SYSTEM, sizeof(file_system));
	file_system* fs = (file_system*)page_buffer;

	uint16_t end = fs->space_used; // The end of the files.

	/* If the data we are about to write does not fit in the remaining space
	 * of the EEPROM. */
	if(EEPROM_SIZE - end < content->size)
	{
		ERROR_PRINTLN_P("Not enough space!"); // Throw and error.
		return 1; // Not enough space.
	}

	// If the file we will be appending to is not the last file.
	if(addr != fs->last_file_ptr)
	{
		/* Increase the last file pointer by the amount of bytes the last file
		 * will get shifted. */
		fs->last_file_ptr += content->size;
	}

	/* Increase the space_used variable by the amount of content we will be
	 * adding to the file system. */
	fs->space_used += content->size;

	// Write the updated file system record to the EEPROM.
	write(FILE_SYSTEM, sizeof(file_system));

	// Read the file entry.
	read(addr + FILE_SIZE, sizeof(uint16_t));
	file_entry* fe = (file_entry*)page_buffer;
	// The start of the content of the file.
	uint16_t start = fe->size + FILE_ENTRY_SIZE + addr;

	// Update the file entry.
	fe->size += content->size; // Increase the size of the file.
	write(addr + FILE_SIZE, sizeof(uint16_t));

	/* Move all subsequent files by the size of the added content to make room
	 * for it on the EEPROM. */
	for(uint8_t size_to_move = (end - start) % PAGE_SIZE; end > start; size_to_move = PAGE_SIZE)
	{
		end -= size_to_move;
		read(end, size_to_move); // Read the bytes to move.
		write(end + content->size, size_to_move); // Move them to their new location.
	}

	/* Append the new content to the file. The loop will stop when the amount of
	 * bytes copied falls under PAGE_SIZE. */
	/// TODO initializing copied is not required.
	for(uint8_t copied = PAGE_SIZE; copied == PAGE_SIZE ;)
	{
		// Read from the new content into the page buffer.
		copied = content->read(page_buffer, PAGE_SIZE);
		write(start, copied); // Write the page.
		start += copied; // The start of the new block.
	}

	return 0; // Done appending data to file.
}

uint8_t EEPROM_24LCXX::delete_file(uint16_t addr)
{
	VERBOSE_PRINT_P("Deleting file at 0x");
	VERBOSE_TPRINTLN(addr, HEX);

	// Read the file size at the wanted address.
	read(addr + FILE_SIZE, sizeof(uint16_t));
	uint16_t file_size = *((uint16_t*)page_buffer);

	// Read the file system state structure.
	read(FILE_SYSTEM, sizeof(file_system));
	file_system* fs = (file_system*)page_buffer;
	uint16_t end = fs->space_used; // The end of the file system.

	// If there is only one file on the file system.
	if(addr == FIRST_FILE && fs->number_of_files == 1)
	{
		fs->last_file_ptr = 0; // No more files on the files system.
	}
	else if(addr == fs->last_file_ptr) // If the last file is being deleted.
	{
		uint16_t current = FIRST_FILE; // Start at the first file.

		// Loop until the second last file is reached.
		file_entry* fe = (file_entry*)page_buffer;
		for(uint8_t i = 1; i < fs->number_of_files - 1; i++)
		{
			// Read the current file entry.
			read(current + FILE_SIZE, sizeof(uint16_t));
			current += FILE_ENTRY_SIZE + fe->size; // Move to the next file.
		}

		// Read the file system state structure again to have it in the buffer.
		read(FILE_SYSTEM, sizeof(file_system));

		// The second last file is now the last file.
		fs->last_file_ptr = current;
	}
	else // A file in the middle is being deleted.
	{
		// Move the last file pointer back by the size of the file to delete.
		fs->last_file_ptr -= file_size + FILE_ENTRY_SIZE;
	}

	// Update the file system state structure.
	fs->space_used -= file_size + FILE_ENTRY_SIZE;
	fs->number_of_files--;
	write(FILE_SYSTEM, sizeof(file_system));

	// The address of the next file.
	uint16_t next_addr = addr + FILE_ENTRY_SIZE + file_size;

	// The number of bytes to read to fall on a page boundary.
	uint8_t bytes_read = (file_size + FILE_ENTRY_SIZE ) % PAGE_SIZE;

	/* Move back the files that follow the one that has been deleted. This loop
	 * will not be run if the last file was deleted.*/
	for(; next_addr < end; bytes_read = PAGE_SIZE )
	{
		read(next_addr, bytes_read);
		write(addr, bytes_read);
		next_addr += bytes_read;
		addr += bytes_read;
	}

	return 0; // Successfully deleted the file.
}

#if UPLOAD_FROM_WEB

// Copied from eeprom.xhtm.

#define CONTENT \
"<html>\n\
	<head>\n\
		<title>24LCxx EEPROM configuration</title>\n\
		<script type=\"text/javascript\">\n\
			function status(text){var status=document.getElementById(\"status\");status.innerHTML=text;}\n\
			function format()\n\
			{\n\
				var answer=confirm(\"Are you sure you want to format the file system?\");\n\
				if(answer)\n\
				{\n\
					var fmt = new XMLHttpRequest();\n\
					fmt.open(\"DELETE\", document.URL, false);\n\
					fmt.send()\n\
					if(fmt.status==200){status(\"Formatted\");}\n\
					else{status(\"Failed!\");}\n\
				}\n\
			}\n\
			function upload()\n\
			{\n\
				var file=document.getElementById(\"file\").value.match(RegExp('[\\\\s\\\\S]{1,'+50+'}','g'));\n\
				if(!file){ alert(\"No content provided!\"); return;}\n\
				var len=file.length;\n\
				status(\"Uploaded 0/\"+len);\n\
				function upload_part()\n\
				{\n\
					var ajax_obj=new XMLHttpRequest();\n\
					ajax_obj.onreadystatechange= function()\n\
					{\n\
						if(ajax_obj.status==200&&ajax_obj.readyState==4)\n\
						{\n\
							if(file.length)\n\
							{\n\
								status(\"Uploaded \"+ (len-file.length) +'/'+len);\n\
								upload_part();\n\
							}\n\
							else{status.(\"Done\");}\n\
						}\n\
						else if(ajax_obj.status==404){status(\"Failed!\");}\n\
					}\n\
					ajax_obj.open(\"POST\", document.URL + \"/conf.xhtml\", true);\n\
					ajax_obj.send(file.shift());\n\
				}\n\
				upload_part();\n\
			}\n\
		</script>\n\
	</head>\n\
	<body>\n\
		<h1>Upload configuration HTML file</h1>\n\
		This page is used to upload the configuration HTML file to the file system. To further access file system<br/>\n\
		functions, a more detailed HTML file should be used.<br/>\n\
		<button type=\"button\" onclick=\"format()\">Format</button>\n\
		<button type=\"button\" onclick=\"upload()\">Upload</button>\n\
		<i><span id=\"status\"></span></i><br/>\n\
		<textarea cols=\"100\" rows=\"320\" id=\"file\"></textarea>\n\
	</body>\n\
</html>"
#define CONTENT_SIZE sizeof(CONTENT) - 1

static char content_P[] PROGMEM = CONTENT;

File* EEPROM_24LCXX::http_get(void)
{
	return new PGMSpaceFile(content_P, CONTENT_SIZE);
}
#endif

/// The JSON array giving file system statistics.
#define STATS \
"{\"space_used\":~,\"files\":[~]}"

/// The size of the STATS string.
#define STATS_SIZE sizeof(STATS) - 1

/// The STATS string is stored in program memory to save RAM space.
static const char stats_P[] PROGMEM = STATS;

File* EEPROM_24LCXX::get_stats(void)
{
	// Create a new file.
	File* f = new PGMSpaceFile(stats_P, STATS_SIZE);

	if(!f) // If a new file could not be allocated.
	{
		return NULL; // Failed to render the stats.
	}

	Template* t = new Template(f); // Create a new template with the file.

	if(!t) // If the template could not be allocated.
	{
		delete f; // Delete the file.
		return NULL; // Failed to render the stats.
	}

	// Read the file system state structure.
	read(FILE_SYSTEM, sizeof(file_system));
	file_system* fs = (file_system*)page_buffer;

	// Add the space used numerical argument to the template.
	t->add_narg(fs->space_used);

	char* val = NULL; // This variable will contain the list of files.

	if(fs->number_of_files) // If there are files.
	{
		/* Build a JSON array containing the file names:
		 * "file_name_1","file_name_2","file_name_3" */

		// The size of the file list array string.
		uint8_t size = 0; // The last \0 will be accounted for by the last ","

		// The total number of files in the file system.
		uint8_t number_of_files = fs->number_of_files;

		uint16_t addr; // The address of the current file.

		// For each file in the file system.
		for(uint8_t i = 0; i < number_of_files; i++)
		{
			find_file(i, &addr); // Find the file by index.

			/* Add the size of the file's name to the total. Add 3 for each file
			 * for the , and the two " */
			size += strlen(((file_entry*)page_buffer)->name) + 3;
		}

		// Allocate a string to contain the list of files.
		val = (char*)ts_malloc(size);

		if(!val) // If the string could not be allocated.
		{
			// Attempt to allocate a smaller string to render the stats anyway.
			val = (char*)ts_malloc(2);

			if(!val) // If allocation of the string failed.
			{
				// Delete all created objects so far.
				delete f;
				delete t;

				return NULL; // Failed to render the stats.
			}

			// Render a white space.
			val[0] = ' ';
			val[1] = '\0';
		}
		else // String allocation worked.
		{
			// Fill the string with the file names.

			// For each file in the file system.
			for(uint8_t i = 0, pos = 0; i < number_of_files; i++)
			{
				find_file(i, &addr); // Find the file.

				val[pos++] = '\"'; // Start the name with a quotation mark.

				// Copy the file name to the value.
				strcpy(&val[pos], ((file_entry*)page_buffer)->name);

				// Move the value pointer.
				pos += strlen(((file_entry*)page_buffer)->name);

				val[pos++]='\"'; // End the name with a quotation mark.
				val[ pos++ ]= ','; // Put a comma between file names.
			}

			// Replace the last , with a null character for string termination.
			val[size - 1] = '\0';
		}
	}

	t->add_arg(val);

	return t;
}

Response::status_code EEPROM_24LCXX::process( Request* request, Response* response )
{
	print_transaction(request); // Print the transaction for debugging.

	// Files are assumed as being non existent.
	Response::status_code sc = NOT_FOUND_404;

	if(!request->to_destination()) // If the request is at destination.
	{
		if(request->is_method(Request::DELETE)) // If the request is a DELETE.
		{
			/* TOO DANGEROUS;
			format_file_system();
#if UPLOAD_FROM_WEB
			goto get;
#else
			sc = OK_200;
#endif*/
			sc = NOT_IMPLEMENTED_501;
		}

#if UPLOAD_FROM_WEB
		if(request->is_method(Request::GET)) // If the request is a get.
		{
			get:
			File* body = http_get(request); // Get the body of the response.
			if(!body) // If the body could not be allocated.
			{
				sc = INTERNAL_SERVER_ERROR_500; // Server error.
				return; // Cannot proceed further.
			}
			else
			{
				sc = OK_200; // The request was processed.
			}

			// Set the body of the response.
			response->set_body(body, MIME::TEXT/HTML);
		}
#endif

		else
		{
			sc = NOT_IMPLEMENTED_501; // Method is not implemented.
		}
	}
	/* If the request is one resource before its destination, a file is
	 * being requested. */
	else if(request->to_destination() == 1)
	{
		uint16_t addr; // The address of the file.

		request->next(); // Move the request to get the name of the file.

		if(request->is_method(Request::GET)) // If this is an HTTP GET.
		{
			// If the request is for the special "stats" resource.
			if(!strcmp(request->current(), "stats"))
			{
				File* f = get_stats(); // Get the content of stats.

				if(!f) // If the response string could not be allocated.
				{
					sc = INTERNAL_SERVER_ERROR_500;
					/* No return here, the response can be sent anyway with the
					 * correct MIME type. */
				}
				else
				{
					sc = OK_200;
				}

				// Set the response body.
				response->set_body(f, MIME::APPLICATION_JSON);

				return sc; // Request processed.
			}

			get_file:

			find_file(request->current(), &addr); // Find the requested file.

			if(!addr) // If the file could not be found.
			{
				return NOT_FOUND_404; // Request failed.
			}
			else
			{
				// Get the content of the file on the EEPROM.
				File* file = new EEPROMFile(this, addr + FILE_ENTRY_SIZE, ((file_entry*)page_buffer)->size);

				if(!file) // IF the file could not be allocated.
				{
					sc = INTERNAL_SERVER_ERROR_500;
					/* No return here, the response can be sent anyway with the
					 * correct MIME type. */
				}
				else
				{
					sc = OK_200;
				}

				/* Set the response body but do not set a mime type,
				 * let the client find it using the file extension. */
				response->set_body(file, NULL);
			}
		}

#if UPLOAD_FROM_WEB
		else if(request->is_method(Request::POST)) // If this is a POST request.
		{
			find_file(request->current(), &addr); // Find the requested file.

			// If the file does not exist, create it.
			if(!addr && create_file(request->current()))
			{
				sc = INTERNAL_SERVER_ERROR_500;
			}
			else
			{
				if(!addr) // If the file was just created.
				{
					find_file(request->current(), &addr); // Find it.
				}

				if(request->get_body()) // If the request has a body.
				{
				    // Append it to the file.
					append_to_file(addr, request->get_body());
				}

				goto get_file; // Return the content of the file.
			}
		}
		else if(request->is_method(Request::DELETE)) // If this is a DELETE request.
		{
			find_file(request->current(), &addr); // Find the file to delete.

			if(addr) // If the file exists.
			{
				delete_file(addr); // Delete the file.
				sc = GONE_410; // Return a GONE status code.
			}
		}
#endif

	}

	return sc;
}

uint8_t EEPROM_24LCXX::write(uint16_t addr, uint8_t len)
{
	/// WTF!!!!!!!!!!!!!!!!!!!!
	/* This block is necessary for the file system record to correctly save on
	 * the eeprom. It could have something to do with timing, where the code is being
	 * to fast in switching between read and writes. */
	DEBUG_TPRINTLN(len, DEC);
	file_system* fs = (file_system*)page_buffer;
	//DEBUG_PRINT('-');
	DEBUG_TPRINTLN(fs->space_used, DEC);

	uint8_t n = 0; // The number of write iterations.
	uint8_t twst; // Two Wire bus state.
	uint8_t wrote = 0; // The number of bytes written.

	// If we are attempting to write more bytes than the page buffer can contain.
	if(len > PAGE_SIZE || len == 0)
	{
		return 0; // Error.
	}

	restart: // Restart the transmission.

	if(n++ >= MAX_ITER) // If the maximum number of iterations has been reached.
	{
		ERROR_PRINTLN_P("Write timeout!");

		/** TODO Error code not supported, the number of bytes written should
		 * be returned */
		return -1;
	}

	begin: // Begin the transmission.

	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); // Send start condition.
	while((TWCR & _BV(TWINT)) == 0); // Wait for transmission.

	switch((twst = TW_STATUS))
	{
		case TW_REP_START: // OK, but should not happen.
		case TW_START:
			break; // All good.

		case TW_MT_ARB_LOST: // Bus arbitration lost.
			goto begin; // Attempt to gain arbitration.

		default: // Illegal bus state.
			/// TODO error code not supported.
			return 1; // error: not in start condition.
			// NB: do /not/ send stop condition
	}

	// Send SLA+W.
	TWDR = DEVICE_ADDRESS | TW_WRITE;
	TWCR = _BV(TWINT) | _BV(TWEN); // Clear interrupt to start transmission.
	while((TWCR & _BV(TWINT)) == 0); // Wait for transmission.

	switch((twst = TW_STATUS))
	{
		case TW_MT_SLA_ACK:
			break; // All good.

		case TW_MT_SLA_NACK: // NACK during select: device busy writing.
			goto restart; // Restart transmission.

		case TW_MT_ARB_LOST:
			goto begin; // Re-arbitrate.

		default: // Illegal bus state.
			goto error;	// Must send stop condition.
	}

	TWDR = addr >> 8;	// Send the low 8 bits of the address.
	TWCR = _BV(TWINT) | _BV(TWEN); // Clear interrupt to start transmission.
	while((TWCR & _BV(TWINT)) == 0); // Wait for transmission.

	switch((twst = TW_STATUS))
	{
		case TW_MT_DATA_ACK:
			break; // All good.

		case TW_MT_DATA_NACK: // Device sent a NACK.
			goto quit; // Terminate transmission.

		case TW_MT_ARB_LOST:
			goto begin; // Re-arbitrate.

		default:
			goto error; // Must send stop condition.
	}

	TWDR = addr; // Send the high 8 bits of the address.
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	while((TWCR & _BV(TWINT)) == 0); // Wait for transmission.
	switch((twst = TW_STATUS))
	{
		case TW_MT_DATA_ACK:
			break; // All good.

		case TW_MT_DATA_NACK:
			goto quit; // Transmission failed.

		case TW_MT_ARB_LOST:
			goto begin; // Re-arbitrate.

		default:
			goto error; // Must send stop condition.
	}

	for(; len > 0; len--) // For each byte to write.
	{
		TWDR = page_buffer[wrote++]; // Set the byte to write in the data register.
		TWCR = _BV(TWINT) | _BV(TWEN); // Clear interrupt to start transmission.
		while((TWCR & _BV(TWINT)) == 0); // wait for transmission.

		switch((twst = TW_STATUS))
		{
			case TW_MT_DATA_NACK:
				goto error;	// The device is write protected.

			case TW_MT_DATA_ACK:
				break; // All good.

			default:
				goto error; // Something went wrong.
		}

		if(!(++addr % PAGE_SIZE)) // If a page boundary has been crossed.
		{
			TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); // Send stop condition.
			/* Move the remainder of what needs to be written to the beginning
			 * of the page buffer.*/
			memcpy(page_buffer, page_buffer + wrote, --len);

			VERBOSE_PRINTLN_P("crossed");

			// Nested call to write the remainder of the page.
			wrote += write(addr, len);

			// Stop condition will have been sent in the previous call to write.
			goto quit_no_stop;
		}
	}

	quit: // Terminate transmission.

	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); // Send stop condition.

	quit_no_stop:

	/*VERBOSE_PRINT_P("Wrote ");
	VERBOSE_TPRINT(wrote, DEC);
	VERBOSE_PRINT_P(" at 0x");
	VERBOSE_TPRINTLN(addr - wrote, HEX);*/

	return wrote; // Return the number of bytes written.

	error:

	ERROR_PRINTLN_P("Write error!");

	goto quit; // Terminate transmission.

}

uint8_t EEPROM_24LCXX::read(uint16_t addr, uint8_t len)
{
	uint8_t n = 0; // The number of read iterations.
	uint8_t twst; // The Two Wire Interface state.
	uint8_t twcr = 0; // The Two Wire Interface control register.
	uint8_t rec = 0; // Number of bytes received.

	// If we are attempting to read more bytes than the page buffer can contain.
	if(len > PAGE_SIZE || len == 0)
	{
		return 0; // Nothing was read.
	}

	restart: // Restart the transmission from the beginning.

	if(n++ >= MAX_ITER) // If the maximum number of iterations has been reached.
	{
		ERROR_PRINTLN_P("Read timeout!");
		return -1; // Timeout.
	}

	begin: // Begin transmission.

	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); // Send start condition.
	while(!(TWCR & _BV(TWINT))); // Wait for transmission.

	switch((twst = TW_STATUS))
	{
		case TW_REP_START: // OK, but should not happen.
		case TW_START:
				break; // All good.

		case TW_MT_ARB_LOST:
			goto begin; // Arbitration has been lost.

		default: // Illegal bus state.
			return -1; // Error: not in start condition.
			// NB: do not send stop condition.
	}

	// Send SLA+W.
	TWDR = DEVICE_ADDRESS | TW_WRITE;
	TWCR = _BV(TWINT) | _BV(TWEN); // Clear interrupt to start transmission.
	while(!(TWCR & _BV(TWINT))); // Wait for transmission.

	switch((twst = TW_STATUS))
	{
		case TW_MT_SLA_ACK:
			break; // All good.

		case TW_MT_SLA_NACK: // NACK during select: device busy writing.
			goto restart;

		case TW_MT_ARB_LOST:
			goto begin; // Arbitration has been lost.

		default: // Illegal bus state.
			goto error;	// Must send stop condition.
	}

	TWDR = (addr >> 8);	// 16-bit word address device, send high 8 bits of addr.
	TWCR = _BV(TWINT) | _BV(TWEN);// Clear interrupt to start transmission.
	while(!(TWCR & _BV(TWINT))); // Wait for transmission.

	switch((twst = TW_STATUS))
	{
		case TW_MT_DATA_ACK:
			break; // All good.

		case TW_MT_DATA_NACK:
			goto quit; // Address invalid.

		case TW_MT_ARB_LOST:
			goto begin; // Arbitration has been lost.

		default: // Illegal bus state.
			goto error;		/* must send stop condition */
	}


	TWDR = addr; // Low 8 bits of addr.
	TWCR = _BV(TWINT) | _BV(TWEN);// Clear interrupt to start transmission.
	while(!(TWCR & _BV(TWINT))); // Wait for transmission.

	switch((twst = TW_STATUS))
	{
		case TW_MT_DATA_ACK:
			break; // All good.

		case TW_MT_DATA_NACK:
			goto quit; // Address invalid.

		case TW_MT_ARB_LOST:
			goto begin; // Arbitration has been lost.

		default: // Illegal bus state.
			goto error;	// Must send stop condition.
	}

	// Next cycle(s): master receiver mode
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); // Repeat start condition.
	while(!(TWCR & _BV(TWINT))); // Wait for transmission.

	switch((twst = TW_STATUS))
	{
		case TW_START: // OK, but should not happen.
		case TW_REP_START:
			break; // All good.

		case TW_MT_ARB_LOST:
			goto begin; // Arbitration has been lost.

		default: // Illegal bus state.
			goto error;	// Must send stop condition.
	}

	// Send SLA+R.
	TWDR = DEVICE_ADDRESS | TW_READ;
	TWCR = _BV(TWINT) | _BV(TWEN); // Clear interrupt to start transmission.
	while((TWCR & _BV(TWINT)) == 0); // Wait for transmission.

	switch((twst = TW_STATUS))
	{
		case TW_MR_SLA_ACK:
			break; // All good.

		case TW_MR_SLA_NACK:
			goto quit;

		case TW_MR_ARB_LOST:
			goto begin; // Arbitration has been lost.

		default: // Illegal bus state.
			goto error;	// Must send stop condition.
	}

	// For each byte to read.
	for(twcr = _BV(TWINT) | _BV(TWEN) | _BV(TWEA) ; len > 0; len--)
	{
		if(len == 1) // If this is the last byte to read.
		{
			twcr = _BV(TWINT) | _BV(TWEN); // Send NAK this time.
		}

		TWCR = twcr; // Clear interrupt to start transmission.
		while((TWCR & _BV(TWINT)) == 0); // Wait for transmission.

		switch((twst = TW_STATUS))
		{
			case TW_MR_DATA_NACK:
			case TW_MR_DATA_ACK:
				page_buffer[rec++] = TWDR; // Save the received byte.
				if(twst == TW_MR_DATA_NACK) // If this was the last byte.
				{
					goto quit; // Done reading.
				}
				break;

			default: // Illegal bus state.
				goto error;	// Must send stop condition.
		}
	}

	quit: // Terminate transmission.
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); // Send stop condition.

	/*VERBOSE_PRINT_P("Read ");
	VERBOSE_TPRINT(rec, DEC);
	VERBOSE_PRINT_P(" at 0x");
	VERBOSE_TPRINTLN(addr, HEX);*/

	return rec; // Return the number of bytes read.

	error:
	ERROR_PRINTLN_P("Read error!");
	goto quit; // Terminate transmission.
}

#if UPLOAD_FROM_UART
void reply(uint8_t code)
{
	/*Setting UDR0 twice appears to work. This is probably an easy fix for a bug
	 * somewhere else in the code.*/
	//UDR0 = code;
	UCSR0A |= _BV(TXC0);

	/*Sets the code to send in the transmit buffer. This is done before the USART
	 * transmitter is enabled so as to overwrite anything that might have been put
	 * there by PRINT statements.*/
	UDR0 = code;

	UCSR0B |= _BV(TXEN0); // Turn on the UART.
	loop_until_bit_is_set(UCSR0A, TXC0); // Wait for the code to transmit.
	UCSR0B &= ~(_BV(TXEN0)); // Turn off the UART.
}

void EEPROM_24LCXX::receive_from_uart(char c)
{
	UCSR0B &= ~(_BV(TXEN0)); /* This disables the UART, effectively preventing PRINT functions
	from operating. The reason for that is to prevent asynchronous UART usage during an
	interrupt and to allow us to send control data unobstructed by debug messages. */

	// If it has been 2 seconds since we last received data.
	if(is_expired(last_rx + SECONDS(2)))
	{
		uart_state = CMD; // Resets the protocol state.
		uart_pos = 0; // Resets the protocol packet position counter.
	}

	if(!uart_buffer) // If there is no buffer allocated for the protocol.
	{
		// Allocates a buffer for the protocol.
		uart_buffer = (char*)ts_malloc(UART_BUFFER_SIZE);

		if(!uart_buffer) // If the allocation failed.
		{
			return;  // Failed, let the sending end time out.
		}
	}

	uart_buffer[uart_pos++] = c; //Saves the received char in the buffer.
	last_rx = get_uptime(); // Update the last received data timer.

	if(uart_pos == UART_BUFFER_SIZE) //If the buffer is full.
	{
		uart_pos = 0; //Resets the protocol packet position counter.

		/*This means that we have received a complete packet.The state we are at in
		 * the protocol defines what will happen next. Packets are always padded
		 * to UART_BUFFER_SIZE. */
		switch(uart_state)
		{
			case CMD: // A command packet was received
				if(!strcmp(uart_buffer, "fmt")) // If the command is a format.
				{
					format_file_system(); // Format the file system.
					reply(ACK); // Send an acknowledgment.
				}
				else
				{
					// Else the command is either a delete or an append (dat).
					/* Finds the file we will be working with, its name is stored
					 * within the protocol buffer after the command and save
					 * its address in working_addr.*/
					find_file(uart_buffer + 4, &working_addr);

					if(!strcmp(uart_buffer, "dat")) // If the command is an append.
					{
						if(!working_addr) // If the file was not found.
						{
							 /* This means it does not exist, so create it and
							 * check if there is enough space for it.*/
							if(create_file(uart_buffer + 4))
							{
								reply(FILE_TOO_BIG);
								break; // Not enough space on the EEPROM.
							}

							/* Finds the file so we can get its address, if
							 * create file did return the address the file was created
							 * at, we would not need to call this.*/
							find_file(uart_buffer + 4, &working_addr);
						}

						uart_state = DATA; // Change the protocol state to DATA.
						reply(ACK); // Send an ack nowledgment.
						break;
					}
					else if(!working_addr) // If the file was not found.
					{
						reply(FILE_NOT_FOUND); // File not found.
					}
					else if(!strcmp(uart_buffer, "del")) // If the command is a delete
					{
						delete_file(working_addr); // Delete the file we found.
						reply(ACK); // Send an acknowledgment.
					}
					else
					{
						reply(UNKNOWN_CMD); // Unknown command.
					}
				}

				break;
			case DATA: // A data packet was received.
				if(working_addr) // If there is a file to receive the data.
				{
					// Create a memfile from the protocol buffer.
					MemFile f(uart_buffer, UART_BUFFER_SIZE, false);

					/* Pass that memfile along with the working address (set during the CMD state)
					 * so that data can get append to the file. If this function returns 1,
					 * this means there is no longer enough space on the EEPROM.*/
					if(append_to_file(working_addr, &f))
					{
						reply(FILE_TOO_BIG); //Not enough space on the EEPROM.
					}
					else
					{
						reply(ACK); //Send an acknowledgment.
					}
				}
				else
				{
					reply(PROTOCOL_ERROR); //We are in the wrong state.
				}
				break;

			default: break;
		}

		/* Something failed or the command completed. In either cases, the buffer
		 * is no longer needed so it is freed and its pointer is reset.*/
		ts_free(uart_buffer);
		uart_buffer = NULL;
	}

	UCSR0B |= _BV(TXEN0); // Enables the UART for general use again.
}

#include <avr/interrupt.h">
ISR(USART_RX_vect) // UART reception interrupt routine.
{
	instance->receive_from_uart(UDR0);
}
#endif
