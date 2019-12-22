COMMON_DIR_OBJ = ${BUILD_OBJ_DIR}

COMMON_CACHE_SRC_DIR = ./common/cache/
COMMON_STRUCTURE_SRC_DIR = ./common/structure/

COMMON_OBJECTS = ${COMMON_DIR_OBJ}/ConcurrentQueue.o \
		${COMMON_DIR_OBJ}/ConcurrentCachePool.o

common_all : ${COMMON_OBJECTS} show_common_make_level

${COMMON_DIR_OBJ}/ConcurrentQueue.o : ${COMMON_STRUCTURE_SRC_DIR}ConcurrentQueue.cpp \
										${COMMON_STRUCTURE_SRC_DIR}ConcurrentQueue.h
	$(CPP) $(CPPFLAGS) -c ${COMMON_STRUCTURE_SRC_DIR}ConcurrentQueue.cpp -o ${COMMON_DIR_OBJ}/ConcurrentQueue.o

${COMMON_DIR_OBJ}/ConcurrentCachePool.o : ${COMMON_CACHE_SRC_DIR}ConcurrentCachePool.cpp \
									${COMMON_CACHE_SRC_DIR}ConcurrentCachePool.h
	$(CPP) $(CPPFLAGS) -c ${COMMON_CACHE_SRC_DIR}ConcurrentCachePool.cpp -o ${COMMON_DIR_OBJ}/ConcurrentCachePool.o


${COMMON_DIR_OBJ}/clean:
	rm ${COMMON_OBJECTS}

show_common_make_level:
	@echo "COMMON MAKE LEVEL:"$(MAKELEVEL)