MANAGERS_SOURCES_PATH = ./src
MANAGERS_INCLUDE_PATH = ./include
DARWIN_FRAMEWORK_PATH = ../darwin/darwin/Framework
WEBOTS_INCLUDE_PATH = $(WEBOTS_HOME)/include/controller/cpp

CXX_SOURCES = \
  $(MANAGERS_SOURCES_PATH)/DARwInOPDirectoryManager.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPMotionManager.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPGaitManager.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPVisionManager.cpp 

INCLUDE += -I"$(DARWIN_FRAMEWORK_PATH)/include" -I"$(MANAGERS_INCLUDE_PATH)" -I"$(WEBOTS_INCLUDE_PATH)"
CFLAGS  += -DWEBOTS

space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
