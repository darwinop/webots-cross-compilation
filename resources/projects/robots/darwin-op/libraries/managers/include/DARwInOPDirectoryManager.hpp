// File:          DARwInOPDirectoryManager.hpp
// Date:          20th of September 2011
// Description:   Helper class allowing to retrieve directories
// Author:        fabien.rohrer@cyberbotics.com

#ifndef DARWINOP_DIRECTORY_MANAGER_HPP
#define DARWINOP_DIRECTORY_MANAGER_HPP

#include <string>

namespace managers {
  class DARwInOPDirectoryManager {
    public:
      static const std::string &getDataDirectory();
  };
}

#endif
