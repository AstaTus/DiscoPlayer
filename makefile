ifndef PLATFORM
	PLATFORM = x86_64
endif

ifeq ($(DEBUG_SYMBOLS), TRUE) 
	BUILDDIR = ./build/$(PLATFORM)_debug
else 
	BUILDDIR = ./build/$(PLATFORM)_release
endif 

BUILD_LIB_DIR = ${BUILDDIR}/lib
BUILD_OBJ_DIR = ${BUILDDIR}/obj
BUILD_BIN_DIR = ${BUILDDIR}/bin

export BUILD_LIB_DIR
export BUILD_OBJ_DIR
export BUILD_BIN_DIR

#添加第三方头文件目录，如果你用到了第三方的源码、静态或者动态链接库的话
INCDIR=-I./dependency/libyuv/include \
	-I/Users/laoganbu/lib/QT/5.14.0/Src/**
# INCDIR=-I./dependency/libyuv/include -I./stream -I./render -I./codec -I./common/cache -I./common/structure

#添加静态链接库目录，如果你用到了第三方的静态链接库的话
LIBDIR=-L./dependency/libyuv/debug -L/Users/laoganbu/lib/QT/5.14.0/clang_64/lib/*

.default: all

all : build_dir \
	root_show_make_level \
	stream_all \
	render_all \
	codec_all \
	clock_all \
	player_all \
	player_mac_all \
	disco_player_mac

clean: clear_obj_dir

clear_obj_dir:
	@echo "    Clear directory $@ ..." 
	rm -r $(BUILD_LIB_DIR) $(BUILD_OBJ_DIR) $(BUILD_BIN_DIR)
build_dir: 
	@echo "    Create directory $@ ..." 
	mkdir -p $(BUILD_LIB_DIR) $(BUILD_OBJ_DIR) $(BUILD_BIN_DIR)

root_show_make_level:
	@echo "ROOT MAKE LEVEL:"$(MAKELEVEL)

CPP = clang++
CPPFLAGS = -g -std=c++11 ${INCDIR}

include ./stream/disco_stream.mk
include ./render/disco_render.mk
include ./codec/disco_codec.mk
include ./clock/disco_clock.mk
include ./player/disco_player.mk
include ./player/platform/mac/disco_player_mac.mk

MAC_OBJECTS = ${STREAM_OBJECTS} \
				${RENDER_VIDEO_OBJECTS} \
				${RENDER_AUDIO_OBJECTS} \
				${PLAYER_OBJECTS} \
				${PLAYER_MAC_OBJECTS} \
				${CODEC_OBJECTS} \
				${CLOCK_OBJECTS}

disco_player_mac : $(MAC_OBJECTS) 
	$(CC) $(FLAGS) -o $(BUILD_BIN_DIR)/disco_player_mac $(MAC_OBJECTS) -v ${LIBDIR} -lstdc++ -lavutil -lavcodec -lavformat -lswscale -lSDL2 -lyuv -lswresample