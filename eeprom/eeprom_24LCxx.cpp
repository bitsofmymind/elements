/*
 * eeprom.cpp
 *
 *  Created on: 2011-04-19
 *      Author: antoine
 */

//Inspired from <joerg@FreeBSD.ORG> 's twitest in avr-libc

#include "eeprom_24LCxx.h"
#include "eeprom_file.h"
#include <avr_pal.h>
#include <avr/io.h>
#include <util/twi.h>
#include <string.h>
#include <utils/pgmspace_file.h>
#include <utils/template.h>
#include <stdlib.h>

/*TWI hardware defines*/
#define DEVICE_ADDRESS 0b10100000
#define MAX_ITER 200

#define SUCCESS 0
#define BUS_UNRESPONSIVE 1 /*Too many arbitration errors*/
#define DEVICE_WRITE_PROTECTED 2

/*File system defines*/
#define FILE_ENTRY_SIZE 			16
#define ID 							0xAB

#define FILE_SYSTEM					0
#define FILE_SYSTEM_ID				FILE_SYSTEM
#define LAST_FILE_PTR				FILE_SYSTEM_ID + 1
#define SPACE_USED					LAST_FILE_PTR + 2
#define NUMBER_OF_FILES				SPACE_USED + 2
#define FIRST_FILE					FILE_SYSTEM + sizeof(file_system)

#define FILE_SIZE					0
#define FILE_NAME					FILE_SIZE + sizeof(uint16_t)
#define END							FILE_NAME + 13

EEPROM_24LCXX::EEPROM_24LCXX():
	Resource()
{
	VERBOSE_PRINTLN_P("EEPROM Starting...");
	/* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
	/* has prescaler (mega128 & newer) */
	TWSR = 0;


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
	VERBOSE_NPRINTLN(fs->id, HEX);
	VERBOSE_PRINT_P("space used: ");
	VERBOSE_NPRINTLN(fs->space_used, DEC);
	VERBOSE_PRINT_P("number of files: ");
	VERBOSE_NPRINTLN(fs->number_of_files, DEC);
	VERBOSE_PRINT_P("last file: 0x");
	VERBOSE_NPRINTLN(fs->last_file_ptr, HEX);

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

uint8_t EEPROM_24LCXX::create_file( const char* name)
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
	strncpy(fe->name, name, 13);
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
				VERBOSE_NPRINTLN(addr, HEX);
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

uint8_t EEPROM_24LCXX::append_to_file(uint16_t addr, File* content)
{
	VERBOSE_PRINT_P("Modifying file at 0x");
	VERBOSE_NPRINT(addr, HEX);
	VERBOSE_PRINT_P(" with ");
	VERBOSE_NPRINT(content->size, DEC);
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
	VERBOSE_NPRINTLN(addr, HEX);
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

#define CONTENT \
"<html>\n\
	<head><title>24LCxx EEPROM file system</title></head>\n\
	<body>\n\
		<h2>24LCxx EEPROM file system</h2>\n\
		<br/>\n\
		<form name=\"fmtform\" method=\"post\">\n\
			<input type=\"hidden\" name=\"fmt\" value=\"1\"/>\n\
			<input type=\"button\" value=\"Format\" onclick=\"format()\"/>\n\
		</form>\n\
		<script type=\"text/javascript\">\n\
		function format()\n\
		{\n\
			var answer=confirm(\"Are you sure you want to format the file system?\");\n\
			if(answer){ document.fmtform.submit(); }\n\
		}\n\
		</script>\n\
		<h3>Informations</h3>\n\
		Number of Files:~<br/>\n\
		Space used:~\n\
		<h3>Upload file</h3>\n\
		<form>\n\
			Name:<input type=\"text\" id=\"file_name\" size=\"13\"/>\n\
			<input type=\"button\" value=\"Upload\" onclick=\"upload()\"/>\n\
			<i><span id=\"status\"></span></i><br/>\n\
			<textarea cols=\"100\" rows=\"320\" id=\"file\"></textarea>\n\
		</form>\n\
		<script type=\"text/javascript\">\n\
			function upload()\n\
			{\n\
				var name=document.getElementById(\"file_name\").value;\n\
				if(!name){ alert(\"No file name provided!\"); return;}\n\
				var status=document.getElementById(\"status\");\n\
				var file=document.getElementById(\"file\").value.match(RegExp('.{1,'+50+'}','g'));\n\
				if(!file){ alert(\"No content provided!\"); return;}\n\
				var len=file.length;\n\
				status.innerHTML=\"Uploaded 0/\"+len;\n\
				while(file.length)\n\
				{\n\
					var ajax_obj=new XMLHttpRequest();\n\
					ajax_obj.open(\"POST\", document.URL+\"/\"+name, false);\n\
					ajax_obj.send(file.shift());\n\
					status.innerHTML=\"Uploaded \"+ (len-file.length) +'/'+len;\n\
					if(ajax_obj.status==404){status.innerHTML=\"Failed!\"; return;}\n\
				}\n\
				status.innerHTML=\"Done\";\n\
			}\n\
		</script>\n\
	</body>\n\
</html>"
#define CONTENT_SIZE sizeof(CONTENT) - 1

static char content_P[] PROGMEM = CONTENT;

Response* EEPROM_24LCXX::http_get(Request* request)
{
	Response* response = new Response(OK_200, request);
	if(!response) {	return NULL; }

	File* f = new PGMSpaceFile(content_P, CONTENT_SIZE);
	if(!f)
	{
		response->original_request = NULL;
		delete response;
		return NULL;
	}

	Template* t = new Template(f);
	if(!t)
	{
		response->original_request = NULL;
		delete f;
		delete response;
		return NULL;
	}


	read(FILE_SYSTEM, sizeof(file_system));
	file_system* fs = (file_system*)page_buffer;
	char* val = (char*)ts_malloc(4);
	if(!val)
	{
		response->original_request = NULL;
		delete f;
		delete t;
		delete response;
		return NULL;
	}
	itoa(fs->number_of_files, val, 10);
	t->add_arg(val, strlen(val));

	val = (char*)ts_malloc(6);
	if(!val)
	{
		response->original_request = NULL;
		delete f;
		delete t;
		delete response;
		return NULL;
	}
	itoa(fs->space_used, val, 10);
	t->add_arg(val, strlen(val));


	response->body_file = t;
	response->content_length = t->size;
	response->content_type = "text/html";
	return response;
}

Response::status_code EEPROM_24LCXX::process( Request* request, Message** return_message )
{
	URL* url = request->to_url;
	Response::status_code sc = NOT_FOUND_404;

	if(url->cursor == url->resources.items)
	{
		if(!strcmp(request->method, "post"))
		{
			char format;
			uint8_t len = request->find_arg("fmt", &format, 1);
			if(len && format == '1')
			{
				format_file_system();
			}
			goto get;
		}
		else if(!strcmp(request->method, "get"))
		{
			get:
			*return_message = http_get(request);
			if(!*return_message)
			{
				sc = INTERNAL_SERVER_ERROR_500;
			}
			else { sc = OK_200;	}
		}
		else { sc = NOT_IMPLEMENTED_501; }
	}
	else if(url->cursor + 1 == url->resources.items)
	{
		uint16_t addr;

		if(!strcmp(request->method, "get"))
		{
			get_file:
			find_file(url->resources[url->cursor], &addr);
			if(!addr)
			{
				return NOT_FOUND_404;
			}
			else
			{
				Response* response = new Response(OK_200, request);
				if(!response) {	return INTERNAL_SERVER_ERROR_500; }

				File* file = new EEPROMFile(this, addr + FILE_ENTRY_SIZE, ((file_entry*)page_buffer)->size);
				if(!file)
				{
					response->original_request = NULL;
					delete response;
					sc = INTERNAL_SERVER_ERROR_500;
				}
				response->content_length = file->size;
				response->body_file = file;
				response->content_type = "text/html";
				*return_message = response;
				sc = OK_200;
			}

		}
		else if(!strcmp(request->method, "post"))
		{
			find_file(url->resources[url->cursor], &addr);
			if(!addr && create_file(url->resources[url->cursor]))
			{
				sc = INTERNAL_SERVER_ERROR_500;
			}
			else
			{
				if(!addr)
				{
					find_file(url->resources[url->cursor], &addr);
				}
				if(request->content_length > 0)
				{
					append_to_file(addr, request->body_file);
				}
				goto get_file;
			}
		}
		else if(!strcmp(request->method, "delete"))
		{
			find_file(url->resources[url->cursor], &addr);
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

	VERBOSE_PRINT_P("Wrote ");
	VERBOSE_NPRINT(wrote, DEC);
	VERBOSE_PRINT_P(" at 0x");
	VERBOSE_NPRINTLN(addr - wrote, HEX);

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

	VERBOSE_PRINT_P("Read ");
	VERBOSE_NPRINT(rec, DEC);
	VERBOSE_PRINT_P(" at 0x");
	VERBOSE_NPRINTLN(addr, HEX);

	return rec;

	error:
	ERROR_PRINTLN_P("Read error!");
	goto quit;
}
