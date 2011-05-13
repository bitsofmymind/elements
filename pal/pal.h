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

#if TIME_KEEPING
	extern uint64_t get_time( void );
	extern void set_time(uint64_t time);
	inline void initialize_time( void );
#endif
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

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0

#define DEBUG_PRINT(a) Debug::print(a);
#define DEBUG_NPRINT(a, l) Debug::print(a, l);
#define DEBUG_TPRINT(a, t) Debug::print(a, t);

#define DEBUG_PRINTLN(a) Debug::println(a);
#define DEBUG_NPRINTLN(a, l) Debug::println(a, l);
#define DEBUG_TPRINTLN(a, t) Debug::println(a, t);


namespace Debug
{
	extern void print_char(char);
	extern void println(void);

	void print(const char*);
	void print(const char* str, uint16_t length);
	void print(char, uint8_t = BYTE);
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
#define DEBUG_TPRINT(a, t)


#define DEBUG_PRINTLN(a)
#define DEBUG_NPRINTLN(a, l)
#define DEBUG_TPRINTLN(a, t)

#endif
#endif /* PAL_H_ */
