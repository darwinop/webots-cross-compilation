#include "Transfer.hpp"
#include "Viewer.hpp"

using namespace webotsQtUtils;

Viewer::Viewer() :
  GenericWindow()
{
  mTabWidget->addTab(new Transfer(this), "Transfer");
}

Viewer::~Viewer() {
}
