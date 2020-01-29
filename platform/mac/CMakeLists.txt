cmake_minimum_required (VERSION 3.16.0)

#指定用C++ 11编译
set(CMAKE_CXX_STANDARD 11)
#增加调试选项
add_definitions("-Wall -g")

set( MAC_DISCO_PLAYER_ROOT_DIR ${CMAKE_SOURCE_DIR}/../..)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MAC_DISCO_PLAYER_ROOT_DIR}/cmake-modules)

project (MacDiscoPlayer)
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL)
# find_library(SDL2_LIBRARY NAME SDL)
find_package(SDL2 REQUIRED)
if(SDL2_FOUND)
    message("sdl2 found")
endif(SDL2_FOUND)

find_package(FFMPEG REQUIRED)
if(FFMPEG_FOUND)
    message("FFMPEG found")
endif(FFMPEG_FOUND)

find_package(Qt5 COMPONENTS Core Gui Widgets Test Concurrent OpenGL Qml Quick REQUIRED)
if(Qt5_FOUND)
    message("Qt5 found")
endif(Qt5_FOUND)

find_package(OPENGL REQUIRED)
if(OPENGL_FOUND)
    message("OPENGL found")
endif(OPENGL_FOUND)

message("Qt5Widgets_LIBRARIES"${Qt5Widgets_LIBRARIES})
message("Qt5Core_LIBRARIES"${Qt5Core_LIBRARIES})
message("Qt5Gui_LIBRARIES"${Qt5Gui_LIBRARIES})

message(${MAC_DISCO_PLAYER_ROOT_DIR})
message("SWRESAMPLE_INCLUDE_DIR"${SWRESAMPLE_INCLUDE_DIR})
message("SWRESAMPLE_LIBRARY"${SWRESAMPLE_LIBRARIES})

# run moc automatically
set(CMAKE_AUTOMOC ON)
# if you're using Widgets and editing .ui forms in Designer
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(YUV_INCLUDE_DIR ${MAC_DISCO_PLAYER_ROOT_DIR}/dependency/libyuv/include)
set(YUV_LIBRARY ${MAC_DISCO_PLAYER_ROOT_DIR}/dependency/libyuv/debug/libyuv.dylib)
# 添加其他模块
set( DISCO_CLOCK_DIR ${MAC_DISCO_PLAYER_ROOT_DIR}/clock)
set( DISCO_CODEC_DIR ${MAC_DISCO_PLAYER_ROOT_DIR}/codec)
set( DISCO_COMMON_DIR ${MAC_DISCO_PLAYER_ROOT_DIR}/common)
set( DISCO_PLAYER_DIR ${MAC_DISCO_PLAYER_ROOT_DIR}/player)
set( DISCO_RENDER_DIR ${MAC_DISCO_PLAYER_ROOT_DIR}/render)
set( DISCO_STREAM_DIR ${MAC_DISCO_PLAYER_ROOT_DIR}/stream)
set( DISCO_COMMON_DIR ${MAC_DISCO_PLAYER_ROOT_DIR}/common)
add_subdirectory(${DISCO_CLOCK_DIR} clock)
add_subdirectory(${DISCO_CODEC_DIR} codec)
add_subdirectory(${DISCO_COMMON_DIR} common)
add_subdirectory(${DISCO_PLAYER_DIR} player)
add_subdirectory(${DISCO_RENDER_DIR} render)
add_subdirectory(${DISCO_STREAM_DIR} stream)
#添加资源文件
set(RCC_FILES ./resources/resource.qrc)

set(UI_FILES ./resources/mainwindow.ui)

# 添加shader
file(GLOB SHADER_FILES *.frag *.vert *.glsl)
message("RES_FILES"${SHADER_FILES})

add_executable(MacDiscoPlayer ${SHADER_FILES} ${DIR_SRCS} ${RCC_FILES} ${UI_FILES})

# 添加依赖的头文件目录
target_include_directories(MacDiscoPlayer PRIVATE 
            ${SDL2_INCLUDE_DIRS}
            ${AVUTIL_INCLUDE_DIR}
            ${Qt5Widgets_INCLUDE_DIRS}
            ${Qt5Core_INCLUDE_DIRS}
            ${Qt5Gui_INCLUDE_DIRS}
            ${OPENGL_EGL_INCLUDE_DIRS})
            
# message(${DiscoClock})
# message(${DiscoPlayer})
# 添加链接库
target_link_libraries(MacDiscoPlayer 
        ${SDL2_LIBRARIES}
        ${AVCODEC_LIBRARIES}
        ${AVFORMAT_LIBRARIES}
        ${AVUTIL_LIBRARIES}
        ${SWRESAMPLE_LIBRARIES}
        ${YUV_LIBRARY}
        ${Qt5Widgets_LIBRARIES}
        ${Qt5Core_LIBRARIES}
        ${Qt5Gui_LIBRARIES}
        ${OPENGL_glu_LIBRARY}
        DiscoClock
        DiscoCodec
        DiscoPlayer
        DiscoVideo
        DiscoAudio
        DiscoStream
        DiscoCommon)    