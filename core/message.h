/* message.h - Header file for the Message class.
 * Copyright (C) 2015 Antoine Mercier-Linteau
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

#ifndef MESSAGE_H_
#define MESSAGE_H_

//INCLUDES

#include <configuration.h>
#include <utils/utils.h>
#include <utils/file.h>
#include "url.h"
#include <pal/pal.h>

///@todo delete that if it is useless.
#ifndef MESSAGE_SIZE
	#define MESSAGE_SIZE uint32_t
#endif

/// Message is a base class for HTTP messages.
/**
 * Message implements common functionality between HTTP request and responses.
 * It takes care of functions such as header parsing, header field parsing,
 * body parsing and storage and defines methods that requests and responses
 * override to adapt to their specificities.
 * */
class Message
{
	public:

		/// The different types a message can be.
		enum TYPE { REQUEST, RESPONSE, UNKNOWN };

		/** The possible results of parsing a message.*/
		enum PARSER_RESULT
		{
			//NOT_PARSED,
			PARSING_COMPLETE = 0,
			PARSING_SUCESSFUL,
			LINE_INCOMPLETE,
			LINE_MALFORMED,
			LINE_OVERFLOW,
			BODY_OVERFLOW,
			HEADER_MALFORMED,
			OUT_OF_MEMORY,
			SIZE_IS_0
		};

		/**The type of dispatching of this message. Absolute messages
		 * always transit trough the root resource while relative messages
		 * are routed directly to their destination.*/
		enum DISPATCHING_TYPE { ABSOLUTE, RELATIVE, UNDETERMINED };

	private:

		/// Parsing controls, used to resume parsing when new data arrives.
		/**If we are parsing the body of the message.*/
		bool _parsing_body;

		/** Pointer to the current line we are parsing.*/
		char* _current_line; ///TODO rename current_line to line_buffer.

		/** The type this message is of. Implements a crude form of
		 * reflection.*/
		TYPE _type;

		/** The type of dispatching for this message.*/
		DISPATCHING_TYPE _dispatching_type;

#if MESSAGE_AGE
		/** The uptime at which the message was created. */
		uptime_t _age;
#endif

	protected:

		/** Pointer to the body of the message.*/
		File* body;

		/** The cursor for the destination url. The cursor is used to point
		 * to the current resource we are at within the destination url.*/
		uint8_t to_url_cursor;

		/** The cursor for the origin url. The cursor is used to point
		 * to the current resource we are at within the origin url.*/
		uint8_t from_url_cursor; //todo delete, not used.

		/** The url this message is going to.*/
		URL* to_url;

		/** The url this message is coming from. This url is built as the
		 * message is travelling in the resource tree. It is used to know send
		 * a response to this message by setting it as to to_url when the
		 * message has reached destination.*/
		URL* from_url;

		/** The Content-type of the message's body. Defined as an attribute
		 * field because is it very often needed.*/
		const char* content_type;

		Dictionary<const char*>* fields;

	public:

		/// Class constructor.
		Message();

		/// Class destructor.
		virtual ~Message();

		/** @return the uptime at which the message was created. */
		inline const uptime_t get_age(void) const { return _age; }

		/** @return the destination URL.*/
		inline URL* get_to_url(void) const { return to_url; }

		/** @return the source URL.*/
		inline URL* get_from_url(void) const { return from_url; }

		/** @return the type of the message. */
		inline Message::TYPE get_type(void) const { return _type; }

		/** @return the type of dispatching for this message. */
		inline DISPATCHING_TYPE get_dispatching_type(void) const { return _dispatching_type; }

		/** @return the type of dispatching for this message. */
		inline void set_dispatching_type(DISPATCHING_TYPE type) { _dispatching_type = type; }

		/// Prints the contents of the message to the output.
		/**
		 * Prints the contents of the message to the output. Sub-classes of Message
		 * should override this method to print their own data. If VERBOSITY,
		 * OUTPUT_WARNINGS or OUTPUT_ERRORS are not defined, this method should be
		 * optimized away by the compiler.
		 * */
		virtual void print() const;

		/// Serialize the message to a buffer.
		/**
		 * Serialize the message to a buffer and/or returns the length in bytes of the
		 * serialized message. Simply returning the length is useful for allocating a
		 * buffer to which the message is then serialized to. Sub-classes of Message
		 * should override this method to serialize their own headers and fields,
		 * hence, this method should only be called within a child class.
		 * @param buffer the buffer to serialize the message to.
		 * @param write if the data should be written to the buffer. If set to false,
		 * 		only the length of the serialized message will be returned.
		 * @return if write is true, the number of bytes written to the buffer, if
		 * 		write is false, the length of the serialized message.
		 * */
		virtual size_t serialize(char* buffer, bool write) const;

		/// Parse the content of a buffer with a known size into a message.
		/**
		 * Parses the content of a buffer into this message object. This method assumes
		 * that the size of the buffer is known and can be called on multiple
		 * message parts for cases where they are received serially. Parsing is done
		 * by waiting for complete HTTP lines (ended by CRLF) and parsing them at once.
		 * @param data pointer to the buffer to parse.
		 * @param size the size of the message buffer.
		 * @return the result of the parsing.
		 */
		Message::PARSER_RESULT parse(const char* buffer);

		/// Parse a buffer with an unknown length.
		/**
		 * Overloaded version of Message::parse() for parsing a complete message.
		 * Assumes that the buffer ends with a null character.
		 * @param buffer pointer to the buffer to be parsed.
		 * @return the result of the parsing.
		 */
		Message::PARSER_RESULT parse(const char* buffer, size_t size);

		/// Sets the body of a Message.
		/**
		 * Set the body of a Message.
		 * @param f the File object containing the body.
		 * @param mime the MIME type of the body.
		 */
		void set_body(File* f, const char* mime);

		/**
		 * @return the body of the message as a File object.
		 * */
		inline File* get_body(void) const { return body; }

		/// Unsets the body of a Message.
		/**
		 * Unsets the body of a Message.
		 * @return the body that was unset.
		 */
		File* unset_body(void);

		/// Returns the depth of resources to a Message's destination.
		/**
		 * Returns the number of resources left to a Message's destination.
		 * @return the number of resources.
		 */
		uint8_t to_destination(void) const;

		/// Increments the to_url cursor to the next resource.
		void next(void);

		///Decrements the to_url cursor to the previous resource.
		void previous(void);

		/// Returns the resource name pointed by the destination url cursor.
		/**
		 * The resource name currently pointed to by the destination url
		 * cursor is the resource name the framework is routing the message
		 * to next.
		 * @return name of the resource pointed by the destination url cursor.
		 * */
		inline const char* current(void) const { return (*(to_url->get_resources()))[to_url_cursor]; }

		/// Adds a field to the message.
		/**
		 * @param name the name of the field. This parameter is not case-sensitive.
		 * @param value the value of the field.
		 * @param copy_name if the name string should be copied to memory.
		 *   Use this option carefully, when the message is deleted every field
		 *   name/value pair is automatically freed.
		 * @param copy_value if the value string should be copied to memory.
		 *   Use this option carefully, when the message is deleted every field
		 *   name/value pair is automatically freed.
		 * @return the result of the operation.
		 * */
		Utils::OPERATION_RESULT add_field(const char* name, const char* value, bool copy_name = true, bool copy_value = true);

		/// Gets a field from the message.
		/**
		 * @param name the name of the field to get. This parameter is not case-sensitive.
		 * @return the value of the field or NULL is the field does not exist.
		 * */
		const char* get_field(const char* name) const;

		/// Removes a field from the message.
		/**
		 * @param name the name of field to remove.
		 * @param free_name if the name string should be freed. Set this option
		 *   to false if the field was added with copy_name set to false. This
		 *   parameter is not case-sensitive.
		 * @param free_value if the value string should be freed. Set this option
		 *   to false if the field was added with copy_value set to false.
		 * @return the result of the operation.
		 * */
		Utils::OPERATION_RESULT remove_field(const char* name, bool free_name = true, bool free_value = true);

	protected:

		/** @param type the type of the message. */
		inline void set_type(Message::TYPE type) { _type = type; }

		/// Parses a line from the message header.
		/**
		 * Parses a complete line from an HTTP message header. Subclasses of message
		 * should override this method to parse their specific header lines and hand off
		 * generic parsing to this method. Hence, this method should not be called
		 * directly.
		 * @param line a null terminated complete line.
		 * @return the result of the parsing.
		 */
		virtual Message::PARSER_RESULT parse_header(char* line);

		/// Store a message body from a buffer.
		/**
		 * A message body is treated like header lines and can be received in chunks.
		 * This method assumes that the content-length header field was previously
		 * received and parsed and that a File object was created to store the body,
		 * otherwise, the message is assumed to have not body.
		 * @param buffer pointer to the body buffer.
		 * @param size the size of the body.
		 * @return the result of the parsing.
		 */
		Message::PARSER_RESULT store_body(const char* buffer, size_t size);

		/** Extract values from a header field line.
		 * @param line the line containing the field
		 * @param field the name of the field in position 0 and the value in position 1
		 * @return bool if extraction was successful
		 * */
		bool extract_field(char* line, char* field[2]) const;
};

#endif /* MESSAGE_H_ */
