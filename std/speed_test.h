#ifndef SPEED_TEST_H
#define SPEED_TEST_H

// ===================== FOR USE ======================================
#define RTC_START_TIMER     gettimeofday(&start_v, &start_z)
#define RTC_STOP_TIMER      gettimeofday(&end_v,&end_z)
#define CPU_START_TIMER     clock_gettime(CLOCK_MONOTONIC, &start_s)
#define CPU_STOP_TIMER      clock_gettime(CLOCK_MONOTONIC, &end_s)
#define PRINT_RTC_DIFF_TIME     std::cout << uspeedMeasure() << std::endl
#define PRINT_CPU_DIFF_TIME     std::cout << nspeedMeasure() << std::endl
// --------------------------------------------------------------------

#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <stdint.h>

static struct timeval start_v, end_v, res_v;
static struct timespec start_s, end_s, res_s;
static struct timezone start_z, end_z;

double uspeedMeasure()
{
    timersub(&end, &start, &res);
    return ((double)res.tv_sec + (double)res.tv_usec/CLOCKS_PER_SEC);
}

long nspeedMeasure()
{
    do {
        res_s.tv_sec = end_s.tv_sec - start_s.tv_sec;
        res_s.tv_nsec = end_s.tv_nsec - start_s.tv_nsec;			      \
        if (res_s.tv_nsec < 0) {					      \
            --res_s.tv_sec;						      \
            res_s.tv_nsec += 1000000000;					      \
        }									      \
    } while (0);
    return (res_s.tv_sec + res_s.tv_nsec/(CLOCKS_PER_SEC*1000));
}

#endif // SPEED_TEST_H
