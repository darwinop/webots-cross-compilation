ifndef OSTYPE
OSTYPE=$(shell uname)
endif

ifeq ($(OSTYPE),Darwin)
OSTYPE=darwin
endif

MANAGERS_SOURCES_PATH = ../src/managers
MANAGERS_INCLUDE_PATH = ../include
DARWIN_FRAMEWORK_PATH = ../darwin/Framework
WEBOTS_INCLUDE_PATH = $(WEBOTS_HOME)/include/controller/cpp
TARGET = managers.a

CXX_SOURCES = \
  $(DARWIN_FRAMEWORK_PATH)/src/math/Matrix.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/math/Vector.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/math/Point.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/motion/JointData.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/motion/MotionStatus.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/motion/modules/Action.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/motion/modules/Walking.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/vision/ImgProcess.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/vision/ColorFinder.cpp \
  $(DARWIN_FRAMEWORK_PATH)/src/vision/Image.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPDirectoryManager.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPMotionManager.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPGaitManager.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPVisionManager.cpp
C_SOURCES = \
  $(DARWIN_FRAMEWORK_PATH)/src/minIni/minIni.c
SOURCES = $(C_SOURCES) $(CXX_SOURCES)

ifeq ($(OSTYPE),darwin)
EXTRA_FLAGS = -mmacosx-version-min=10.7 -arch i386 -arch x86_64
AR          = libtool -o
else
EXTRA_FLAGS =
AR          = ar cr
endif

CC        = gcc
CXX       = g++
OBJECTS   = $(notdir $(CXX_SOURCES:.cpp=.o) $(C_SOURCES:.c=.o))
CFLAGS   += -c -O2 -Wall -DWEBOTS -I$(DARWIN_FRAMEWORK_PATH)/include -I$(MANAGERS_INCLUDE_PATH) -I$(WEBOTS_INCLUDE_PATH)
CXXFLAGS += -c -O2 -Wall -DWEBOTS -I$(DARWIN_FRAMEWORK_PATH)/include -I$(MANAGERS_INCLUDE_PATH) -I$(WEBOTS_INCLUDE_PATH)
DEPENDENCIES = $(OBJECTS:.o=.d)

.PHONY: clean build

release debug profile: $(TARGET)

vpath %.c   $(dir $(SOURCES))
vpath %.cpp $(dir $(SOURCES))

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPENDENCIES)
endif

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPENDENCIES)

$(TARGET): $(OBJECTS)
	$(AR) $(TARGET) $(OBJECTS)

%.d:%.cpp
	$(CXX) $(CXXFLAGS) -MM $< > $@

%.d:%.c
	$(CC) $(CFLAGS) -MM $< > $@

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(EXTRA_FLAGS) $< -o $@

%.o:%.c
	$(CC) $(CFLAGS) $(EXTRA_FLAGS) $< -o $@
