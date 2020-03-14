#ifndef __THREAD_UTILS_H__
#define __THREAD_UTILS_H__
class ThreadUtils
{

public:
    ThreadUtils(/* args */);
    ~ThreadUtils();

    static void set_thread_name(const char * name);
    static bool set_thread_prority(int prority);
};
#endif