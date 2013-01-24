# Webots Makefile system 
#
# You may add some variable definitions hereafter to customize the build process
# See documentation in $(WEBOTS_HOME_PATH)/resources/Makefile.include


RESOURCES_PATH = $(WEBOTS_HOME)/resources/projects/robots/darwin-op
PROTECTED_RESOURCES_PATH = $(subst $(space),\ ,$(strip $(subst \,/,$(RESOURCES_PATH))))
CFLAGS = -I"$(RESOURCES_PATH)/include" -I"$(RESOURCES_PATH)/darwin/Framework/include"
LIBRARIES = "$(RESOURCES_PATH)/lib/managers.a"
LINK_DEPENDENCIES = $(PROTECTED_RESOURCES_PATH)/lib/managers.a
CPP_SOURCES = main.cpp Soccer.cpp

### Do not modify: this includes Webots global Makefile.include
space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
