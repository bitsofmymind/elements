/* pal.h - Implements an abstract platform abstraction layer
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

#ifndef PAL_H_
#define PAL_H_

#include <stdint.h>
#include <configuration.h>

/*typedef struct
{
        uint32_t years;
        uint8_t months;
        uint8_t days;
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
        uint16_t milliseconds;
}formatedTime;*/

//formatedTime get_formated_time( void );

extern void init(void);

#define ASAP 0
#define SECONDS(s) (s) * 1000
#define MINUTES(m) (m) * SECONDS(60)
#define HOURS(h) (h) * MINUTES(60)
#define DAYS(d) (d) * HOURS(24)
#define WEEKS(w) (w) * DAYS(7)
#define NEVER MAX_UPTIME

#if TIME_KEEPING
	extern uint64_t get_time( void );
	extern void set_time(uint64_t time);
	inline void initialize_time( void );
#endif

#define expire(var, time) var = get_uptime() + time
#define is_expired(var) var <= get_uptime()

uptime_t get_uptime(void);
void increase_uptime(uptime_t time);


extern void processing_wake();
extern void processing_sleep(uptime_t time);

#if HEARTBEAT
extern void heart_beat(void);
#endif

void* ts_malloc(size_t size);
void ts_free(void* block);
void* ts_realloc(void* ptr, size_t size);

#ifdef MULTITHREADING
	void lock();
        bool try_lock();
	void unlock();

	void enter_critical_region();
	void exit_critical_region();
#endif

#if OUTPUT_ERRORS

#define ERROR_PRINT(a) DEBUG_PRINT(a)
#define ERROR_NPRINT(a, l) DEBUG_NPRINT(a, l)
#define ERROR_TPRINT(a, t) DEBUG_TPRINT(a, t);

#define ERROR_PRINTLN(a) DEBUG_PRINTLN(a);
#define ERROR_NPRINTLN(a, l) DEBUG_NPRINTLN(a, l);
#define ERROR_TPRINTLN(a, t) DEBUG_TPRINTLN(a, t);

#else

#define ERROR_PRINT(a)
#define ERROR_NPRINT(a, l)
#define ERROR_TPRINT(a, t)

#define ERROR_PRINTLN(a)
#define ERROR_NPRINTLN(a, l)
#define ERROR_TPRINTLN(a, t)

#endif

#if OUTPUT_WARNINGS

#define WARNING_PRINT(a) DEBUG_PRINT(a)
#define WARNING_NPRINT(a, l) DEBUG_NPRINT(a, l)
#define WARNING_TPRINT(a, t) DEBUG_TPRINT(a, t);

#define WARNING_PRINTLN(a) DEBUG_PRINTLN(a);
#define WARNING_NPRINTLN(a, l) DEBUG_NPRINTLN(a, l);
#define WARNING_TPRINTLN(a, t) DEBUG_TPRINTLN(a, t);

#else

#define WARNING_PRINT(a)
#define WARNING_NPRINT(a, l)
#define WARNING_TPRINT(a, t)

#define WARNING_PRINTLN(a)
#define WARNING_NPRINTLN(a, l)
#define WARNING_TPRINTLN(a, t)

#endif

#if VERBOSITY

#define VERBOSE_PRINT(a) DEBUG_PRINT(a)
#define VERBOSE_NPRINT(a, l) DEBUG_NPRINT(a, l)
#define VERBOSE_TPRINT(a, t) DEBUG_TPRINT(a, t)

#define VERBOSE_PRINTLN(a) DEBUG_PRINTLN(a);
#define VERBOSE_NPRINTLN(a, l) DEBUG_NPRINTLN(a, l);
#define VERBOSE_TPRINTLN(a, t) DEBUG_TPRINTLN(a, t)

#else

#define VERBOSE_PRINT(a)
#define VERBOSE_NPRINT(a, l)
#define VERBOSE_TPRINT(a, t)

#define VERBOSE_PRINTLN(a)
#define VERBOSE_NPRINTLN(a, l)
#define VERBOSE_TPRINTLN(a, t)

#endif

#if OUTPUT_ERRORS || OUTPUT_WARNINGS || VERBOSITY || OUTPUT_DEBUG

#define DEC (uint8_t)10
#define HEX (uint8_t)16
#define OCT (uint8_t)8
#define BIN (uint8_t)2
#define BYTE (uint8_t)0

#define DEBUG_PRINT(a) Debug::print(a);
#define DEBUG_NPRINT(a, l) Debug::nprint(a, l);
#define DEBUG_TPRINT(a, t) Debug::print(a, t);

#define DEBUG_PRINTLN(a) Debug::println(a);
#define DEBUG_NPRINTLN(a, l) Debug::nprintln(a, l);
#define DEBUG_TPRINTLN(a, t) Debug::println(a, t);


namespace Debug
{
	extern void print_char(char);
	extern void println(void);

	void print(const char*);
	void print(char);
	void nprint(const char* str, uint16_t length);
	void print(int8_t, uint8_t);
	void print(uint8_t, uint8_t);
	void print(int16_t, uint8_t);
	void print(uint16_t, uint8_t);
	void print(int32_t, uint8_t);
	void print(uint32_t, uint8_t);
	//void print(double, int = 2);

	void println(const char*);
	void println(char);
	void nprintln(const char* str, uint16_t length);
	void println(int8_t, uint8_t);
	void println(uint8_t, uint8_t);
	void println(int16_t, uint8_t);
	void println(uint16_t, uint8_t);
	void println(int32_t, uint8_t);
	void println(uint32_t, uint8_t);
	//void println(double, int = 2);
}
#else
#define DEBUG_PRINT(a)
#define DEBUG_NPRINT(a, l)
#define DEBUG_TPRINT(a, t)


#define DEBUG_PRINTLN(a)
#define DEBUG_NPRINTLN(a, l)
#define DEBUG_TPRINTLN(a, t)

#endif
#endif /* PAL_H_ */
