MANAGERS_SOURCES_PATH = ./src
MANAGERS_INCLUDE_PATH = ./include
DARWIN_FRAMEWORK_PATH = ../../darwin/Framework
WEBOTS_INCLUDE_PATH = $(WEBOTS_HOME)/include/controller/cpp

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
  $(DARWIN_FRAMEWORK_PATH)/src/WebotsStatus.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPDirectoryManager.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPMotionManager.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPGaitManager.cpp \
  $(MANAGERS_SOURCES_PATH)/DARwInOPVisionManager.cpp 
C_SOURCES = \
  $(DARWIN_FRAMEWORK_PATH)/src/minIni/minIni.c

INCLUDE += -I$(DARWIN_FRAMEWORK_PATH)/include -I$(MANAGERS_INCLUDE_PATH) -I$(WEBOTS_INCLUDE_PATH)
CFLAGS  += -c -O2 -DWEBOTS

space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
