CPP_SOURCES = \
  CategoryViewer.cpp \
  CategoryWidget.cpp \
  entry_points.cpp \
  SSH.cpp \
  Transfer.cpp

HPP_FILES_TO_MOC = Transfer.hpp

USE_QT = 1
USE_C_API = 1

QT_UTILS = $(WEBOTS_HOME_PATH)/resources/projects/default/libraries/qt_utils
CFLAGS = -I$(QT_UTILS)
LIBRARIES = -L$(QT_UTILS) -lqt_utils -lssh -ltar

space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
