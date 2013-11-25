#!/usr/bin/env python

import os


script_path = os.path.abspath(os.path.dirname(__file__))

# interesting paths in absolute
webots_path = os.path.abspath(script_path + '/../webots')
monitored_directories = [webots_path + '/' + s for s in ['resources/projects/robots/darwin-op', 'projects/robots/darwin-op', 'projects/contests/robotstadium/controllers/darwin-op_team_0', 'projects/contests/robotstadium/controllers/darwin-op_team_1', 'projects/contests/robotstadium/worlds']]

# generate and copy the files
for directory in monitored_directories:
  for dirname, dirnames, filenames in os.walk(directory):
    # deal with exceptions
    if 'build' in dirnames:
      dirnames.remove('build')
    if '.svn' in dirnames:
      dirnames.remove('.svn')
    for filename in filenames:
      f = os.path.join(dirname, filename)
      if filename.endswith('.dylib') or \
         filename.endswith('.cache') or \
         os.access(f, os.X_OK) or \
         filename == '.DS_Store':
        filenames.remove(filename)

    # directories
    for subdirname in dirnames:
      d = os.path.join(dirname, subdirname)
      command = 'mkdir -p ' + d.replace(webots_path, script_path)
      print command
      os.system(command)

    # files
    for filename in filenames:
      f = os.path.join(dirname, filename)
      command = 'cp ' + f + ' ' + f.replace(webots_path, script_path)
      print command
      os.system(command)

