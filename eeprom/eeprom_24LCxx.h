/*
 * eeprom.h
 *
 *  Created on: 2011-04-19
 *      Author: antoine
 */

#ifndef EEPROM_24LCXX_H_
#define EEPROM_24LCXX_H_

#include <core/resource.h>

#define PAGE_SIZE 64
#define EEPROM_SIZE 32768

class EEPROM_24LCXX: public Resource
{
	private:
		friend class EEPROMFile;
		char page_buffer[PAGE_SIZE];

		struct file_entry
		{
			uint16_t size;
			char name[13];
			//The end of name is indicated by a 0
			char end;
		};
		struct file_system
		{
			uint8_t id;
			uint16_t last_file_ptr;
			uint16_t space_used;
			uint8_t number_of_files;
		};
	public:
		EEPROM_24LCXX();

		uint8_t format_file_system(void);
		uint8_t create_file(const char* name);
		uint8_t find_file(const char* name, uint16_t* entry_addr);
		uint8_t find_file(uint8_t index, uint16_t* entry_addr);
		uint8_t append_to_file(uint16_t addr, File* content);
		uint8_t delete_file(uint16_t addr);

	protected:

		uint8_t write(uint16_t addr, uint8_t len);
		uint8_t read(uint16_t addr, uint8_t len);

		Response* http_get(Request* request);
		Response* get_stats(Request* request);
		virtual Response::status_code process( Request* request, Message** return_message );
};

#endif /* EEPROM_24LCXX_H_ */
