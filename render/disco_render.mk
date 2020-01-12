RENDER_DIR_OBJ = ${BUILD_OBJ_DIR}

RENDER_VIDEO_SRC_DIR = ./render/video/
RENDER_AUDIO_SRC_DIR = ./render/audio/

# VPATH = ./dependency/libyuv/include:./dependency/libyuv/debug

RENDER_VIDEO_OBJECTS = ${RENDER_DIR_OBJ}/Image.o \
		${RENDER_DIR_OBJ}/RenderView.o \
		${RENDER_DIR_OBJ}/VideoTransformNode.o \
		${RENDER_DIR_OBJ}/VideoTransformProcessor.o \
		${RENDER_DIR_OBJ}/VideoFrameTransformer.o \
		${RENDER_DIR_OBJ}/VideoRender.o

RENDER_AUDIO_OBJECTS = ${RENDER_DIR_OBJ}/AudioClip.o \
		${RENDER_DIR_OBJ}/AudioDevice.o \
		${RENDER_DIR_OBJ}/AudioTransformNode.o \
		${RENDER_DIR_OBJ}/AudioTransformProcessor.o \
		${RENDER_DIR_OBJ}/AudioFrameTransformer.o \
		${RENDER_DIR_OBJ}/AudioRender.o


render_all : ${RENDER_VIDEO_OBJECTS} ${RENDER_AUDIO_OBJECTS} show_render_make_level

${RENDER_DIR_OBJ}/Image.o : ${RENDER_VIDEO_SRC_DIR}Image.cpp \
										${RENDER_VIDEO_SRC_DIR}Image.h
	$(CPP) $(CPPFLAGS) -c ${RENDER_VIDEO_SRC_DIR}Image.cpp -o ${RENDER_DIR_OBJ}/Image.o

${RENDER_DIR_OBJ}/RenderView.o : ${RENDER_VIDEO_SRC_DIR}RenderView.cpp \
									${RENDER_VIDEO_SRC_DIR}RenderView.h
	$(CPP) $(CPPFLAGS) -c ${RENDER_VIDEO_SRC_DIR}RenderView.cpp -o ${RENDER_DIR_OBJ}/RenderView.o

${RENDER_DIR_OBJ}/VideoTransformNode.o : ${RENDER_VIDEO_SRC_DIR}VideoTransformNode.h
	$(CPP) $(CPPFLAGS) -c ${RENDER_VIDEO_SRC_DIR}VideoTransformNode.h -o ${RENDER_DIR_OBJ}/VideoTransformNode.o

${RENDER_DIR_OBJ}/VideoTransformProcessor.o : ${RENDER_VIDEO_SRC_DIR}VideoTransformProcessor.h \
						${RENDER_VIDEO_SRC_DIR}VideoTransformProcessor.cpp
	$(CPP) $(CPPFLAGS) -c ${RENDER_VIDEO_SRC_DIR}VideoTransformProcessor.cpp -o ${RENDER_DIR_OBJ}/VideoTransformProcessor.o

${RENDER_DIR_OBJ}/VideoFrameTransformer.o : ${RENDER_VIDEO_SRC_DIR}VideoFrameTransformer.h \
						${RENDER_VIDEO_SRC_DIR}VideoFrameTransformer.cpp
	$(CPP) $(CPPFLAGS) -c ${RENDER_VIDEO_SRC_DIR}VideoFrameTransformer.cpp -o ${RENDER_DIR_OBJ}/VideoFrameTransformer.o

${RENDER_DIR_OBJ}/VideoRender.o : ${RENDER_VIDEO_SRC_DIR}VideoRender.h \
						${RENDER_VIDEO_SRC_DIR}VideoRender.cpp
	$(CPP) $(CPPFLAGS) -c ${RENDER_VIDEO_SRC_DIR}VideoRender.cpp -o ${RENDER_DIR_OBJ}/VideoRender.o

##########################################################################################################################

${RENDER_DIR_OBJ}/AudioClip.o : ${RENDER_AUDIO_SRC_DIR}AudioClip.cpp \
										${RENDER_AUDIO_SRC_DIR}AudioClip.h
	$(CPP) $(CPPFLAGS) -c ${RENDER_AUDIO_SRC_DIR}AudioClip.cpp -o ${RENDER_DIR_OBJ}/AudioClip.o

${RENDER_DIR_OBJ}/AudioDevice.o : ${RENDER_AUDIO_SRC_DIR}AudioDevice.cpp \
									${RENDER_AUDIO_SRC_DIR}AudioDevice.h
	$(CPP) $(CPPFLAGS) -c ${RENDER_AUDIO_SRC_DIR}AudioDevice.cpp -o ${RENDER_DIR_OBJ}/AudioDevice.o

${RENDER_DIR_OBJ}/AudioTransformNode.o : ${RENDER_AUDIO_SRC_DIR}AudioTransformNode.h
	$(CPP) $(CPPFLAGS) -c ${RENDER_AUDIO_SRC_DIR}AudioTransformNode.h -o ${RENDER_DIR_OBJ}/AudioTransformNode.o

${RENDER_DIR_OBJ}/AudioTransformProcessor.o : ${RENDER_AUDIO_SRC_DIR}AudioTransformProcessor.h \
						${RENDER_AUDIO_SRC_DIR}AudioTransformProcessor.cpp
	$(CPP) $(CPPFLAGS) -c ${RENDER_AUDIO_SRC_DIR}AudioTransformProcessor.cpp -o ${RENDER_DIR_OBJ}/AudioTransformProcessor.o

${RENDER_DIR_OBJ}/AudioFrameTransformer.o : ${RENDER_AUDIO_SRC_DIR}AudioFrameTransformer.h \
						${RENDER_AUDIO_SRC_DIR}AudioFrameTransformer.cpp
	$(CPP) $(CPPFLAGS) -c ${RENDER_AUDIO_SRC_DIR}AudioFrameTransformer.cpp -o ${RENDER_DIR_OBJ}/AudioFrameTransformer.o

${RENDER_DIR_OBJ}/AudioRender.o : ${RENDER_AUDIO_SRC_DIR}AudioRender.h \
						${RENDER_AUDIO_SRC_DIR}AudioRender.cpp
	$(CPP) $(CPPFLAGS) -c ${RENDER_AUDIO_SRC_DIR}AudioRender.cpp -o ${RENDER_DIR_OBJ}/AudioRender.o


${RENDER_DIR_OBJ}/clean:
	rm ${RENDER_VIDEO_OBJECTS} ${RENDER_AUDIO_OBJECTS}

show_render_make_level:
	@echo "RENDER MAKE LEVEL:"$(MAKELEVEL)