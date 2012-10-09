###############################################################
#
# Purpose: Makefile for building a wrapper between the Webots
#          API and the darwin-op real robot framework
# Author : fabien.rohrer@cyberbotics.com
# License: GPL
# Date   : September 2011
#
###############################################################

# -------------------------------------------------------------
# TO MODIFY:

# pathes
DARWINOP_ROOT = /darwin

# -------------------------------------------------------------

TARGET = wrapper.a
CXX_SOURCES = \
  ../src/Robot.cpp \
  ../src/Servo.cpp \
  ../src/LED.cpp \
  ../src/Accelerometer.cpp \
  ../src/Gyro.cpp \
  ../src/Camera.cpp \
  ../src/Speaker.cpp
OBJECTS = $(CXX_SOURCES:.cpp=.o)
INCLUDE_DIRS = -I$(DARWINOP_ROOT)/Linux/include -I$(DARWINOP_ROOT)/Framework/include -I../include

AR = ar
ARFLAGS = cr
CXX = g++
CXXFLAGS += -c -O2 -DLINUX -Wall $(INCLUDE_DIRS)
DARWINOP_STATIC_LIBRARY = $(DARWINOP_ROOT)/Linux/lib/darwin.a

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)

$(DARWINOP_STATIC_LIBRARY):
	make -C $(DARWINOP_ROOT)/Linux/build

$(TARGET): $(DARWINOP_STATIC_LIBRARY) $(OBJECTS)
	$(AR) $(ARFLAGS) $(TARGET) $(OBJECTS) $(DARWINOP_STATIC_LIBRARY)
	chmod 755 $(TARGET)
