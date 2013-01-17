/*
 * File:          CategoryViewer.hpp
 * Date:          December 2012
 * Description:   View all the supported webots devices in a tab based window sorted by categories
 * Author:        fabien.rohrer@cyberbotics.com
 * Modifications: January 2013 add of a new tab for the communication with the DARwIn-OP by david.mansolino@epfl.ch
 */

#ifndef CATEGORY_VIEWER_HPP
#define CATEGORY_VIEWER_HPP

#include <gui/MainWindow.hpp>

#include <QtCore/QList>

class CategoryWidget;

namespace webotsQtUtils {
  class Device;
  class DeviceWidget;
}

class CategoryViewer : public webotsQtUtils::MainWindow
{
  public:
    CategoryViewer();
    virtual ~CategoryViewer();

    void readSensors();
    void writeActuators();

  protected:
    CategoryWidget *findCategory(const QString &name) const;

    QTabWidget *mTabWidget;
    QList<webotsQtUtils::Device *> mDeviceList;
    QList<webotsQtUtils::DeviceWidget *> mDeviceWidgetList;
    QList<CategoryWidget *> mCategoryList;
};

#endif

