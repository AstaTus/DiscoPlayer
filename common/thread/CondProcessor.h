#ifndef __MUTUAL_PROCESSOR_H__
#define __MUTUAL_PROCESSOR_H__

class Cond;
class Mutex;
#include <stdint.h>

class CondProcessor
{
private:
    /* data */
public:
    static int cond_wait(Cond& cond, const Mutex& mutex);
    static int cond_wait_timeout(Cond& cond, const Mutex& mutex, uint32_t millisecond);
};
#endif