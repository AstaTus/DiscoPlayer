#include "Log.h"
#include <sys/time.h>
#include <stdio.h>
Log & Log::get_instance() {
    static Log instance;
    return instance;
}

Log::Log(/* args */)
{
    av_log_set_level(AV_LOG_DEBUG);
}

void Log::format_time_log(char *result, const char *format, int total_size)
{
    timeval tp;
    gettimeofday(&tp, 0);
    time_t curtime = tp.tv_sec;
    tm *t = localtime(&curtime);
    sprintf(result, "[%04d-%02d-%02d %02d:%02d:%02d.%05d]", t->tm_year, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, tp.tv_usec/1000);
    int result_len = strlen(result);
    int format_len = strlen(format) + 1;
    int rest_len = total_size - result_len;

    memcpy(result + result_len, format, format_len < rest_len ? format_len : rest_len);

}
