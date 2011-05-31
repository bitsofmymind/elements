/* eeprom_24LCxx.h - Implements a resource for interfacing with a 24_LCxx eeprom
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

#ifndef EEPROM_24LCXX_H_
#define EEPROM_24LCXX_H_

#include <core/resource.h>

#define FILE_NAME_MAX_SIZE 21
#define PAGE_SIZE 64

#define EEPROM_SIZE 32768

#define UPLOAD_FROM_WEB 0

#define UPLOAD_FROM_UART 1
#define UART_BUFFER_SIZE sizeof(file_entry)

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
			char name[FILE_NAME_MAX_SIZE];
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
#if UPLOAD_FROM_WEB
		File* http_get(void);
#endif
		File* get_stats(void);
		virtual Response::status_code process( Request* request, File** return_body, const char** mime );

};

#if UPLOAD_FROM_UART
static EEPROM_24LCXX* instance;
#endif

#endif /* EEPROM_24LCXX_H_ */
