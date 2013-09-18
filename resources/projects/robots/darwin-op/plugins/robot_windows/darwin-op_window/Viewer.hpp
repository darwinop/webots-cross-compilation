/*
 * File:         Viewer.hpp
 * Date:         January 2013
 * Description:  Add a new tab at CategoryViewer to interact with DARwIn-OP robot 
 * Author:       david.mansolino@epf.ch
 * Modifications:
 */

#ifndef VIEWER_HPP
#define VIEWER_HPP

#include <gui/GenericWindow.hpp>

class Viewer : public webotsQtUtils::GenericWindow
{
  public:
             Viewer();
    virtual ~Viewer();
};

#endif

