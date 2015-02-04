#ifndef _simple_timer_h_
#define _simple_timer_h_
/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*  
*   Simple time operations
*
*END************************************************************************/

#define OS_MSECS_TO_TICKS wl_msecs_to_ticks

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint64_t wl_hw_get_microsecond();
uint64_t wl_msecs_to_ticks(uint32_t msecs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*
 * Bare bones microsecond timer class.
 */
class SimpleTimer
{
public:
    /* Constructor; takes the start timestamp. */
    inline SimpleTimer()
    {
        m_start = wl_hw_get_microsecond();

    }
    
    /* Computes and returns the elapsed time since the object was constructed. */
    inline uint64_t getElapsed()
    {
        return wl_hw_get_microsecond() - m_start;
    }

protected:
    /* The start timestamp in microseconds. */
    uint64_t m_start;   
};

/*
** Struct used for computing average operation times.
*/
class AverageTime
{
public:
    /* Constructor to init counts to zero. */
    inline AverageTime() : accumulator(0), count(0), averageTime(0), minTime(~0), maxTime(0) {}
    
    /* Add time to the average. */
    inline void add(uint64_t amount, unsigned c=1)
    {
        accumulator += amount;
        count += c;
        averageTime = accumulator / count;
        maxTime = (unsigned)MAX(maxTime, amount/c);
        minTime = (unsigned)MIN(minTime, amount/c);
    }
    
    /* Overloaded operator to add time to the average. */
    inline AverageTime & operator += (uint64_t amount)
    {
        add(amount);
        return *this;
    }
    
    /* Accessor methods */
    inline unsigned getCount() const { return count; }
    inline unsigned getAverage() const { return averageTime; }
    inline unsigned getMin() const { return minTime; }
    inline unsigned getMax() const { return maxTime; }
    
    /* Clear the accumulator and counter and reset times. */
    inline void reset()
    {
        accumulator = 0;
        count = 0;
        averageTime = 0;
        minTime = ~0;
        maxTime = 0;
    }

protected:
    uint64_t accumulator;
    unsigned count;
    unsigned averageTime;
    unsigned minTime;
    unsigned maxTime;
};

#endif /* _simple_timer_h_ */

/* EOF */

