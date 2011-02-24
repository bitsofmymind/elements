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

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0

#ifdef DEBUG
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
#endif
#endif /* PAL_H_ */
