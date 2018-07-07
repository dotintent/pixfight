TARGET = bin/server

# Flags
CPPFLAGS += -std=c++14 -w -O2

LIBS :=

#bind paths from external souces
VPATH := ../clsocket:../Utils:./:

ifeq ($(OS),Windows_NT)

	BIN := $(BIN).exe
#	LIBS := -llibc

else

	CPPFLAGS += -I/usr/local/include
	CPPFLAGS += -I/usr/include

#	LIBS := -llibc

endif

#additional includes
CPPFLAGS += -I./
CPPFLAGS += -I../clsocket/
CPPFLAGS += -I../Utils/


#source files
COMMON_SRC :=	./main.cpp \
			  	./PFServerRoom.cpp \
              	../clsocket/SimpleSocket.cpp \
              	../clsocket/PassiveSocket.cpp \
              	../clsocket/ActiveSocket.cpp \
              	../Utils/PFSocketClient.cpp \
				../Utils/PFServerUtils.cpp 


#generate bin/*.o paths from files
COMMON_OBJS := $(patsubst %.cpp, bin/%.o, $(notdir $(COMMON_SRC)))

#build
all: generate $(TARGET)
	
#make bin 
generate: clean
ifeq ($(OS),Windows_NT)
	@mkdir bin 2> nul || exit 0
else
	@mkdir -p bin	
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
