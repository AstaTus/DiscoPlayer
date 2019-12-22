RENDER_DIR_OBJ = ${BUILD_OBJ_DIR}

RENDER_VIDEO_SRC_DIR = ./render/video/
RENDER_AUDIO_SRC_DIR = ./render/audio/

VPATH = ./dependency/libyuv/include:./dependency/libyuv/debug

RENDER_VIDEO_OBJECTS = ${RENDER_DIR_OBJ}/Image.o \
		${RENDER_DIR_OBJ}/RenderView.o \
		${RENDER_DIR_OBJ}/TransformNode.o \
		${RENDER_DIR_OBJ}/TransformPorcessor.o \
		${RENDER_DIR_OBJ}/VideoFrameTransformer.o \
		${RENDER_DIR_OBJ}/VideoRender.o


render_all : ${RENDER_VIDEO_OBJECTS} show_render_make_level

${RENDER_DIR_OBJ}/Image.o : ${RENDER_VIDEO_SRC_DIR}Image.cpp \
										${RENDER_VIDEO_SRC_DIR}Image.h
	$(CPP) $(CPPFLAGS) -c ${RENDER_VIDEO_SRC_DIR}Image.cpp -o ${RENDER_DIR_OBJ}/Image.o

${RENDER_DIR_OBJ}/RenderView.o : ${RENDER_VIDEO_SRC_DIR}RenderView.cpp \
									${RENDER_VIDEO_SRC_DIR}RenderView.h
	$(CPP) $(CPPFLAGS) -c ${RENDER_VIDEO_SRC_DIR}RenderView.cpp -o ${RENDER_DIR_OBJ}/RenderView.o

${RENDER_DIR_OBJ}/TransformNode.o : ${RENDER_VIDEO_SRC_DIR}TransformNode.h
	$(CPP) $(CPPFLAGS) -c ${RENDER_VIDEO_SRC_DIR}TransformNode.h -o ${RENDER_DIR_OBJ}/TransformNode.o

${RENDER_DIR_OBJ}/TransformPorcessor.o : ${RENDER_VIDEO_SRC_DIR}TransformPorcessor.h \
						${RENDER_VIDEO_SRC_DIR}TransformPorcessor.cpp
	$(CPP) $(CPPFLAGS) -c ${RENDER_VIDEO_SRC_DIR}TransformPorcessor.cpp -o ${RENDER_DIR_OBJ}/TransformPorcessor.o

${RENDER_DIR_OBJ}/VideoFrameTransformer.o : ${RENDER_VIDEO_SRC_DIR}VideoFrameTransformer.h \
						${RENDER_VIDEO_SRC_DIR}VideoFrameTransformer.cpp
	$(CPP) $(CPPFLAGS) -c ${RENDER_VIDEO_SRC_DIR}VideoFrameTransformer.cpp -o ${RENDER_DIR_OBJ}/VideoFrameTransformer.o

${RENDER_DIR_OBJ}/VideoRender.o : ${RENDER_VIDEO_SRC_DIR}VideoRender.h \
						${RENDER_VIDEO_SRC_DIR}VideoRender.cpp
	$(CPP) $(CPPFLAGS) -c ${RENDER_VIDEO_SRC_DIR}VideoRender.cpp -o ${RENDER_DIR_OBJ}/VideoRender.o


${RENDER_DIR_OBJ}/clean:
	rm ${RENDER_VIDEO_OBJECTS}

show_render_make_level:
	@echo "RENDER MAKE LEVEL:"$(MAKELEVEL)