#include "CategoryViewer.hpp"
#include "CategoryWidget.hpp"
#include "Transfer.hpp"

#include <widgets/Device.hpp>
#include <widgets/DeviceWidget.hpp>
#include <widgets/DeviceWidgetFactory.hpp>

using namespace webotsQtUtils;

CategoryViewer::CategoryViewer() :
  MainWindow()
{
  setStyleSheet("QWidget#borderedWidget { border: 2px groove darkGray; border-radius: 8px; }");
  setFixedWidth(810);

  QString title(QString("Robot View: %1").arg(wb_robot_get_name()));
  setWindowTitle(title);
  
  setUnifiedTitleAndToolBarOnMac(true);
  
  mTabWidget = new QTabWidget(this);
  mTabWidget->setMovable(true);
  
  mDeviceList.append(new Device((WbDeviceTag) 0));
  for (int index=0; index<wb_robot_get_number_of_devices(); index++) {
    WbDeviceTag tag = wb_robot_get_device_by_index(index);
    mDeviceList.append(new Device(tag));
  }

  foreach (Device *device, mDeviceList) {
    const QString &categoryName = device->category();
    if (categoryName != "Unknown") {
      CategoryWidget *category = findCategory(categoryName);
      if (category == NULL) {
        category = new CategoryWidget(categoryName);
        mCategoryList.append(category);
        mTabWidget->addTab(category, categoryName);
      }
      DeviceWidget *widget = DeviceWidgetFactory::createDeviceWidget(device);
      if (widget) {
        mDeviceWidgetList.append(widget);
        category->addWidget(widget);
      }
    }
  }
  
  mTabWidget->addTab(new Transfer, QString("Transfer"));
  
  setCentralWidget(mTabWidget);
}

CategoryViewer::~CategoryViewer() {
}

void CategoryViewer::readSensors() {
  foreach (DeviceWidget *widget, mDeviceWidgetList)
    widget->readSensors();
}

void CategoryViewer::writeActuators() {
  foreach (DeviceWidget *widget, mDeviceWidgetList)
    widget->writeActuators();
}

CategoryWidget *CategoryViewer::findCategory(const QString &name) const {
  foreach (CategoryWidget *category, mCategoryList) {
    if (category->name() == name)
      return category;
  }
  return NULL;
}
