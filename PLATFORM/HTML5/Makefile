TARGET = bin/pixfight.html

CXX := emcc

#TOTAL_MEMORY must be a multiple of 16MB

EMFLAGS += -s ALLOW_MEMORY_GROWTH=1 
EMFLAGS += -s TOTAL_MEMORY=268435456 
EMFLAGS += -s NO_EXIT_RUNTIME=1
EMFLAGS += -s DISABLE_EXCEPTION_CATCHING=1
EMFLAGS += --memory-init-file 0

#enable it only if your browser support SharedArrayBuffer (e.g: in chrome -> chrome://flags/)
#EMFLAGS += -s USE_PTHREADS=1

CPPFLAGS += -std=c++14 -w -O2  
CPPFLAGS += $(EMFLAGS)

LIBS := -s USE_GLFW=3 -s FULL_ES2=1 
LIBS += $(EMFLAGS)
LIBS += -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' 
LIBS += -s DEMANGLE_SUPPORT=1 

VPATH += ../../CORE
VPATH += ../../CORE/PathFinding
VPATH += ../../CORE/Rendering
VPATH += ../../CORE/Math
VPATH += ../../CORE/Shader
VPATH += ../../CORE/utilities
VPATH += ../../CORE/utilities/Timer
VPATH += ../../CORE/iFurionGL
VPATH += ../../CORE/GameLogic
VPATH += ../../CORE/AI
VPATH += ../../CORE/Audio
VPATH += ../Linux/

CPPFLAGS += -I../../CORE/
CPPFLAGS += -I../../CORE/AI/
CPPFLAGS += -I../../CORE/PathFinding/
CPPFLAGS += -I../../CORE/Rendering/
CPPFLAGS += -I../../CORE/Math/
CPPFLAGS += -I../../CORE/Shader/
CPPFLAGS += -I../../CORE/utilities/
CPPFLAGS += -I../../CORE/utilities/Timer/
CPPFLAGS += -I../../CORE/iFurionGL/
CPPFLAGS += -I../../CORE/GameLogic/
CPPFLAGS += -I../../CORE/Audio/
CPPFLAGS += -I../Linux/
CPPFLAGS += -I./FMOD/inc/


COMMON_SRC := ./main.cpp \
			  ../../CORE/Core-pch.cpp \
              ../../CORE/PathFinding/SearchNode.cpp \
              ../../CORE/Rendering/DrawingContext.cpp \
              ../../CORE/Rendering/Mesh2d.cpp \
              ../../CORE/Rendering/GameAnimation.cpp \
              ../../CORE/Math/xMath.cpp \
              ../../CORE/Math/xVec3.cpp \
              ../../CORE/Shader/ShaderProgram.cpp \
              ../../CORE/utilities/BasePathExtraction.cpp \
              ../../CORE/utilities/glMatrix4x4.cpp \
              ../../CORE/utilities/glTextureLoader.cpp \
              ../../CORE/utilities/Timer/GameTimer.cpp \
              ../../CORE/utilities/Timer/AndroidTimer.cpp \
              ../../CORE/iFurionGL/DataTypes.cpp \
              ../../CORE/iFurionGL/FGLCollisions.cpp \
              ../../CORE/iFurionGL/FGLDrawingDescriptor.cpp \
              ../../CORE/iFurionGL/FGLMath.cpp \
              ../../CORE/iFurionGL/FGLTexture.cpp \
              ../../CORE/iFurionGL/FGLVector.cpp \
              ../../CORE/iFurionGL/FurionGL.cpp \
              ../../CORE/GameLogic/GameBase.cpp \
              ../../CORE/GameLogic/GameMap.cpp \
              ../../CORE/GameLogic/GameUnit.cpp \
              ../../CORE/GameLogic/GameSync.cpp \
              ../../CORE/GameLogic/GameLogic.cpp \
              ../../CORE/AI/AIObject.cpp \
              ../../CORE/AI/AIPlayer.cpp \
              ../../CORE/AI/AITaskGenerator.cpp \
              ../../CORE/Audio/Audio.cpp \
              ../Linux/LoadGameScene.cpp \
              ../Linux/MenuScene.cpp \
              ../Linux/PFSettings.cpp \
              ../Linux/RenderScene.cpp \
              ../Linux/NewGameScene.cpp \
              ../Linux/SceneManager.cpp \
              ../Linux/SettingsScene.cpp                                                        

COMMON_OBJS := $(patsubst %.cpp, bin/%.bc, $(notdir $(COMMON_SRC)))
COMMON_OBJS += ./FMOD/lib/fmod.bc 

all: copy $(TARGET)

copy: generate
	cp -a ../../CORE/Resources/Lato-Black.ttf bin/data/
	cp -a ../../CORE/Resources/FFFATLAN.TTF bin/data/
	cp -a ../../CORE/Resources/shaders/Mobile/* bin/data/
	cp -a ../../CORE/Resources/textures/* bin/data/
	cp -a ../../CORE/Resources/sfx/* bin/data/
	cp -a ../../CORE/Resources/music/* bin/data/
	cp -a ../../CORE/Resources/maps/* bin/data/
	cp -a ../../CORE/Resources/UI/* bin/data/

generate: clean

	@mkdir -p bin	
	@mkdir -p bin/data	
	@mkdir -p bin/data/save	

clean:
	@rm -rf bin

$(TARGET) : $(COMMON_OBJS)
	$(CXX) $^ -o $@ $(LIBS) --preload-file bin/data@data

bin/%.bc: %.cpp
	$(CXX) $(CPPFLAGS) $< -o $@
