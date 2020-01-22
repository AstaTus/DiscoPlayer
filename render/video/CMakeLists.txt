# 引入ffmpeg
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)

set( MAC_DISCO_PLAYER_ROOT_DIR ${CMAKE_SOURCE_DIR}/../..)

set(YUV_INCLUDE_DIR ${MAC_DISCO_PLAYER_ROOT_DIR}/dependency/libyuv/include)
# find_path(YUV_INCLUDE_DIR libyuv.h)
message("yuv include "${YUV_INCLUDE_DIR})
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)
# 生成链接库
add_library (DiscoVideo ${DIR_LIB_SRCS})

target_include_directories(DiscoVideo PRIVATE ${AVCODEC_INCLUDE_DIR} ${YUV_INCLUDE_DIR})