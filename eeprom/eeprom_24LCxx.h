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

/// The maximum size of a file name.
#define FILE_NAME_MAX_SIZE 32 - 1 - sizeof(uint16_t)

/// The maximum size of a page.
#define PAGE_SIZE 64

/// The size of the EEPROM in bytes.
#define EEPROM_SIZE 32768

/// Turns on manipulation of files on the EEPROM using HTTP methods.
#define UPLOAD_FROM_WEB 0

/// Turns on manipulation of files on the EEPROM through the UART.
#define UPLOAD_FROM_UART 1

/// The size of the received buffer on the UART.
#define UART_BUFFER_SIZE 24

/// A Resource that interface with a file system on a 24LCXX EEPROM.
/** This class is a resource to interface with a 24LCXX EEPROM. Files on the
 * EEPROM can be accessed as nodes of this resource through a HTTP GET
 * /eeprom/file1.xml
 * /eeprom/file2.xml
 * and are arranged as a simple linear file system.
 *
 * By default, a call to the resource /stats returns a JSON page giving
 * statistics about the file system.
 *
 * When the constant UPLOAD_FROM_WEB is defined and set to 1, files can
 * also be added and deleted using HTTP POST and DELETE methods respectively.
 * eeprom.xhtml contains HTML and JavaScript example code to upload, create
 * and delete files through HTTP.
 *
 * When UPLOAD_FROM_UART is defined and set to 1, data can be uploaded to the
 * EEPROM using a simplistic protocol. The python script upload_to_eeprom.py
 * provides a command line tool to manipulate files on the EEPROM.
 *
 * The contents of the EEPROM are always exposed through a file system,
 * individual blocks and cannot be accessed or written to.
 * */
class EEPROM_24LCXX: public Resource
{
	private:

		// EEPROMFile needs access to file manipulation methods.
		friend class EEPROMFile;

		/// Contains the most recently read page.
		char page_buffer[PAGE_SIZE];

#if UPLOAD_FROM_UART

		/** UART file manipulation protocol response code for acknowledgment
		 * of the last command. */
		static const uint8_t ACK = '0';

		/** UART file manipulation protocol response code for an
		 * unknown command. */
		static const uint8_t UNKNOWN_CMD = '1';

		/** UART file manipulation protocol response code for a file that is
		 * too large. */
		static const uint8_t FILE_TOO_BIG = '2';

		/** UART file manipulation protocol response code for an io error. */
		static const uint8_t IO_ERROR = '3';

		/** UART file manipulation protocol response code if the requested file
		 * was not found. */
		static const uint8_t FILE_NOT_FOUND = '4';

		/** UART file manipulation protocol response code for a
		 * protocol error. */
		static const uint8_t PROTOCOL_ERROR = '5';

		/** UART file manipulation protocol response code used for
		 * debugging purposes. */
		static const uint8_t DEBUG = '6';

		/// A pointer to a buffer that received UART data.
		char* uart_buffer;

		/// The current state of the UART uploading routine.
		enum uart_state_tp {CMD, DATA} uart_state;

		/// The current position inside the UART buffer.
		volatile uint8_t uart_pos;

		/// The current working address inside the EEPROM.
		uint16_t working_addr;

		/// The last time something was received.
		volatile uptime_t last_rx;

#endif
		/// A structure that defines a file in the EEPROM.
		struct file_entry
		{
			/// The size of the file.
			uint16_t size;

			/// The name of the file.
			char name[FILE_NAME_MAX_SIZE];

			/// The end of a file entry, always a null character.
			/**
			 * This value is used to ensure that a file entry will always
			 * terminate with a null character.
			 * */
			char end;
		};

		/// A structure for storing the state of the file system.
		struct file_system
		{
			/** An identifier for the type of file system in use. Used
			to detect if the file system has been formatted.*/
			uint8_t id;

			/// A pointer to the start of the last file.
			uint16_t last_file_ptr;

			/// The amount of space used on the EEPROM.
			uint16_t space_used;

			/// The number of files on the file system.
			uint8_t number_of_files;
		};

	public:

		/// Class constructor.
		EEPROM_24LCXX();

#if UPLOAD_FROM_UART
		/// Received a character from the UART.
		/** This method is meant to be called from an interrupt routine.
		 * @oaram c the character received.
		 * */
		void receive_from_uart(char c);
#endif

	protected:

		/// Format the file system.
		/** The EEPROM file system should always be formated before usage.
		 * @return 0 if the operation was a success, 1 if it failed.
		 * */
		uint8_t format_file_system(void);

		/// Create a file.
		/**
		 * @param name the name of the file.
		 * @return 0 if the operation was a success, 1 if there is no
		 * more space.
		 * */
		uint8_t create_file(const char* name);

		/// Find a file by name.
		/**
		 * @param name the name of the file.
		 * @param entry_addr a pointer to an integer that will be set with
		 * the address of the file if it is found.
		 * @return 0 if the file was found, 1 if not.
		 * */
		uint8_t find_file(const char* name, uint16_t* entry_addr);

		/// Find a file by index.
		/**
		 * Since files are organized on the file system in a linear fashion,
		 * it is possible to retrieved files by index.
		 * @param index the index of the file.
		 * @param entry_addr a pointer to an integer that will be set with
		 * the address of the file if it is found.
		 * @return 0 if the file was found, 1 if not.
		 * */
		uint8_t find_file(uint8_t index, uint16_t* entry_addr);

		/// Append content to the end of a file.
		/**
		 * @param addr the address of the file.
		 * @param content the content to append to the file.
		 * @return 0 if the operation was a success, 1 if there was not enough
		 * space left on the device to append the data.
		 * */
		uint8_t append_to_file(uint16_t addr, File* content);

		/// Delete a file.
		/**
		 * @param addr the address of the file.
		 * @return 0 if the operation was a success, 1 if it failed.
		 * */
		uint8_t delete_file(uint16_t addr);

		/// Write the content of the page_buffer to the EEPROM.
		/** len cannot be larger than PAGE_SIZE.
		 * TODO the value return should be int8_t, to account for both bytes
		 * written and error codes.
		 * @param addr the address where to write the data.
		 * @param len the length of the data to write.
		 * @return the number of bytes written.
		 * */
		uint8_t write(uint16_t addr, uint8_t len);

		/// Read the content of the EEPROM to the page_buffer.
		/** len cannot be larger than PAGE_SIZE.
		 * TODO the value return should be int8_t, to account for both bytes
		 * read and error codes.
		 * @param addr the address where the data should be read.
		 * @param len the amount of data to read.
		 * @return the amount of bytes read.
		 * */
		uint8_t read(uint16_t addr, uint8_t len);

#if UPLOAD_FROM_WEB
		/// Returns the HTML page for this resource.
		/**
		 * @return the HTML file for this resource, NULL if it could not get
		 * allocated.
		 * */
		File* http_get(void);
#endif

		/// Returns file system informations as a JSON array.
		/**
		 * @return the statistics as a JSON array in a file. NULL if the file
		 * couln not get allocated.
		 * */
		File* get_stats(void);

        /// Process a request message.
        /**
         * Override of parent implementation to process file system queries
         * @param request the request to process.
         * @param response the response to fill if a response should be returned (which
         * depends on the status code).
         * @return the status_code produced while processing the request.
         */
		virtual Response::status_code process(Request* request, Response* response);
};

#if UPLOAD_FROM_UART
/// The current instance of the resource. Used by the interrupt routine.
static EEPROM_24LCXX* instance;
#endif

#endif /* EEPROM_24LCXX_H_ */
