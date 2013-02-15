CXX_SOURCES = $(wildcard *.cpp)

USE_QT = true
USE_C_API = true

QT_UTILS = $(WEBOTS_HOME_PATH)/resources/projects/default/libraries/qt_utils
INCLUDE = -I"$(QT_UTILS)"
LIBRARIES = -L"$(QT_UTILS)" -lqt_utils

space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
