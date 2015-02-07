/* response.h - Header file for the Response class.
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

#ifndef RESPONSE_H_
#define RESPONSE_H_

//INCLUDES
#include <utils/utils.h>
#include "message.h"
#include "request.h"

///Response implements an HTTP response.
class Response: public Message
{
	public:
		///Typedef for the HTTP status code.
		typedef uint8_t status_code;

		//STATUS CODES DEFINITIONS

		/*To reduce the bit footprint of a status code, the actual number
		 * value is compress by allocating the first 3 bits of a byte for
		 * the status type (informational, successful, redirection, client
		 * error and server error) and the remaining 5 bits for the code.
		 * This effectively allows 8 differrent types and 32 different
		 * codes, which is more than enough for HTTP/1.1. */

		//Informational: 1xx
		#define CONTINUE_100 ((1 << 5) + 0)
		#define SWITCHING_PROTOCOLS_101 ((1 << 5) + 1)
		/**This status code is specific to the Elements framework, it is used
		 * to inform the framework that a Resource is holding a request and
		 * will return the response at a later time. This is useful for cases
		 * where a response requires lengthy processing or when it must wait
		 * for an external event.*/
		#define RESPONSE_DELAYED_102 ((1 << 5) + 2)

		//Successful: 2xx
		#define OK_200 ((2 << 5) + 0)
		#define CREATED_201 ((2 << 5) + 1)
		#define ACCEPTED_202 ((2 << 5) + 2)
		#define NON_AUTHORITATIVE_INFORMATION_203 ((2 << 5) + 3)
		#define NO_CONTENT_204 ((2 << 5) + 4)
		#define RESET_CONTENT_205 ((2 << 5) + 5)
		#define PARTIAL_CONTENT_206 ((2 << 5) + 6)
		#define DONE_207 ((2 << 5) + 7)

		//Redirection: 3xx
		#define MULTIPLE_CHOICES_300 ((3 << 5) + 0)
		#define MOVED_PERMANENTLY_301 ((3 << 5) + 1)
		#define FOUND_302 ((3 << 5) + 2)
		#define SEE_OTHER_303 ((3 << 5) + 3)
		#define NOT_MODIFIED_304 ((3 << 5) + 4)
		#define USE_PROXY_305 ((3 << 5) + 5)
		//306 is no longer used and thus reserved
		#define TEMPORARY_REDIRECT_307 ((3 << 5) + 7)
		/** This status code is specific to the elements framework. It is
		 * used by processing to inform it that Resource will not process
		 * a message due to it not being the destination. Note that is does
		 * not necessarily mean that no processing was done on the message.
		 * For cases where the resource is not capable of processing
		 * the message due to other reasons, 4xx and 5xx status codes should
		 * be used. */
		#define PASS_308 ((3 << 5) + 8) //Elements framework specific

		//Client Error: 4xx
		#define BAD_REQUEST_400 ((4 << 5) + 0)
		#define UNAUTHORIZED_401 ((4 << 5) + 1)
		#define PAYMENT_REQUIRED_402 ((4 << 5) + 2)
		#define FORBIDDEN_403 ((4 << 5) + 3)
		#define NOT_FOUND_404 ((4 << 5) + 4)
		#define METHOD_NOT_ALLOWED_405 ((4 << 5) + 5)
		#define NOT_ACCEPTABLE_406 ((4 << 5) + 6)
		#define PROXY_AUTHENTICATION_REQUIRED_407 ((4 << 5) + 7)
		#define REQUEST_TIMEOUT_408 ((4 << 5) + 8)
		#define CONFLICT_409 ((4 << 5) + 9)
		#define GONE_410 ((4 << 5) + 10)
		#define LENGTH_REQUIRED_411 ((4 << 5) + 11)
		#define PRECONDITION_FAILED_412 ((4 << 5) + 12)
		#define REQUEST_ENTITY_TOO_LARGE_413 ((4 << 5) + 13)
		#define REQUEST_URI_TOO_LONG_414 ((4 << 5) + 14)
		#define UNSUPPORTED_MEDIA_TYPE_415 ((4 << 5) + 15)
		#define REQUESTED_RANGE_NOT_SATISFIABLE_416 ((4 << 5) + 16)
		#define EXPECTATION_FAILED_417 ((4 << 5) + 17)

		//Server error: 5xx
		#define INTERNAL_SERVER_ERROR_500 ((5 << 5) + 0)
		#define NOT_IMPLEMENTED_501 ((5 << 5) + 1)
		#define BAD_GATEWAY_502 ((5 << 5) + 2)
		#define SERVICE_UNAVAILABLE_503 ((5 << 5) + 3)
		#define GATEWAY_TIMEOUT_504 ((5 << 5) + 4)
		#define HTTP_VERSION_NOT_SUPPORTED_503 ((5 << 5) + 5)

		//Response header fields
		//static const char ACCEPT_RANGES[];
		//static const char AGE[];
		//static const char ALLOW[];
		//static const char ETAG[];
#if LOCATION
		///Location header field.
		static const char LOCATION_STR[];
		///The content of the location header field.
		const char* location;
#endif
		//static const char PROXY_AUTHENTICATE[];
		//static const char RETRY_AFTER[];
		//static const char SERVER[];
		//static const char VARY[];
		//static const char WWW_AUTHENTICATE[];

	private:

		///The status code of this response.
		status_code _response_code_int;

		///The request that triggered this response.
		/**The request is kept for many reasons but mainly for the framework
		 * to know which request triggered what response.*/
		Request* _original_request;

	public:

		///Class constructor.
		/**
		 * @param _response_code the response's status code.
		 * @param _orginial_request the request that triggered this response; NULL
		 * 	if there is none.
		 * */
		Response(const status_code code, Request* request);

		///Class destructor.
		~Response();

		/** @param url the destination URL.*/
		inline void set_to_url(URL* url) { to_url = url; }

		/** @param url the source URL.*/
		inline void set_from_url(URL* url) { from_url = url; }

		/** @return the response status code. */
		inline status_code get_status_code() const { return _response_code_int; }

		/** @param code the response status code. */
		inline void set_status_code(status_code code) { _response_code_int = code; }

		/** @return the original request. */
		inline const Request* get_request(void) const { return _original_request; }

		/** @param request the original request. */
		void set_request(Request* request);


		/// Prints the content of a Response to the output.
		virtual void print();

		/// Serialize the response to a buffer.
		/**
		 * Serialize the response to a buffer and/or returns the length in bytes of the
		 * serialized request. Simply returning the length is useful for allocating a
		 * buffer to which the message is then serialized to.
		 * @param buffer the buffer to serialize the response to.
		 * @param write if the data should be written to the buffer. If set to false,
		 * 		only the length of the serialized response will be returned.
		 * @return if write is true, the number of bytes written to the buffer, if
		 * 		write is false, the length of the serialized response.
		 * */
		virtual size_t serialize(char* buffer, bool write) const;

	protected:

#if RESPONSE_DESERIALIZATION
		///Parses a line from the response specific part of the HTTP header.
		/**
		 * @param line the header line currently being parsed.
		 * @param size the size in bytes of the header line.
		 * @see Message::parse().
		 * */
		virtual Message::PARSER_RESULT parse_header(const char* line, size_t size);
#endif
};
#endif /* RESPONSE_H_ */
