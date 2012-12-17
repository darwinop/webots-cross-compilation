###############################################################
#
# Purpose: Makefile for "check_start_position"
# Author: david.mansolino@epfl.ch
# Date: 11 December 2012
#
###############################################################

TARGET = check_start_position

CXX = g++
INCLUDE_DIRS = -I../../../include -I../../../../Framework/include
CXXFLAGS +=	-O2 -DLINUX -g -Wall -fmessage-length=0 $(INCLUDE_DIRS)
LIBS += -lpthread -lrt

OBJS =	main.o


all: darwin.a $(TARGET)

darwin.a:
	make -C ../../../build

$(TARGET): $(OBJS) ../../../lib/darwin.a
	$(CXX) -o $(TARGET) $(OBJS) ../../../lib/darwin.a $(LIBS)
	
clean:
	rm -f $(OBJS) $(TARGET)
