/* eeprom_24LCxx.cpp - Implements a resource for interfacing with a 24_LCxx eeprom
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

//Inspired from <joerg@FreeBSD.ORG> 's twitest in avr-libc

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

/*TWI hardware defines*/
#define DEVICE_ADDRESS 0b10100000
#define MAX_ITER 200

#define SUCCESS 0
#define BUS_UNRESPONSIVE 1 /*Too many arbitration errors*/
#define DEVICE_WRITE_PROTECTED 2

/*File system defines*/
#define FILE_ENTRY_SIZE 			sizeof(file_entry)
#define ID 							0xAB

#define FILE_SYSTEM					0
#define FILE_SYSTEM_ID				FILE_SYSTEM
#define LAST_FILE_PTR				FILE_SYSTEM_ID + 1
#define SPACE_USED					LAST_FILE_PTR + 2
#define NUMBER_OF_FILES				SPACE_USED + 2
#define FIRST_FILE					FILE_SYSTEM + sizeof(file_system)

#define FILE_SIZE					0
#define FILE_NAME					FILE_SIZE + sizeof(uint16_t)
#define END							FILE_NAME + FILE_NAME_MAX_SIZE

EEPROM_24LCXX::EEPROM_24LCXX():
	Resource()
{
	VERBOSE_PRINTLN_P("EEPROM Starting...");
	/* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
	/* has prescaler (mega128 & newer) */
	TWSR = 0;

#if UPLOAD_FROM_UART
	instance = this;
	uart_pos = 0;
	uart_buffer = NULL;
	uart_state = CMD;
	last_rx = 0;
#endif


#if F_CPU < 3600000UL
	TWBR = 10;			/* smallest TWBR value, see note [5] */
#else
	TWBR = (F_CPU / 100000UL - 16) / 2;
#endif

	if(!read(FILE_SYSTEM_ID, sizeof(file_system)))
	{
		ERROR_PRINTLN_P("Read operation from EEPROM failed");
		return;
	}
	file_system* fs = (file_system*)page_buffer;
	if(fs->id != ID)
	{
		if(format_file_system())
		{
			return;
		}
	}
	else
	{
		VERBOSE_PRINTLN_P("File system detected");
	}

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
	page_buffer[0] = ID;
	file_system* fs = (file_system*)(page_buffer);
	fs->last_file_ptr = 0;
	fs->number_of_files = 0;
	fs->space_used = sizeof(file_system);
	if(!write(FILE_SYSTEM, sizeof(file_system)))
	{
		ERROR_PRINTLN_P("Formatting failed");
		return 1;
	}
	return 0;
}

uint8_t EEPROM_24LCXX::create_file(const char* name)
{
	VERBOSE_PRINT_P("Creating file \"");
	VERBOSE_PRINTLN(name);

	read(FILE_SYSTEM , sizeof(file_system));

	file_system* fs = (file_system*)page_buffer;
	uint16_t addr;

	if(fs->number_of_files)
	{
		addr = fs->space_used;
		if(EEPROM_SIZE - (addr + FILE_ENTRY_SIZE) < FILE_ENTRY_SIZE)
		{
			ERROR_PRINTLN_P("Not enough space for file");
			return 1;
		}
	}
	else
	{
		VERBOSE_PRINTLN_P("First file");
		addr = FIRST_FILE;
	}

	fs->last_file_ptr = addr;
	fs->number_of_files++;
	fs->space_used += FILE_ENTRY_SIZE;
	write(FILE_SYSTEM, sizeof(file_system));

	file_entry* fe = (file_entry*)page_buffer;
	fe->size = 0;
	fe->end  = '\0';
	strncpy(fe->name, name, FILE_NAME_MAX_SIZE);
	write(addr, FILE_ENTRY_SIZE);

	VERBOSE_PRINTLN_P("File created");
	return 0;
}

uint8_t EEPROM_24LCXX::find_file(const char* name, uint16_t* entry_addr)
{
	VERBOSE_PRINT_P("Finding file ");
	VERBOSE_PRINT(name);
	VERBOSE_PRINT_P(" ... ");
	read(LAST_FILE_PTR, 2);
	uint16_t last_file = *((uint16_t*)page_buffer);
	uint16_t addr = FIRST_FILE;
	uint8_t res;

	*entry_addr = 0;

	if(last_file != 0x0000)
	{
		while(true)
		{
			res = read(addr, FILE_ENTRY_SIZE);
			if(!res){ return res; }
			if(!strcmp(name, ((file_entry*)page_buffer)->name))
			{
				*entry_addr = addr;
				VERBOSE_PRINT_P("Found at 0x");
				VERBOSE_TPRINTLN(addr, HEX);
				return 0;
			}
			if(addr == last_file)
			{
				break;
			}
			addr += ((file_entry*)page_buffer)->size + FILE_ENTRY_SIZE;
		}
	}
	VERBOSE_PRINTLN_P("Not found");
	return 1;
}

uint8_t EEPROM_24LCXX::find_file(uint8_t index, uint16_t* entry_addr)
{
	VERBOSE_PRINT_P("Finding index ");
	VERBOSE_TPRINT(index, DEC);
	VERBOSE_PRINT_P(" ... ");
	read(LAST_FILE_PTR, 2);
	uint16_t last_file = *((uint16_t*)page_buffer);
	uint16_t addr = FIRST_FILE;
	uint8_t res;

	*entry_addr = 0;

	if(last_file != 0x0000)
	{
		for(uint8_t i = 0; i <= index; i++, addr += ((file_entry*)page_buffer)->size + FILE_ENTRY_SIZE)
		{
			res = read(addr, FILE_ENTRY_SIZE);
			if(!res){ return res; }
			if(addr > last_file)
			{
				return 1;
				VERBOSE_PRINTLN_P("Not found");
			}
		}
	}
	*entry_addr = addr;
	return 0;
}

uint8_t EEPROM_24LCXX::append_to_file(uint16_t addr, File* content)
{
	VERBOSE_PRINT_P("Appending to file at 0x");
	VERBOSE_TPRINT(addr, HEX);
	VERBOSE_PRINT_P(" with ");
	VERBOSE_TPRINT((uint16_t)content->size, DEC);
	VERBOSE_PRINTLN_P(" bytes");

	read(FILE_SYSTEM, sizeof(file_system));
	file_system* fs = (file_system*)page_buffer;

	uint16_t end = fs->space_used;

	if(EEPROM_SIZE - end < content->size)
	{
		ERROR_PRINTLN_P("Not enough space!");
		return 1;
	}

	if(addr != fs->last_file_ptr)
	{
		fs->last_file_ptr += content->size;
	}
	fs->space_used += content->size;

	write(FILE_SYSTEM, sizeof(file_system));

	read(addr + FILE_SIZE, sizeof(uint16_t));
	file_entry* fe = (file_entry*)page_buffer;
	uint16_t start = fe->size + FILE_ENTRY_SIZE + addr;
	fe->size += content->size;
	write(addr + FILE_SIZE, sizeof(uint16_t));

	for(uint8_t size_to_move = (end - start) % PAGE_SIZE; end > start; size_to_move = PAGE_SIZE)
	{
		end -= size_to_move;
		read(end, size_to_move);
		write(end + content->size, size_to_move);
	}

	for(uint8_t copied = PAGE_SIZE; copied == PAGE_SIZE ;)
	{
		copied = content->read(page_buffer, PAGE_SIZE);
		write(start, copied);
		start += copied;
	}

	return 0;
}
uint8_t EEPROM_24LCXX::delete_file(uint16_t addr)
{
	VERBOSE_PRINT_P("Deleting file at 0x");
	VERBOSE_TPRINTLN(addr, HEX);
	read(addr + FILE_SIZE, sizeof(uint16_t));
	uint16_t file_size = *((uint16_t*)page_buffer);

	read(FILE_SYSTEM, sizeof(file_system));
	file_system* fs = (file_system*)page_buffer;
	uint16_t end = fs->space_used;

	if(addr == FIRST_FILE && fs->number_of_files == 1){	fs->last_file_ptr = 0;}
	else if(addr == fs->last_file_ptr)
	{
		uint16_t current = FIRST_FILE;
		file_entry* fe = (file_entry*)page_buffer;
		for(uint8_t i = 1; i < fs->number_of_files - 1; i++)
		{
			read(current + FILE_SIZE, sizeof(uint16_t));
			current += FILE_ENTRY_SIZE + fe->size;
		}
		read(FILE_SYSTEM, sizeof(file_system));
		fs->last_file_ptr = current;
	}
	else
	{
		fs->last_file_ptr -= file_size + FILE_ENTRY_SIZE;
	}

	fs->space_used -= file_size + FILE_ENTRY_SIZE;
	fs->number_of_files--;

	write(FILE_SYSTEM, sizeof(file_system));

	uint16_t next_addr = addr + FILE_ENTRY_SIZE + file_size;
	uint8_t bytes_read = (file_size + FILE_ENTRY_SIZE ) % PAGE_SIZE;

	for(; next_addr < end; bytes_read = PAGE_SIZE )
	{
		read(next_addr, bytes_read);
		write(addr, bytes_read);
		next_addr += bytes_read;
		addr += bytes_read;
	}

	return 0;
}
#if UPLOAD_FROM_WEB
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

#define STATS \
"{\"space_used\":~,\"files\":[~]}"
#define STATS_SIZE sizeof(STATS) - 1

static const char stats_P[] PROGMEM = STATS;

File* EEPROM_24LCXX::get_stats(void)
{
	File* f = new PGMSpaceFile(stats_P, STATS_SIZE);
	if(!f)
	{
		return NULL;
	}

	Template* t = new Template(f);
	if(!t)
	{
		delete f;
		return NULL;
	}

	read(FILE_SYSTEM, sizeof(file_system));
	file_system* fs = (file_system*)page_buffer;
	t->add_narg(fs->space_used);
	char* val = NULL;
	if(fs->number_of_files)
	{
		uint8_t size = 0; //The last \0 will be accounted for by the last ","
		uint8_t number_of_files = fs->number_of_files;
		uint16_t addr;
		for(uint8_t i = 0; i < number_of_files; i++)
		{
			find_file( i, &addr );
			size += strlen(((file_entry*)page_buffer)->name) + 3/*For ',' and '""' */;
		}
		val = (char*)ts_malloc(size);
		if(!val)
		{
			val = (char*)ts_malloc(2);
			if(!val)
			{
				delete f;
				delete t;
				return NULL;
			}
			val[0] = ' ';
			val[1] = '\0';
		}
		else
		{
			for(uint8_t i = 0, pos=0; i < number_of_files; i++)
			{
				find_file( i, &addr );
				val[pos++]='\"';
				strcpy(&val[pos], ((file_entry*)page_buffer)->name);
				pos += strlen(((file_entry*)page_buffer)->name);
				val[pos++]='\"';
				val[ pos++ ]= ',';
			}
			val[size - 1] = '\0';
		}
	}
	t->add_arg(val);

	return t;
}

Response::status_code EEPROM_24LCXX::process( Request* request, Response* response )
{

	print_transaction(request);

	Response::status_code sc = NOT_FOUND_404;

	if(!request->to_destination())
	{
		if(request->is_method(Request::DELETE))
		{
			format_file_system();
#if UPLOAD_FROM_WEB
			goto get;
#else
			sc = OK_200;
#endif
		}
#if UPLOAD_FROM_WEB
		if(request->is_method(Request::GET))
		{
			get:
			File* body = http_get(request);
			if(!body)
			{
				sc = INTERNAL_SERVER_ERROR_500;
				return;
			}
			else { sc = OK_200;	}
			response->set_body(body, MIME::TEXT/HTML);
		}
#endif
		else { sc = NOT_IMPLEMENTED_501; }
	}
	else if(request->to_destination() == 1)
	{
		uint16_t addr;

		request->next();

		if(request->is_method(Request::GET))
		{
			if(!strcmp(request->current(), "stats"))
			{
				File* f = get_stats();
				if(!f)
				{
					sc = INTERNAL_SERVER_ERROR_500;
				}
				else { sc = OK_200;	}
				response->set_body(f, MIME::APPLICATION_JSON);
				return sc;
			}

			get_file:
			find_file(request->current(), &addr);
			if(!addr)
			{
				return NOT_FOUND_404;
			}
			else
			{
				File* file = new EEPROMFile(this, addr + FILE_ENTRY_SIZE, ((file_entry*)page_buffer)->size);
				if(!file)
				{
					sc = INTERNAL_SERVER_ERROR_500;
				}
				else { sc = OK_200; }
				response->set_body(file, NULL);
			}
		}
		else if(request->is_method(Request::POST))
		{

			find_file(request->current(), &addr);
			if(!addr && create_file(request->current()))
			{
				sc = INTERNAL_SERVER_ERROR_500;
			}
			else
			{
				if(!addr)
				{
					find_file(request->current(), &addr);
				}
				if(request->get_body())
				{
					append_to_file(addr, request->get_body());
				}
				goto get_file;
			}
		}
		else if(request->is_method(Request::DELETE))
		{
			find_file(request->current(), &addr);
			if(addr)
			{
				delete_file(addr);
				sc = GONE_410;
			}
		}
	}

	return sc;
}

uint8_t EEPROM_24LCXX::write(uint16_t addr, uint8_t len)
{

	uint8_t n = 0;
	uint8_t twst;
	uint8_t wrote = 0;

	if(len > PAGE_SIZE)
	{
		return 0;
	}

	restart:
	if (n++ >= MAX_ITER)
	{
		ERROR_PRINTLN_P("Write timeout!");
		return -1;
	}

	begin:

	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send start condition */
	while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */

	switch ((twst = TW_STATUS))
	{
		case TW_REP_START:		/* OK, but should not happen */
		case TW_START:
			break;

		case TW_MT_ARB_LOST: //Bus arbitration lost
			goto begin;

		default:
			return 1;		/* error: not in start condition */
				/* NB: do /not/ send stop condition */
	}

	/* send SLA+W */
	TWDR = DEVICE_ADDRESS | TW_WRITE;
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
	switch ((twst = TW_STATUS))
	{
		case TW_MT_SLA_ACK:
			break;

		case TW_MT_SLA_NACK:	/* nack during select: device busy writing */
			goto restart;

		case TW_MT_ARB_LOST:	/* re-arbitrate */
			goto begin;

		default:
			goto error;		/* must send stop condition */
	}

	TWDR = addr>>8;		/* low 8 bits of addr */
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
	switch ((twst = TW_STATUS))
	{
		case TW_MT_DATA_ACK:
			break;

		case TW_MT_DATA_NACK:
			goto quit;

		case TW_MT_ARB_LOST:
			goto begin;

		default:
			goto error;		/* must send stop condition */
	}

	TWDR = addr;
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
	switch ((twst = TW_STATUS))
	{
		case TW_MT_DATA_ACK:
			break;

		case TW_MT_DATA_NACK:
			goto quit;

		case TW_MT_ARB_LOST:
			goto begin;

		default:
			goto error;		/* must send stop condition */
	}

	for (; len > 0; len--)
	{
		TWDR = page_buffer[wrote++];
		TWCR = _BV(TWINT) | _BV(TWEN); /* start transmission */
		while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
		switch ((twst = TW_STATUS))
		{
			case TW_MT_DATA_NACK:
				goto error;		/* device write protected -- Note [16] */

			case TW_MT_DATA_ACK:
				break;

			default:
				goto error;
		}
		if(!(++addr % PAGE_SIZE))
		{
			/*We have just crossed a page boundary!*/
			TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */
			memcpy(page_buffer, page_buffer + wrote, --len);
			VERBOSE_PRINTLN_P("crossed");
			wrote += write(addr, len);
			goto quit_no_stop;
		}
	}
	quit:
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */

	quit_no_stop:

	/*VERBOSE_PRINT_P("Wrote ");
	VERBOSE_TPRINT(wrote, DEC);
	VERBOSE_PRINT_P(" at 0x");
	VERBOSE_TPRINTLN(addr - wrote, HEX);*/

	return wrote;

	error:
	ERROR_PRINTLN_P("Write error!");
	goto quit;

}
uint8_t EEPROM_24LCXX::read(uint16_t addr, uint8_t len)
{
	uint8_t n = 0;
	uint8_t twst;
	uint8_t twcr = 0;
	uint8_t rec = 0;

	if(len > PAGE_SIZE)
	{
		return 0;
	}
	else if(len == 0)
	{

	}

	restart:
	if (n++ >= MAX_ITER)
	{
		ERROR_PRINTLN_P("Read timeout!");
		return -1;
	}

	begin:
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send start condition */
	while (!(TWCR & _BV(TWINT))) ; /* wait for transmission */

	switch ((twst = TW_STATUS))
	{
		case TW_REP_START:		/* OK, but should not happen */
		case TW_START:
				break;

		case TW_MT_ARB_LOST:	/* Note [9] */
			goto begin;

		default:
			return -1;		/* error: not in start condition */
	/* NB: do /not/ send stop condition */
	}

	/* Note [10] */
	/* send SLA+W */
	TWDR = DEVICE_ADDRESS | TW_WRITE;
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	while (!(TWCR & _BV(TWINT))) ; /* wait for transmission */
	switch ((twst = TW_STATUS))
	{
		case TW_MT_SLA_ACK:
			break;

		case TW_MT_SLA_NACK:	/* nack during select: device busy wrVERBOSE_PRINTLN_P("asd");iting */
		/* Note [11] */
			goto restart;

		case TW_MT_ARB_LOST:	/* re-arbitrate */
			goto begin;

		default:
			goto error;		/* must send stop condition */
	}

	TWDR = (addr >> 8);		/* 16-bit word address device, send high 8 bits of addr */
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	while (!(TWCR & _BV(TWINT))) ; /* wait for transmission */
	switch ((twst = TW_STATUS))
	{
		case TW_MT_DATA_ACK:
			break;

		case TW_MT_DATA_NACK:
			goto quit;
		case TW_MT_ARB_LOST:
			goto begin;

		default:
			goto error;		/* must send stop condition */
	}


	TWDR = addr;		/* low 8 bits of addr */
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	while (!(TWCR & _BV(TWINT))) ; /* wait for transmission */
	switch ((twst = TW_STATUS))
	{
		case TW_MT_DATA_ACK:
			break;

		case TW_MT_DATA_NACK:
			goto quit;

		case TW_MT_ARB_LOST:
			goto begin;

		default:
			goto error;		/* must send stop condition */
	}

	/*
	* Note [12]
	* Next cycle(s): master receiver mode
	*/
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send (rep.) start condition */
	while (!(TWCR & _BV(TWINT))) ; /* wait for transmission */
	switch ((twst = TW_STATUS))
	{
		case TW_START:		/* OK, but should not happen */
		case TW_REP_START:
			break;

		case TW_MT_ARB_LOST:
			goto begin;

		default:
			goto error;
	}

	/* send SLA+R */
	TWDR = DEVICE_ADDRESS | TW_READ;
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
	switch ((twst = TW_STATUS))
	{
		case TW_MR_SLA_ACK:
			break;

		case TW_MR_SLA_NACK:
			goto quit;

		case TW_MR_ARB_LOST:
			goto begin;

		default:
			goto error;
	}


	for (twcr = _BV(TWINT) | _BV(TWEN) | _BV(TWEA) /* Note [13] */; len > 0; len--)
	{
		if (len == 1)
		{
			twcr = _BV(TWINT) | _BV(TWEN); /* send NAK this time */
		}
		TWCR = twcr;		/* clear int to start transmission */
		while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
		switch ((twst = TW_STATUS))
		{
			case TW_MR_DATA_NACK:
			case TW_MR_DATA_ACK:
				page_buffer[rec++] = TWDR;
				if(twst == TW_MR_DATA_NACK) { goto quit; }
				break;

			default:
				goto error;
		}
	}

	quit:
	/* Note [14] */
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */

	/*VERBOSE_PRINT_P("Read ");
	VERBOSE_TPRINT(rec, DEC);
	VERBOSE_PRINT_P(" at 0x");
	VERBOSE_TPRINTLN(addr, HEX);*/

	return rec;

	error:
	ERROR_PRINTLN_P("Read error!");
	goto quit;
}

#if UPLOAD_FROM_UART
void reply(uint8_t code)
{
	/*Setting UDR0 twice appears to work. This is probably an easy fix for a bug
	 * somewhere else in the code.*/
	//UDR0 = code;
	UCSR0A |= _BV(TXC0);
	/*Sets the code to send in the transmit buffer. This is done before the USART
	 * transmitter is enables so as to overwrite anything that might have been put
	 * there by PRINT statements.*/
	UDR0 = code;
	UCSR0B |= _BV(TXEN0);
	loop_until_bit_is_set(UCSR0A, TXC0);
	UCSR0B &= ~(_BV(TXEN0));
}

void EEPROM_24LCXX::receive_from_uart(char c)
{
	UCSR0B &= ~(_BV(TXEN0)); /* This disables the UART, effectively preventing PRINT functions
	from operating. The reason for that is to prevent synchronous UART usage during an
	interrupt and to allow us to send control data unobstructed by debug messages.*/

	/*If it has been 2 seconds since we last received data.*/
	if(is_expired(last_rx + SECONDS(2)))
	{
		uart_state = CMD; //Resets the protocol state.
		uart_pos = 0; //Resets the protocol packet position counter.
	}
	/*If there is no buffer allocated for the protocol.*/
	if(!uart_buffer)
	{
		/*Allocates a buffer for the protocol.*/
		uart_buffer = (char*)ts_malloc(UART_BUFFER_SIZE);
		/*If the allocation failed, return.*/
		if(!uart_buffer){ return; }
	}
	uart_buffer[uart_pos++] = c; //Saves the received char in the buffer.
	last_rx = get_uptime(); //Update the last received data counter.

	if(uart_pos == UART_BUFFER_SIZE) //If the buffer is full.
	{
		uart_pos = 0; //Resets the protocol packet position counter.
		/*This means that we have received a complete packet.The state we are at in
		 * the protocol defines what will happen next.*/
		switch(uart_state)
		{
			case CMD: //A command packet was received
				if(!strcmp(uart_buffer, "fmt")) //If the command is a format.
				{
					format_file_system(); //Format the file system.
					reply(ACK); //Send an acnowledgment.
				}
				else
				{
					//Else the command is either a delete or an append (dat).
					/*Finds the file we will be working with, its name is stored
					 * within the protocol buffer after the command and save
					 * its address in working_addr.*/
					find_file(uart_buffer + 4, &working_addr);

					if(!strcmp(uart_buffer, "dat")) //If the command is an append.
					{
						if(!working_addr) //If the file was not found.
						{
							 /*This means it does not exist, so create it and
							 * check if there is enough space for it.*/
							if(create_file(uart_buffer + 4))
							{
								reply(FILE_TOO_BIG); //Not enough space on the eeprom.
								break;
							}

							/*Finds the file so we can get its address, if
							 * create file did return the address the file was created
							 * at, we would not need to call this.*/
							find_file(uart_buffer + 4, &working_addr);
						}

						uart_state = DATA; //Change the protocol state to DATA.
						reply(ACK); //Send an acnowledgment.
						break;
					}
					else if(!working_addr) //If the file was not found.
					{
						reply(FILE_NOT_FOUND); //File not found.
					}
					else if(!strcmp(uart_buffer, "del")) //If the command is a delete
					{
						delete_file(working_addr); //Delete the file we found.
						reply(ACK); //Send an acnowledgment.
					}
					else
					{
						reply(UNKNOWN_CMD); //Unknown command.
					}
				}
				break;
			case DATA: //A data packet was received.
				if(working_addr) //If there is a file to receive the data.
				{
					//Create a memfile from the protocol buffer.
					MemFile f(uart_buffer, UART_BUFFER_SIZE, false);
					/*Pass that memfile along with the working address (set during the CMD state)
					 * so that data can get append to the file. If this function returns 1,
					 * this means there is no longer enough space on the eeprom.*/
					if(append_to_file(working_addr, &f))
					{
						reply(FILE_TOO_BIG); //Not enough space on the eeprom.
					}
					else
					{
						reply(ACK); //Send an acnowledgment.
					}
				}
				else
				{
					reply(PROTOCOL_ERROR); //We are in the wrong state.
				}
				break;

			default: break;
		}

		/*Something failed or the command completed. In either cases, the buffer
		 * is no longer needed so it is freed and its pointer is reset.*/
		ts_free(uart_buffer);
		uart_buffer = NULL;
	}

	UCSR0B |= _BV(TXEN0); //Enables the UART for general use again.
}

#include "avr/interrupt.h"
ISR(USART_RX_vect)
{
	instance->receive_from_uart(UDR0);
}
#endif
