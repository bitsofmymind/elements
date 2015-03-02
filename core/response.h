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
		typedef uint16_t status_code;

		//STATUS CODES DEFINITIONS

		enum STATUS_CODE
		{
			NONE_0 = 0, // No response code defined yet.

			//Informational: 1xx

			CONTINUE_100 = 100,
			SWITCHING_PROTOCOLS_101  = 101,
			/** This status code is specific to the Elements framework. It is used
			* to inform the framework that a Resource is holding a request and
			* will return the response at a later time. This is useful for cases
			* where a response requires lengthy processing or when it must wait
			* for an external event.*/
			RESPONSE_DELAYED_102 = 102,

			//Successful: 2xx

			OK_200 = 200,
			CREATED_201 = 201,
			ACCEPTED_202 = 202,
			NON_AUTHORITATIVE_INFORMATION_203 = 203,
			NO_CONTENT_204 = 204,
			RESET_CONTENT_205 = 205,
			PARTIAL_CONTENT_206 = 206,
			/** This status code is specific to the framework. It is used
			* to inform that a message has been consumed by a resource and
			* will not initiate a reply.
			*/
			DONE_207 = 207,

			//Redirection: 3xx

			MULTIPLE_CHOICES_300 = 300,
			MOVED_PERMANENTLY_301 = 301,
			FOUND_302 = 302,
			SEE_OTHER_303 = 303,
			NOT_MODIFIED_304 = 304,
			USE_PROXY_305 = 305,
			//306 is no longer used and thus reserved
			TEMPORARY_REDIRECT_307 = 307,
			/** This status code is specific to the framework. It is
			* used by processing to inform it that a Resource will not process
			* a message due to it not being the destination. Note that it does
			* not necessarily mean that no processing was done on the message.
			* For cases where the resource is not capable of processing
			* the message due to other reasons, 4xx and 5xx status codes should
			* be used. */
			PASS_308 = 308, //Elements framework specific

			//Client Error: 4xx

			BAD_REQUEST_400 = 400,
			UNAUTHORIZED_401 = 401,
			PAYMENT_REQUIRED_402 = 402,
			FORBIDDEN_403 = 403,
			NOT_FOUND_404 = 404,
			METHOD_NOT_ALLOWED_405 = 405,
			NOT_ACCEPTABLE_406 = 406,
			PROXY_AUTHENTICATION_REQUIRED_407 = 407,
			REQUEST_TIMEOUT_408 = 408,
			CONFLICT_409 = 409,
			GONE_410 = 410,
			LENGTH_REQUIRED_411 = 411,
			PRECONDITION_FAILED_412 = 412,
			REQUEST_ENTITY_TOO_LARGE_413 = 413,
			REQUEST_URI_TOO_LONG_414 = 414,
			UNSUPPORTED_MEDIA_TYPE_415 = 415,
			REQUESTED_RANGE_NOT_SATISFIABLE_416 = 416,
			EXPECTATION_FAILED_417 = 417,

			//Server error: 5xx

			INTERNAL_SERVER_ERROR_500 = 500,
			NOT_IMPLEMENTED_501 = 501,
			BAD_GATEWAY_502 = 502,
			SERVICE_UNAVAILABLE_503 = 503,
			GATEWAY_TIMEOUT_504 = 504,
			HTTP_VERSION_NOT_SUPPORTED_505 = 505,

			MAXIMUM_65535 = 65535
		};

#if LOCATION
		///The content of the location header field.
		const char* location;
#endif

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
		virtual ~Response();

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
		virtual void print() const;

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
		 * @see Message::parse().
		 * */
		virtual Message::PARSER_RESULT parse_header(char* line);
#endif
};
#endif /* RESPONSE_H_ */
