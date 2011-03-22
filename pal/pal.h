/*
 * pal.h
 *
 *  Created on: May 23, 2009
 *      Author: Antoine
 */

#ifndef PAL_H_
#define PAL_H_

#include <stdint.h>
#include <elements.h>

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
#define SECONDS(s) s*1000
#define MINUTES(m) m*SECONDS(60)
#define HOURS(h) h*MINUTES(60)
#define DAYS(d) d*HOURS(24)
#define WEEKS(w) w*DAYS(7)
#define NEVER MAX_UPTIME

#ifdef TIME_KEEPING
	extern uint64_t get_time( void );
	extern void set_time(uint64_t time);
	inline void initialize_time( void );
#endif
uptime_t get_uptime(void);
void increase_uptime(uptime_t time);


extern void processing_wake();
extern void processing_sleep(uptime_t time);


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
#define ERROR_PRINT_DEC(a) DEBUG_PRINT_DEC(a);
#define ERROR_PRINT_HEX(a) DEBUG_PRINT_HEX(a);
#define ERROR_PRINT_OCT(a) DEBUG_PRINT_OCT(a);
#define ERROR_PRINT_BIN(a) DEBUG_PRINT_BIN(a);
#define ERROR_PRINT_BYTE(a) DEBUG_PRINT(a);

#define ERROR_PRINTLN(a) DEBUG_PRINTLN(a);
#define ERROR_NPRINTLN(a, l) DEBUG_NPRINTLN(a, l);
#define ERROR_PRINTLN_DEC(a) DEBUG_PRINTLN_DEC(a);
#define ERROR_PRINTLN_HEX(a) DEBUG_PRINTLN_HEX(a);
#define ERROR_PRINTLN_OCT(a) DEBUG_PRINTLN_OCT(a);
#define ERROR_PRINTLN_BIN(a) DEBUG_PRINTLN_BIN(a);
#define ERROR_PRINTLN_BYTE(a) DEBUG_PRINTLN(a);

#else

#define ERROR_PRINT(a)
#define ERROR_NPRINT(a, l)
#define ERROR_PRINT_DEC(a)
#define ERROR_PRINT_HEX(a)
#define ERROR_PRINT_OCT(a)
#define ERROR_PRINT_BIN(a)
#define ERROR_PRINT_BYTE(a)

#define ERROR_PRINTLN(a)
#define ERROR_NPRINTLN(a, l)
#define ERROR_PRINTLN_DEC(a)
#define ERROR_PRINTLN_HEX(a)
#define ERROR_PRINTLN_OCT(a)
#define ERROR_PRINTLN_BIN(a)
#define ERROR_PRINTLN_BYTE(a)

#endif

#if OUTPUT_WARNINGS

#define WARNING_PRINT(a) DEBUG_PRINT(a)
#define WARNING_NPRINT(a, l) DEBUG_NPRINT(a, l)
#define WARNING_PRINT_DEC(a) DEBUG_PRINT_DEC(a);
#define WARNING_PRINT_HEX(a) DEBUG_PRINT_HEX(a);
#define WARNING_PRINT_OCT(a) DEBUG_PRINT_OCT(a);
#define WARNING_PRINT_BIN(a) DEBUG_PRINT_BIN(a);
#define WARNING_PRINT_BYTE(a) DEBUG_PRINT(a);

#define WARNING_PRINTLN(a) DEBUG_PRINTLN(a);
#define WARNING_NPRINTLN(a, l) DEBUG_NPRINTLN(a, l);
#define WARNING_PRINTLN_DEC(a) DEBUG_PRINTLN_DEC(a);
#define WARNING_PRINTLN_HEX(a) DEBUG_PRINTLN_HEX(a);
#define WARNING_PRINTLN_OCT(a) DEBUG_PRINTLN_OCT(a);
#define WARNING_PRINTLN_BIN(a) DEBUG_PRINTLN_BIN(a);
#define WARNING_PRINTLN_BYTE(a) DEBUG_PRINTLN(a);

#else

#define WARNING_PRINT(a)
#define WARNING_NPRINT(a, l)
#define WARNING_PRINT_DEC(a)
#define WARNING_PRINT_HEX(a)
#define WARNING_PRINT_OCT(a)
#define WARNING_PRINT_BIN(a)
#define WARNING_PRINT_BYTE(a)

#define WARNING_PRINTLN(a)
#define WARNING_NPRINTLN(a, l)
#define WARNING_PRINTLN_DEC(a)
#define WARNING_PRINTLN_HEX(a)
#define WARNING_PRINTLN_OCT(a)
#define WARNING_PRINTLN_BIN(a)
#define WARNING_PRINTLN_BYTE(a)

#endif

#if VERBOSITY

#define VERBOSE_PRINT(a) DEBUG_PRINT(a)
#define VERBOSE_NPRINT(a, l) DEBUG_PRINT(a, l)
#define VERBOSE_PRINT_DEC(a) DEBUG_PRINT_DEC(a);
#define VERBOSE_PRINT_HEX(a) DEBUG_PRINT_HEX(a);
#define VERBOSE_PRINT_OCT(a) DEBUG_PRINT_OCT(a);
#define VERBOSE_PRINT_BIN(a) DEBUG_PRINT_BIN(a);
#define VERBOSE_PRINT_BYTE(a) DEBUG_PRINT(a);

#define VERBOSE_PRINTLN(a) DEBUG_PRINTLN(a);
#define VERBOSE_NPRINTLN(a, l) DEBUG_NPRINTLN(a, l);
#define VERBOSE_PRINTLN_DEC(a) DEBUG_PRINTLN_DEC(a);
#define VERBOSE_PRINTLN_HEX(a) DEBUG_PRINTLN_HEX(a);
#define VERBOSE_PRINTLN_OCT(a) DEBUG_PRINTLN_OCT(a);
#define VERBOSE_PRINTLN_BIN(a) DEBUG_PRINTLN_BIN(a);
#define VERBOSE_PRINTLN_BYTE(a) DEBUG_PRINTLN(a);

#else

#define VERBOSE_PRINT(a)
#define VERBOSE_NPRINT(a, l)
#define VERBOSE_PRINT_DEC(a)
#define VERBOSE_PRINT_HEX(a)
#define VERBOSE_PRINT_OCT(a)
#define VERBOSE_PRINT_BIN(a)
#define VERBOSE_PRINT_BYTE(a)

#define VERBOSE_PRINTLN(a)
#define VERBOSE_NPRINTLN(a, l)
#define VERBOSE_PRINTLN_DEC(a)
#define VERBOSE_PRINTLN_HEX(a)
#define VERBOSE_PRINTLN_OCT(a)
#define VERBOSE_PRINTLN_BIN(a)
#define VERBOSE_PRINTLN_BYTE(a)

#endif

#if OUTPUT_ERRORS || OUTPUT_WARNINGS || VERBOSITY

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0

#define DEBUG_PRINT(a) Debug::print(a);
#define DEBUG_NPRINT(a, l) Debug::print(a, l);
#define DEBUG_PRINT_DEC(a) Debug::print(a, DEC);
#define DEBUG_PRINT_HEX(a) Debug::print(a, HEX);
#define DEBUG_PRINT_OCT(a) Debug::print(a, OCT);
#define DEBUG_PRINT_BIN(a) Debug::print(a, BIN);
#define DEBUG_PRINT_BYTE(a) Debug::print(a);

#define DEBUG_PRINTLN(a) Debug::println(a);
#define DEBUG_NPRINTLN(a, l) Debug::println(a, l);
#define DEBUG_PRINTLN_DEC(a) Debug::println(a, DEC);
#define DEBUG_PRINTLN_HEX(a) Debug::println(a, HEX);
#define DEBUG_PRINTLN_OCT(a) Debug::println(a, OCT);
#define DEBUG_PRINTLN_BIN(a) Debug::println(a, BIN);
#define DEBUG_PRINTLN_BYTE(a) Debug::println(a);

namespace Debug
{
	extern void print(char);
	extern void println(void);

	void print(const char*);
	void print(const char* str, uint16_t length);
	void print(char, uint8_t);
	void print(uint8_t, uint8_t = BYTE);
	void print(int16_t, uint8_t = DEC);
	void print(uint16_t, uint8_t = DEC);
	void print(int32_t, uint8_t= DEC);
	void print(uint32_t, uint8_t = DEC);
	//void print(double, int = 2);

	void println(const char*);
	void println(const char* str, uint16_t length);
	void println(char, uint8_t = BYTE);
	void println(uint8_t, uint8_t = BYTE);
	void println(int16_t, uint8_t = DEC);
	void println(uint16_t, uint8_t = DEC);
	void println(int32_t, uint8_t= DEC);
	void println(uint32_t, uint8_t = DEC);
	//void println(double, int = 2);
}
#else
#define DEBUG_PRINT(a)
#define DEBUG_NPRINT(a, l)
#define DEBUG_PRINT_DEC(a)
#define DEBUG_PRINT_HEX(a)
#define DEBUG_PRINT_OCT(a)
#define DEBUG_PRINT_BIN(a)
#define DEBUG_PRINT_BYTE(a)

#define DEBUG_PRINTLN(a)
#define DEBUG_NPRINTLN(a, l)
#define DEBUG_PRINTLN_DEC(a)
#define DEBUG_PRINTLN_HEX(a)
#define DEBUG_PRINTLN_OCT(a)
#define DEBUG_PRINTLN_BIN(a)
#define DEBUG_PRINTLN_BYTE(a)
#endif
#endif /* PAL_H_ */
