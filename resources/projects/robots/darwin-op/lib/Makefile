MANAGERS_SOURCES_PATH = ../src/managers
MANAGERS_INCLUDE_PATH = ../include
DARWIN_FRAMEWORK_PATH = ../darwin/Framework

TARGET = managers.a

CXX_SOURCES = \
  $(DARWIN_FRAMEWORK_PATH)/src/math/Matrix.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/math/Vector.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/math/Point.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/motion/JointData.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/motion/MotionStatus.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/motion/modules/Action.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/motion/modules/Walking.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPDirectoryManager.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPMotionManager.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPGaitManager.cpp
C_SOURCES = \
  $(DARWIN_FRAMEWORK_PATH)/src/minIni/minIni.c
SOURCES = $(C_SOURCES) $(CXX_SOURCES)

AR        = ar
ARFLAGS   = cr
CC        = gcc
CXX       = g++
OBJECTS   = $(notdir $(CXX_SOURCES:.cpp=.o) $(C_SOURCES:.c=.o))
CFLAGS   += -c -O2 -Wall -DWEBOTS -I$(DARWIN_FRAMEWORK_PATH)/include -I$(MANAGERS_INCLUDE_PATH) -I$(WEBOTS_HOME)/include/controller/cpp
CXXFLAGS += -c -O2 -Wall -DWEBOTS -I$(DARWIN_FRAMEWORK_PATH)/include -I$(MANAGERS_INCLUDE_PATH) -I$(WEBOTS_HOME)/include/controller/cpp
LIBS     += -L"$(WEBOTS_HOME)/lib" -lController -lCppController
DEPENDENCIES = $(OBJECTS:.o=.d)

.PHONY : clean all compil debug release

vpath %.c   $(dir $(SOURCES))
vpath %.cpp $(dir $(SOURCES))

all compil debug release: $(TARGET)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(DEPENDENCIES),)
-include $(DEPENDENCIES)
endif
endif

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPENDENCIES)

$(TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $(TARGET) $(OBJECTS)

%.d:%.cpp
	$(CXX) $(CXXFLAGS) $(LIBS) -MM $< > $@

%.d:%.c
	$(CC) $(CFLAGS) $(LIBS) -MM $< > $@

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(LIBS) $< -o $@

%.o:%.c
	$(CC) $(CFLAGS) $(LIBS) $< -o $@

test:
	echo $(dir $(SOURCES))
