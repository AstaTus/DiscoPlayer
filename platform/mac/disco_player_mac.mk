PLAYER_MAC_DIR_OBJ = ${BUILD_OBJ_DIR}

PLAYER_MAC_SRC_DIR = ./player/platform/mac/

PLAYER_MAC_OBJECTS = ${PLAYER_MAC_DIR_OBJ}/SDLRenderView.o \
					${PLAYER_MAC_DIR_OBJ}/SDLAudioDevice.o \
					${PLAYER_MAC_DIR_OBJ}/test.o

player_mac_all : ${PLAYER_OBJECTS} show_player_mac_make_level

${PLAYER_MAC_DIR_OBJ}/SDLRenderView.o : ${PLAYER_MAC_SRC_DIR}SDLRenderView.cpp \
										${PLAYER_MAC_SRC_DIR}SDLRenderView.h
	$(CPP) $(CPPFLAGS) -c ${PLAYER_MAC_SRC_DIR}SDLRenderView.cpp -o ${PLAYER_MAC_DIR_OBJ}/SDLRenderView.o

${PLAYER_MAC_DIR_OBJ}/SDLAudioDevice.o : ${PLAYER_MAC_SRC_DIR}SDLAudioDevice.cpp \
										${PLAYER_MAC_SRC_DIR}SDLAudioDevice.h
	$(CPP) $(CPPFLAGS) -c ${PLAYER_MAC_SRC_DIR}SDLAudioDevice.cpp -o ${PLAYER_MAC_DIR_OBJ}/SDLAudioDevice.o

${PLAYER_MAC_DIR_OBJ}/test.o : ${PLAYER_MAC_SRC_DIR}test.cpp
	$(CPP) $(CPPFLAGS) -c ${PLAYER_MAC_SRC_DIR}test.cpp -o ${PLAYER_MAC_DIR_OBJ}/test.o

${PLAYER_MAC_DIR_OBJ}/clean:
	rm ${PLAYER_MAC_OBJECTS}

show_player_mac_make_level:
	@echo "PLAYER MAC MAKE LEVEL:"$(MAKELEVEL)