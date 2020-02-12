/**
  @file
  @brief A header only module for measuring elapsed time
  */

#ifndef _TIMER_H
#define _TIMER_H

#include <time.h>

static clock_t m_timer_reference_time;
/**
  @brief Reset the reference time

  @returns 0 on success, 1 on fail
  */
int timer_reset()
{
  m_timer_reference_time = clock();
  return 0;
}

/**
  @brief Measure the amount of elapsed seconds since last reset

  @returns The amount of seconds since last reset
  */
int timer_get_seconds()
{
  return ((clock()-m_timer_reference_time)/CLOCKS_PER_SEC);
}

#endif // _TIMER_H
