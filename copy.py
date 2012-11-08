#!/usr/bin/env python

# Description: copy all the files inside the script directory to the destination directory
# Author:      fabien.rohrer@cyberbotics.com
# Date:        3th October 2012

import sys, os, os.path, inspect, shutil


def scriptdirectory():
  return os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))


def help():
  print 'description: copy all the files inside the script directory to the destination directory'
  print 'usage:       ' + sys.argv[0] + ' destination_directory'


if len(sys.argv) != 2:
  help()
  sys.exit(-1)

if not os.path.exists(sys.argv[1]):
  sys.stderr.write('Destination directory doesn\'t exits\n')
  help()
  sys.exit(-1)

scriptdirectoryfullpath = scriptdirectory()
scriptdirectorylength = len(scriptdirectoryfullpath)

# parse all directories and files of the script directory
for dirname, dirnames, filenames in os.walk(scriptdirectoryfullpath):
  # for each directory
  for subdirname in dirnames:
    # generate the pathes
    directoryfullpath = os.path.join(dirname, subdirname)
    directoryrelativepath = directoryfullpath[scriptdirectorylength+1:]
    destinationfullpath = os.path.join(sys.argv[1], directoryrelativepath)

    # remove exceptions
    if ".git" in directoryrelativepath: continue
    if os.path.basename(sys.argv[0]) in directoryrelativepath: continue

    # create the directory if required
    if not os.path.exists(destinationfullpath):
      os.makedirs(destinationfullpath)

  # parse each file
  for filename in filenames:
    # generate the pathes
    filefullpath = os.path.join(dirname, filename)
    filerelativepath = filefullpath[scriptdirectorylength+1:]
    destinationfullpath = os.path.join(sys.argv[1], filerelativepath)

    # remove exceptions
    if "README" in filerelativepath: continue
    if ".git" in filerelativepath: continue
    if "ChangeLog" in filerelativepath: continue
    if os.path.basename(sys.argv[0]) in filerelativepath: continue

    # copy the file
    shutil.copyfile(filefullpath, destinationfullpath)
