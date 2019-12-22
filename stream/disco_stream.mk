STREAM_DIR_OBJ = ${BUILD_OBJ_DIR}

STREAM_SRC_DIR = ./stream/

STREAM_OBJECTS = ${STREAM_DIR_OBJ}/InputStreamIterator.o \
		${STREAM_DIR_OBJ}/MediaInputStream.o \
		${STREAM_DIR_OBJ}/PacketReader.o \
		${STREAM_DIR_OBJ}/Reader.o


stream_all : ${STREAM_OBJECTS} show_make_level

${STREAM_DIR_OBJ}/InputStreamIterator.o : ${STREAM_SRC_DIR}InputStreamIterator.cpp \
										${STREAM_SRC_DIR}InputStreamIterator.h \
										${STREAM_SRC_DIR}IStreamIterator.h
	$(CPP) $(CPPFLAGS) -c ${STREAM_SRC_DIR}InputStreamIterator.cpp -o ${STREAM_DIR_OBJ}/InputStreamIterator.o

${STREAM_DIR_OBJ}/MediaInputStream.o : ${STREAM_SRC_DIR}MediaInputStream.cpp \
									${STREAM_SRC_DIR}MediaInputStream.h
	$(CPP) $(CPPFLAGS) -c ${STREAM_SRC_DIR}MediaInputStream.cpp -o ${STREAM_DIR_OBJ}/MediaInputStream.o

${STREAM_DIR_OBJ}/PacketReader.o : ${STREAM_SRC_DIR}PacketReader.cpp \
								${STREAM_SRC_DIR}PacketReader.h
	$(CPP) $(CPPFLAGS) -c ${STREAM_SRC_DIR}PacketReader.cpp -o ${STREAM_DIR_OBJ}/PacketReader.o

${STREAM_DIR_OBJ}/Reader.o : ${STREAM_SRC_DIR}Reader.h \
						${STREAM_SRC_DIR}Reader.cpp
	$(CPP) $(CPPFLAGS) -c ${STREAM_SRC_DIR}Reader.cpp -o ${STREAM_DIR_OBJ}/Reader.o

${STREAM_DIR_OBJ}/clean:
	rm ${STREAM_OBJECTS}

show_make_level:
	@echo "STREAM MAKE LEVEL:"$(MAKELEVEL)