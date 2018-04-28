TARGET = bin/editor

# Flags
CPPFLAGS += -std=c++14 -w -O2 -DMAP_EDITOR=1

LIBS :=

#bind paths from external souces
VPATH := ../CORE:../CORE/PathFinding:../CORE/Rendering:../CORE/Math:../CORE/Shader:../CORE/utilities:../CORE/iFurionGL:../CORE/GameLogic

# libs finding and binding
ifeq ($(OS),Windows_NT)
BIN := $(BIN).exe
	LIBS := -lopengl32 -lm -lGLU32 -lglfw3dll -lglew32
else
	UNAME_S := $(shell uname -s)
	GLFW3 := $(shell pkg-config --libs glfw3)
	ifeq ($(UNAME_S),Darwin)
		LIBS := $(GLFW3) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lm -lGLEW -L/usr/local/lib
		CPPFLAGS += -I/usr/local/include
	else
		LIBS := $(GLFW3) -lGL -lm -lGLU -lGLEW
	endif
endif

#additional includes
CPPFLAGS += -I../CORE/
CPPFLAGS += -I../CORE/AI/
CPPFLAGS += -I../CORE/PathFinding/
CPPFLAGS += -I../CORE/Rendering/
CPPFLAGS += -I../CORE/Math/
CPPFLAGS += -I../CORE/Shader/
CPPFLAGS += -I../CORE/utilities/
CPPFLAGS += -I../CORE/iFurionGL/
CPPFLAGS += -I../CORE/GameLogic/
CPPFLAGS += -I../PLATFORM/Linux/

ifeq ($(OS),Windows_NT)

CPPFLAGS += -m32

endif

#source files
COMMON_SRC := ./main.cpp \
			  ./PFMapEditor.cpp \
              ../CORE/Core-pch.cpp \
              ../CORE/PathFinding/SearchNode.cpp \
              ../CORE/Rendering/DrawingContext.cpp \
              ../CORE/Rendering/Mesh2d.cpp \
              ../CORE/Math/xMath.cpp \
              ../CORE/Math/xVec3.cpp \
              ../CORE/Shader/ShaderProgram.cpp \
              ../CORE/utilities/BasePathExtraction.cpp \
              ../CORE/utilities/glMatrix4x4.cpp \
              ../CORE/utilities/glTextureLoader.cpp \
              ../CORE/iFurionGL/DataTypes.cpp \
              ../CORE/iFurionGL/FGLCollisions.cpp \
              ../CORE/iFurionGL/FGLDrawingDescriptor.cpp \
              ../CORE/iFurionGL/FGLMath.cpp \
              ../CORE/iFurionGL/FGLTexture.cpp \
              ../CORE/iFurionGL/FGLVector.cpp \
              ../CORE/iFurionGL/FurionGL.cpp \
              ../CORE/GameLogic/GameBase.cpp \
              ../CORE/GameLogic/GameMap.cpp \
              ../CORE/GameLogic/GameUnit.cpp

#generate bin/*.o paths from files
COMMON_OBJS := $(patsubst %.cpp, bin/%.o, $(notdir $(COMMON_SRC)))

#build
all: copy $(TARGET)

#copy files
copy: generate
	cp -a ../CORE/Resources/maps/* bin/maps/
	cp -a ./icons/* bin/data/
	cp -a ../CORE/Resources/Lato-Black.ttf bin/data/
	cp -a ../CORE/Resources/shaders/Desktop/tile.vsh bin/data/
	cp -a ../CORE/Resources/shaders/Desktop/tile.fsh bin/data/
	cp -a ../CORE/Resources/textures/* bin/data/
	@rm -rf bin/data/icons/

ifeq ($(OS),Windows_NT)
	
	cp -a ./glfw3.dll bin/
	cp -a ./glew32.dll bin/

endif
	
#make bin 
generate: clean
ifeq ($(OS),Windows_NT)
	@mkdir bin 2> nul || exit 0
else
	@mkdir -p bin	
endif

ifeq ($(OS),Windows_NT)
	@mkdir bin/maps 2> nul || exit 0
else
	@mkdir -p bin/maps	
endif

ifeq ($(OS),Windows_NT)
	@mkdir bin/data 2> nul || exit 0
else
	@mkdir -p bin/data	
endif

#clean before build
clean:
	@rm -rf bin

#for each *.o file link with editor
$(TARGET) : $(COMMON_OBJS)
	@echo linking $@
	$(CXX) $^ -o $@ $(LIBS)

#for each *.o file compile coresponding *.cpp file and put to bin/*.o
bin/%.o: %.cpp
	@echo compiling $< into $@
	$(CXX) $(CPPFLAGS) -c $< -o $@
