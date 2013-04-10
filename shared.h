#ifndef _SHARED_H_
#define _SHARED_H_

#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <inttypes.h>

typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

const int kCacheLineSize = 64;

inline double
timeval_diff(const struct timeval *start, const struct timeval *end)
{
    double r = (end->tv_sec - start->tv_sec)* 1000000;
  
    if (end->tv_usec > start->tv_usec)
        r += (end->tv_usec - start->tv_usec);
    else if (end->tv_usec < start->tv_usec)
        r -= (start->tv_usec - end->tv_usec);

    return (double) r / 1000000;
}

#endif /* _SHARED_H_ */

