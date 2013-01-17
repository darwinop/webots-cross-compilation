#include "entry_points.hpp"

#include <core/MainApplication.hpp>
#include "CategoryViewer.hpp"

using namespace webotsQtUtils;

static MainApplication *gApplication = NULL;
static CategoryViewer *gCategoryViewer = NULL;

bool wbw_init() {
  gApplication = new MainApplication;
  if (gApplication->isInitialized())
    gCategoryViewer = new CategoryViewer;
  return gApplication->isInitialized();
}

void wbw_cleanup() {
  if (gCategoryViewer) {
    delete gCategoryViewer;
    gCategoryViewer = NULL;
  }
  if (gApplication) {
    delete gApplication;
    gApplication = NULL;
  }
}

void wbw_pre_update_gui() {
  if (gApplication && gApplication->isInitialized())
    gApplication->preUpdateGui();
}

void wbw_update_gui() {
  if (gApplication && gApplication->isInitialized())
    gApplication->updateGui();
}

void wbw_read_sensors() {
  if (gCategoryViewer)
    gCategoryViewer->readSensors();
}

void wbw_write_actuators() {
  if (gCategoryViewer)
    gCategoryViewer->writeActuators();
}

void wbw_show() {
  if (gCategoryViewer)
    gCategoryViewer->showWindow();
}

