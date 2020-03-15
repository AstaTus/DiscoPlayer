#ifndef __DISCO_LOG_H__
#define __DISCO_LOG_H__

#include <sstream>
#include <string>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
// #include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}
#define LOG_CHAR_SIZE 200
class Log
{
private:
    Log(/* args */);

    void format_time_log(char *result, const char *format, int total_size);

public:
    Log(const Log &) = delete;
    Log &operator=(const Log &) = delete;
    static Log &get_instance();

    template <typename... Args>
    void log_error(const char *format, Args &&... args);

    template <typename... Args>
    void log_info(const char *format, Args &&... args);

    template <typename... Args>
    void log_debug(const char *format, Args &&... args);

    template <typename... Args>
    void log_warning(const char *format, Args &&... args);
};

template <typename... Args>
void Log::log_error(const char *format, Args &&... args)
{
    char result[LOG_CHAR_SIZE];
    format_time_log(result, format, LOG_CHAR_SIZE);
    av_log(NULL, AV_LOG_ERROR, result, std::forward<Args>(args)...);
}

template <typename... Args>
void Log::log_info(const char *format, Args &&... args)
{
    char result[LOG_CHAR_SIZE];
    format_time_log(result, format, LOG_CHAR_SIZE);
    av_log(NULL, AV_LOG_INFO, result, std::forward<Args>(args)...);
}

template <typename... Args>
void Log::log_warning(const char *format, Args &&... args)
{
    char result[LOG_CHAR_SIZE];
    format_time_log(result, format, LOG_CHAR_SIZE);
    av_log(NULL, AV_LOG_WARNING, result, std::forward<Args>(args)...);
}

template <typename... Args>
void Log::log_debug(const char *format, Args &&... args)
{
    char result[LOG_CHAR_SIZE];
    format_time_log(result, format, LOG_CHAR_SIZE);
    av_log(NULL, AV_LOG_DEBUG, result, std::forward<Args>(args)...);
}
#endif