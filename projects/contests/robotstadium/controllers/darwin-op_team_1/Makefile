# Webots Makefile system 
#
# You may add some variable definitions hereafter to customize the build process
# See documentation in $(WEBOTS_HOME_PATH)/resources/Makefile.include

RESOURCES_PATH = $(WEBOTS_HOME)/resources/projects/robots/darwin-op
INCLUDE = -I"$(RESOURCES_PATH)/include"
LIBRARIES = "$(RESOURCES_PATH)/lib/managers.a"
LINK_DEPENDENCIES = $(RESOURCES_PATH)/lib/managers.a
CXX_SOURCES = $(wildcard *.cpp)
JAR_FILE = darwin-op_team.jar

### Do not modify: this includes Webots global Makefile.include
space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
