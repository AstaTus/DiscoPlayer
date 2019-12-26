CODEC_DIR_OBJ = ${BUILD_OBJ_DIR}

CODEC_SRC_DIR = ./codec/

CODEC_OBJECTS = ${CODEC_DIR_OBJ}/MediaDecoder.o

codec_all : ${CODEC_OBJECTS} show_codec_make_level

${CODEC_DIR_OBJ}/MediaDecoder.o : ${CODEC_SRC_DIR}MediaDecoder.cpp \
								${CODEC_SRC_DIR}MediaDecoder.h
	$(CPP) $(CPPFLAGS) -c ${CODEC_SRC_DIR}MediaDecoder.cpp -o ${CODEC_DIR_OBJ}/MediaDecoder.o

${CODEC_DIR_OBJ}/clean:
	rm ${CODEC_OBJECTS}

show_codec_make_level:
	@echo "CODEC MAKE LEVEL:"$(MAKELEVEL)