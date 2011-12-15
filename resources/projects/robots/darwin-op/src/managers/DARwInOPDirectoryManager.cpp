#include <managers/DARwInOPDirectoryManager.hpp>

#include <cstdlib>

using namespace std;
using namespace managers;

const string &DARwInOPDirectoryManager::getDataDirectory() {
#ifdef CROSSCOMPILATION
  static string path = "/darwin/Data/";
#else
  static string path = string(getenv("WEBOTS_HOME")) + "/resources/projects/robots/darwin-op/darwin/Data/";
#endif
  return path;
}
