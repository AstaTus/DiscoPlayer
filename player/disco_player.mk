PLAYER_DIR_OBJ = ${BUILD_OBJ_DIR}

PLAYER_SRC_DIR = ./player/

PLAYER_OBJECTS = ${PLAYER_DIR_OBJ}/CorePlayer.o \
				${PLAYER_DIR_OBJ}/PlayItem.o

player_all : ${PLAYER_OBJECTS} show_player_make_level

${PLAYER_DIR_OBJ}/CorePlayer.o : ${PLAYER_SRC_DIR}CorePlayer.cpp \
										${PLAYER_SRC_DIR}CorePlayer.h
	$(CPP) $(CPPFLAGS) -c ${PLAYER_SRC_DIR}CorePlayer.cpp -o ${PLAYER_DIR_OBJ}/CorePlayer.o

${PLAYER_DIR_OBJ}/PlayItem.o : ${PLAYER_SRC_DIR}PlayItem.cpp \
										${PLAYER_SRC_DIR}PlayItem.h
	$(CPP) $(CPPFLAGS) -c ${PLAYER_SRC_DIR}PlayItem.cpp -o ${PLAYER_DIR_OBJ}/PlayItem.o

${PLAYER_DIR_OBJ}/clean:
	rm ${PLAYER_OBJECTS}

show_player_make_level:
	@echo "PLAYER MAKE LEVEL:"$(MAKELEVEL)