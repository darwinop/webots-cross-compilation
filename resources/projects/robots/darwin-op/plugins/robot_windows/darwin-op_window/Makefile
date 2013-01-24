CPP_SOURCES = $(wildcard *.cpp)
HPP_FILES_TO_MOC = Transfer.hpp

USE_QT = true
USE_C_API = true

QT_UTILS = $(WEBOTS_HOME_PATH)/resources/projects/default/libraries/qt_utils
CFLAGS = -I"$(QT_UTILS)"
LIBRARIES = -L"$(QT_UTILS)" -lqt_utils -lssh -ltar

space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
