COMMON_DIR_OBJ = ${BUILD_OBJ_DIR}

COMMON_CACHE_SRC_DIR = ./common/cache/
COMMON_STRUCTURE_SRC_DIR = ./common/structure/
COMMON_LOG_SRC_DIR = ./common/log/


COMMON_OBJECTS = ${COMMON_DIR_OBJ}/Log.o

common_all : ${COMMON_OBJECTS} show_common_make_level

${COMMON_DIR_OBJ}/Log.o : ${COMMON_LOG_SRC_DIR}Log.cpp \
									${COMMON_LOG_SRC_DIR}Log.h
	$(CPP) $(CPPFLAGS) -c ${COMMON_LOG_SRC_DIR}Log.cpp -o ${COMMON_DIR_OBJ}/Log.o


${COMMON_DIR_OBJ}/clean:
	rm ${COMMON_OBJECTS}

show_common_make_level:
	@echo "COMMON MAKE LEVEL:"$(MAKELEVEL)