#include "Log.h"

Log & Log::get_instance() {
    static Log instance;
    return instance;
}

Log::Log(/* args */)
{
    av_log_set_level(AV_LOG_DEBUG);
}
