# 引入ffmpeg
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(./log DIR_LOG_LIB_SRCS)
aux_source_directory(./thread DIR_THREAD_LIB_SRCS)
aux_source_directory(./type DIR_TYPE_LIB_SRCS)

message("DiscoCommon include " ${DIR_LOG_LIB_SRCS})
# 生成链接库
add_library(DiscoCommon ${DIR_LOG_LIB_SRCS} ${DIR_THREAD_LIB_SRCS} ${DIR_TYPE_LIB_SRCS})

target_include_directories(DiscoCommon PRIVATE ${AVUTIL_INCLUDE_DIR})
