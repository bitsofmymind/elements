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
#define UPLOAD_FROM_UART 1
#define UART_BUFFER_SIZE 20

class EEPROM_24LCXX: public Resource
{
	private:
		friend class EEPROMFile;
		char page_buffer[PAGE_SIZE];
#if UPLOAD_FROM_UART
		char* uart_buffer;
		enum uart_state_tp { CMD, DATA} uart_state;
		volatile uint8_t uart_pos;
		uint16_t working_addr;
		volatile uptime_t last_rx;
#endif

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
#if UPLOAD_FROM_UART
		void receive_from_uart(char c);
#endif

	protected:
		uint8_t format_file_system(void);
		uint8_t create_file(const char* name);
		uint8_t find_file(const char* name, uint16_t* entry_addr);
		uint8_t find_file(uint8_t index, uint16_t* entry_addr);
		uint8_t append_to_file(uint16_t addr, File* content);
		uint8_t delete_file(uint16_t addr);

		uint8_t write(uint16_t addr, uint8_t len);
		uint8_t read(uint16_t addr, uint8_t len);

		Response* http_get(Request* request);
		Response* get_stats(Request* request);
		virtual Response::status_code process( Request* request, Message** return_message );

};

#if UPLOAD_FROM_UART
static EEPROM_24LCXX* instance;
#endif

#endif /* EEPROM_24LCXX_H_ */
