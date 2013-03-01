CXX_SOURCES = $(wildcard *.cpp)
HPP_FILES_TO_MOC = Transfer.hpp

USE_QT = true
USE_C_API = true

QT_UTILS = $(WEBOTS_HOME_PATH)/resources/projects/default/libraries/qt_utils
INCLUDE = -I"$(QT_UTILS)" -I"$(WEBOTS_HOME_PATH)/include/libssh" -I"$(WEBOTS_HOME_PATH)/include/libzip"
LIBRARIES = -L"$(QT_UTILS)" -lqt_utils -L"$(WEBOTS_HOME_PATH)/lib" -lssh -lzip-2

ifeq ($(OS),Windows_NT)
LIBRARIES += -lpthreadGC2
endif

space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
