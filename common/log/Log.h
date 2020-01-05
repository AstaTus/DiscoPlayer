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


class Log
{
private:
    Log(/* args */);
public:
    Log(const Log&)=delete;
    Log& operator=(const Log&)=delete;
    static Log & get_instance();
    
    template<typename... Args>
    void log_error(const char *format, Args&&... args);

    template<typename... Args>
    void log_info(const char *format, Args&&... args);

    template<typename... Args>
    void log_debug(const char *format, Args&&... args);

    template<typename... Args>
    void log_warning(const char *format, Args&&... args);
};

Log & Log::get_instance() {
    static Log instance;
    return instance;
}

Log::Log(/* args */)
{
    av_log_set_level(AV_LOG_DEBUG);
}


template<typename... Args>
void Log::log_error(const char *format, Args&&... args) {
    av_log(NULL, AV_LOG_ERROR, format, std::forward<Args>(args)...);
}

template<typename... Args>
void Log::log_info(const char *format, Args&&... args) {
    av_log(NULL, AV_LOG_INFO, format, std::forward<Args>(args)...);
}

template<typename... Args>
void Log::log_warning(const char *format, Args&&... args) {
    av_log(NULL, AV_LOG_WARNING, format, std::forward<Args>(args)...);
}

template<typename... Args>
void Log::log_debug(const char *format, Args&&... args) {
    av_log(NULL, AV_LOG_DEBUG, format, std::forward<Args>(args)...);
}

#endif