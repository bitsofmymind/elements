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


extern uint64_t get_time( void );
extern void set_time(uint64_t time);
uptime_t get_uptime(void);
void increase_uptime(uptime_t time);


inline void initialize_time( void );
inline uint32_t get_number_of_elements( void );
inline void register_element( void );
inline void unregister_element( void );
extern void processing_wake();
extern void processing_sleep(uptime_t time);


void* ts_malloc(size_t size);
void ts_free(void* block);

#ifdef MULTITHREADING
	void lock();
        bool try_lock();
	void unlock();

	void enter_critical_region();
	void exit_critical_region();
#endif

#endif /* PAL_H_ */
