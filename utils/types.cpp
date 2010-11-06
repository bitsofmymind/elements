#define __STDC_LIMIT_MACROS

#include "types.h"
#include <stdint.h>

using namespace Elements;

const e_time_t e_time_t::MAX = e_time_t(UINT32_MAX, 999);
const e_time_t e_time_t::MIN = e_time_t(0, 0);

e_time_t::e_time_t(void)
{
    milliseconds = 0;
    seconds = 0;
}

e_time_t::e_time_t(uint32_t seconds, uint16_t milliseconds):
    seconds(seconds),
    milliseconds(milliseconds)
{

}

e_time_t::e_time_t(uint32_t milliseconds):
	seconds(0),
	milliseconds(0)
{
	seconds = milliseconds / 1000;
	this->milliseconds = milliseconds % 1000;
}

e_time_t e_time_t::operator+(e_time_t rhs)
{
    e_time_t res = *this;
    res += rhs;
    return res;
}
e_time_t e_time_t::operator-(e_time_t rhs)
{
   
    e_time_t res = *this;
    res-= rhs;
    return res;
}
void e_time_t::operator++(void)
{
    milliseconds++;
    if(milliseconds >= 1000)
    {
        milliseconds = 0;
        seconds++;
    }
}
void e_time_t::operator++(int)
{
    milliseconds++;
    if(milliseconds >= 1000)
    {
        milliseconds = 0;
        seconds++;
    }
}
void e_time_t::operator--(void)
{
    if(milliseconds == 0)
    {
        milliseconds = 999;
        seconds--;
    }
}
bool e_time_t::operator==(e_time_t rhs)
{
    if(milliseconds == rhs.milliseconds && seconds == rhs.seconds)
    {
        return false;
    }
    return true;
}
bool e_time_t::operator>=(e_time_t rhs)
{
    if(seconds >= rhs.seconds && milliseconds >= rhs.milliseconds)
    {
        return true;
    }
    return false;
}
bool e_time_t::operator<=(e_time_t rhs)
{
    if(seconds <= rhs.seconds && milliseconds <= rhs.milliseconds)
    {
        return true;
    }
    return false;
}
bool e_time_t::operator>(e_time_t rhs)
{
    if(seconds > rhs.seconds && milliseconds > rhs.milliseconds)
    {
        return true;
    }
    return false;
}
bool e_time_t::operator<(e_time_t rhs)
{
    if(seconds < rhs.seconds && milliseconds < rhs.milliseconds)
    {
        return true;
    }
    return false;
}
e_time_t e_time_t::operator+=(e_time_t rhs)
{
    milliseconds += rhs.milliseconds;
    if(milliseconds >= 1000)
    {
        seconds++;
        milliseconds -= 1000;
    }
    seconds += rhs.seconds;

    return *this;
}

e_time_t e_time_t::operator-=(e_time_t rhs)
{
    if(milliseconds < rhs.milliseconds)
    {
        milliseconds = 1000 - rhs.milliseconds - milliseconds;
        seconds--;
    }
    else
    {
        milliseconds -= rhs.milliseconds;
    }
    seconds -= rhs.seconds;

    return *this;
}

void e_time_t::increase_ms(uint16_t amount)
{
    if(amount + milliseconds > 999)
    {
        seconds++;
        milliseconds = amount + milliseconds - 1000;
    }
    else
    {
        milliseconds += amount;
    }
}
