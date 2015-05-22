/***************************************
* @file     avaitime.h
* @brief    available time--适用的时间操作函数与相关类
* @details  计时函数(opencv2.4.6 modules\core\src\system.cpp)--getTickCount/getTickFrequency/getCPUTickCount
* @author   phata, wqvbjhc@gmail.com
* @date     2013-9-2
* @mod		2013-9-12	添加休眠函数
****************************************/
#ifndef AVAI_TIME_H
#define AVAI_TIME_H
#include <stdint.h>
//! Returns the number of ticks.

/*!
  The function returns the number of ticks since the certain event (e.g. when the machine was turned on).
  It can be used to initialize cv::RNG or to measure a function execution time by reading the tick count
  before and after the function call. The granularity of ticks depends on the hardware and OS used. Use
  cv::getTickFrequency() to convert ticks to seconds.
*/
int64_t getTickCount();

/*!
  Returns the number of ticks per seconds.

  The function returns the number of ticks (as returned by cv::getTickCount()) per second.
  The following code computes the execution time in milliseconds:

  \code
  double exec_time = (double)getTickCount();
  // do something ...
  exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
  \endcode
*/
double getTickFrequency();

/*!
  Returns the number of CPU ticks.

  On platforms where the feature is available, the function returns the number of CPU ticks
  since the certain event (normally, the system power-on moment). Using this function
  one can accurately measure the execution time of very small code fragments,
  for which cv::getTickCount() granularity is not enough.
*/
int64_t getCPUTickCount();

/*体眠ms毫秒*/
void sleep(int ms);

#endif  //AVAI_TIME_H
