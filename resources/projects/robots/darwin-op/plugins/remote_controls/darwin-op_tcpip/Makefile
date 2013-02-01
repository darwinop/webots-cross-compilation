CXX_SOURCES = $(wildcard *.cpp)

USE_QT = true
USE_C_API = true

LIBRARIES = -ljpeg

space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
