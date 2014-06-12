// SVN FILE: $Id: $
/**
 * @lastChangedBy           $lastChangedBy: Mercier $
 * @revision                $Revision: 397 $
 * @copyright    			GNU General Public License
 *		This program is free software: you can redistribute it and/or modify
 * 		it under the terms of the GNU General Public License as published by
 * 		the Free Software Foundation, either version 3 of the License, or
 * 		(at your option) any later version.
 * 		This program is distributed in the hope that it will be useful,
 * 		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 		GNU General Public License for more details.
 * 		You should have received a copy of the GNU General Public License
 * 		along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Defines configuration options for the framework.
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

//INCLUDES

#include <stddef.h>
#include <stdint.h>

//DEFINES

/// If the Resource class should include a destructor method.
/**
 * Not all implementation of the framework will need to dynamically create and
 * destroy resources. To save on dynamic memory and code space, destructors for
 * resources can be defined away.
 * */
#define RESOURCE_DESTRUCTION 1

/// If the framework will process serialized response messages.
/**
 * Responses that come in through an interface are always serialized. If the
 * framework is not to receive responses at all (ie: for an implementation that
 * does not send requests) or if the responses received will come from resources
 * within this implementation (in the form of objects),
 * RESPONSE_DESERIALIZATION should be set to 0 in order to save on code space.
 */
#define RESPONSE_DESERIALIZATION 0

/// If the framework should send out serialized request.
/**
 * For an implementation of the framework that does not send out serialized
 * request through an interface (ie: a simple file server) or only sends
 * requests to within its own implementation, set REQUEST_SERIALIZATION to 0
 * to save on code space.
 */
#define REQUEST_SERIALIZATION 1

/// If urls need to be serialized.
/**
 * If urls do not need to be serialized, set URL_SERIALIZATION to 0 to save on
 * code space.
 */
#define URL_SERIALIZATION 1

/// Whether an internal clock should be kept.
/**
 * If the implementation of the framework requires time-keeping services, set
 * TIME_KEEPING to 1. Otherwise deactivate it to save on code space and memory.
 */
#define TIME_KEEPING 1

/// If the creation uptime of a message should be kept.
/**
 * Set MESSAGE_AGE to one if a message should save the current uptime when
 * instantiated. This option is useful for implementing messages that have
 * been waiting for processing for too long.
 */
#define MESSAGE_AGE 1


/// If the framework should have a heatbeat.
/**
 * An heartbeat is signal that is toggled every time a timer expires. That time
 * is checked at every processing run. The mechanism is useful on embedded
 * implementations, where an LED can be toggled at a set interval to inform
 * users if the framework is still alive or if it has crashed.
 */
#define HEARTBEAT 1

/// If the Processing class should inherit from Authority.
/**
 * Since a Processing class (for rocessing) is always required and an Authority
 * class (for message buffering) strongly recommended, a Processing class
 * can serve as an Authority too by setting PROCESSING_AS_AUTHORITY to 1 to
 * save both code space and memory.
 */
#define PROCESSING_AS_AUTHORITY 1

/// The size in bits of the underlying type uptime_t.
/**
 * uptime_t is a type that used for uptime keeping. Uptime is defined as the
 * number of milliseconds elapsed since the framework first started running.
 * Here is a example list of types with the maximum amount of time they
 * can store:
 * 	- uint8_t: 256 ms (unlikely to ever be useful)
 * 	- uint16_t: 16,384 ms or 16 seconds, 384 milliseconds
 * 	- uint32_t:  4,294,967,296 ms or 49 days, 17 hours, 2 minutes,
 * 		47 seconds and 296 milliseconds
 * 	- uint64_t: 584,542,046 years.
 *
 * Needless to say that a timer that overflows will yield to impredictable
 * results so this type should be picked carefully so as to find a good balance
 * between usage requirements and the memory savings of a smaller type.
 */
#define UPTIME_BITS 64

///@todo move this bit of code out of configuration.h
#if UPTIME_BITS == 64
	#define MAX_UPTIME 18446744073709551615ULL //Largest number that can fit in a 64 bit space.
	typedef uint64_t uptime_t;
#elif UPTIME_BITS == 32
	#define MAX_UPTIME 4294967295 //Largest number that can fit in a 32 bit space.
	typedef uint32_t uptime_t;
#elif UPTIME_BITS == 16
	#define MAX_UPTIME 16385 //Largest number that can fit in a 16 bit space.
	typedef uint16_t uptime_t;
#elif UPTIME_BITS == 8
	#define MAX_UPTIME 255 //Largest number that can fit in a 8 bit space.
	typedef uint8_t uptime_t;
#else
	#error "Illegal UPTIME_BITS number. This value should be one of 8, 16, 32 or 64."
#endif

/// If the standard library implementation provides an ITOA function.
/**
 * Really an ugly hack.
 * @todo get rid of this and find an stdlib agnostic itoa function.
 */
#define ITOA 0

/// If the framework should be able to parse POST body arguments.
/**
 * For an implementation of the framework that does not need to receive
 * arguments within the body of a message, set BODY_ARGS_PARSING to 0 to
 *  save on code space.
 */
#define BODY_ARGS_PARSING 1

/// If debug messages should be displayed.
#define OUTPUT_DEBUG 1

/// If framework errors should be displayed.
#define OUTPUT_ERRORS 1

/// If framework warnings should be displayed.
#define OUTPUT_WARNINGS 1

/// If framework verbosity messages should be displayed.
#define VERBOSITY 1

/// Code to be called at the beginning of an atomic block.
/**
 * An atomic code block is one that is guaranteed to execute without
 * interruption. Since this call is platform dependent, it needs to be defined
 * as part of the configuration of the framework. Here is an example of an
 * ATOMIC define on an Atmel AVR:
 * @code
 * 		#define ATOMIC_RESTORESTATE  uint8_t sreg_save __attribute__((__cleanup__(__iRestore))) = SREG
 * 		#define ATOMIC_BLOCK(type) for ( type, __ToDo = __iCliRetVal(); __ToDo ; __ToDo = 0 )
 * 		#define ATOMIC ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
 * 		//...
 * 		ATOMIC
 * 		{
 * 			//Atomic code here.
 * 		}
 * @endcode
 * In this case the ATOMIC_BLOCK exploits the three parts of a for loop
 * construct to disable interrupts (__iCliRetVal()), define a __cleanup__
 * directive (type) , and run the block only once. The cleanup directive is
 * here to restore the state of a certain register when a variable goes
 * out of scope, doing it without regards to how the block was exited from,
 * hence the need for a * block construct. Effectively, the atomicity if given
 * here by saving the state of the interrupt flag, disabling interrupts,
 * running the code in the block and then reinstating the interrupt flag,
 * thereby guaranteeing that the code within the block ran without
 * interruptions.
 *
 * If Atomicity is not required, leave this define empty.
 */
#define ATOMIC

/// If the framework should parse the protocol part of an url.
/**
 * For cases there the protocol part of an url does not matter for a framework
 * implementation, set URL_PROTOCOL to 0 to save on memory and code space.
 */
#define URL_PROTOCOL 0

/// If the framework should parse the authority part of an url.
/**
 * For cases there the authority part of an url does not matter for a framework
 * implementation, set URL_PROTOCOL to 0 to save on memory and code space.
 */
#define URL_AUTHORITY 0

/// If the framework should parse the port part of an url.
/**
 * For cases there the port part of an url does not matter for a framework
 * \mplementation, set URL_PROTOCOL to 0 to save on memory and code space.
 */
#define URL_PORT 0

/// If the framework should parse the arguments part of an url.
/**
 * For cases there the arguments part of an url does not matter for a framework
 * implementation, set URL_PROTOCOL to 0 to save on memory and code space.
 */
#define URL_ARGUMENTS 0

/// If the framework should parse the fragment part of an url.
/**
 * For cases there the fragment part of an url does not matter for a framework
 * implementation, set URL_PROTOCOL to 0 to save on memory and code space.
 */
#define URL_FRAGMENT 0

/// If a file pointer should be able to rewind.
/**
 * For some implementation of the File class, such as with offboard storage,
 * rewinding a file pointer can be an expensive operation. In cases where it
 * is not needed, set REVERSIBLE to 0 to save on code space.
 */
#define REVERSIBLE 1

/// If the file class should be read only.
/**
 * If writing to files is not necessary, set READ_ONLY to 1 to save on
 * code space.
 */
#define READ_ONLY 1

/// Define the HTTP location header.
/**
 * Set LOCATION to 1 if the Location http header is needed.
 */
#define LOCATION 1

/// If authority should have the capacity to redirect http messages.
/**
 * An HTTP message is redirected by replying with a 307 status code
 * (temporary redirect) and a location header. This mechanism is useful when
 * the page we want to show users when they ask for the root resource does not
 * reside there.
 */
#define AUTHORITY_REDIRECT 1

///@todo move this bit of code out of configuration.h
#if AUTHORITY_REDIRECT //Authority redirect needs the location header.
	#undef LOCATION
	#define LOCATION 1
#endif

#endif /* CONFIGURATION_H_ */
