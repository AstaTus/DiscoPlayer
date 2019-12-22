CLOCK_DIR_OBJ = ${BUILD_OBJ_DIR}

CLOCK_SRC_DIR = ./clock/

CLOCK_OBJECTS = ${CLOCK_DIR_OBJ}/Clock.o \
				${CLOCK_DIR_OBJ}/AudioMasterSyncStrategy.o \
				${CLOCK_DIR_OBJ}/SyncClockManager.o \
				${CLOCK_DIR_OBJ}/SyncStrategy.o \
				${CLOCK_DIR_OBJ}/VideoMasterSyncStrategy.o

clock_all : ${CLOCK_OBJECTS} show_clock_make_level

${CLOCK_DIR_OBJ}/Clock.o : ${CLOCK_SRC_DIR}Clock.cpp \
										${CLOCK_SRC_DIR}Clock.h
	$(CPP) $(CPPFLAGS) -c ${CLOCK_SRC_DIR}Clock.cpp -o ${CLOCK_DIR_OBJ}/Clock.o

${CLOCK_DIR_OBJ}/AudioMasterSyncStrategy.o : ${CLOCK_SRC_DIR}AudioMasterSyncStrategy.cpp \
										${CLOCK_SRC_DIR}AudioMasterSyncStrategy.h
	$(CPP) $(CPPFLAGS) -c ${CLOCK_SRC_DIR}AudioMasterSyncStrategy.cpp -o ${CLOCK_DIR_OBJ}/AudioMasterSyncStrategy.o

${CLOCK_DIR_OBJ}/SyncClockManager.o : ${CLOCK_SRC_DIR}SyncClockManager.cpp \
										${CLOCK_SRC_DIR}SyncClockManager.h
	$(CPP) $(CPPFLAGS) -c ${CLOCK_SRC_DIR}SyncClockManager.cpp -o ${CLOCK_DIR_OBJ}/SyncClockManager.o

${CLOCK_DIR_OBJ}/SyncStrategy.o : ${CLOCK_SRC_DIR}SyncStrategy.cpp \
										${CLOCK_SRC_DIR}SyncStrategy.h
	$(CPP) $(CPPFLAGS) -c ${CLOCK_SRC_DIR}SyncStrategy.cpp -o ${CLOCK_DIR_OBJ}/SyncStrategy.o

${CLOCK_DIR_OBJ}/VideoMasterSyncStrategy.o : ${CLOCK_SRC_DIR}VideoMasterSyncStrategy.cpp \
										${CLOCK_SRC_DIR}VideoMasterSyncStrategy.h
	$(CPP) $(CPPFLAGS) -c ${CLOCK_SRC_DIR}VideoMasterSyncStrategy.cpp -o ${CLOCK_DIR_OBJ}/VideoMasterSyncStrategy.o

${CLOCK_DIR_OBJ}/clean:
	rm ${CLOCK_OBJECTS}

show_clock_make_level:
	@echo "CLOCK MAKE LEVEL:"$(MAKELEVEL)