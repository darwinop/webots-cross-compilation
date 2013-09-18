#include "ZIP.hpp"

#include <QtCore/QtCore>

#include <zip.h>

#include <stdlib.h>
#include <stdio.h>

ZIP::ZIP() {
}

ZIP::~ZIP() {
}

bool ZIP::AddFile(struct zip *archive, QString file, QString name) {
  struct zip_source *s;
  if ((s = zip_source_file(archive, (const char *)file.toStdString().c_str(), 0, 0)) == NULL || zip_add(archive, (const char *) name.toStdString().c_str(), s) < 0) {
    zip_source_free(s);
    return false;
  }
  return true;
}

bool ZIP::AddFolder(struct zip *archive, QString folder, QString name, bool recursive) {
  QStringList fileList;
  QStringList directoryList;

  // Add all files in this directory
  fileList = QDir(folder).entryList(QDir::Files);
  for (int i = 0; i < fileList.size(); i++) {
    if (!(AddFile(archive, (folder + QString("/") + fileList.at(i)), (name + QString("/") + fileList.at(i))))) {
      printf("Error while adding file %s to archive\n", (char *)fileList.at(i).toStdString().c_str());
      return false;
    }
  }

  // Add all sub-directories
  if (recursive) {
    directoryList = QDir(folder).entryList(QDir::AllDirs);
    for (int i = 2; i < directoryList.size(); i++) {
      if ((directoryList.at(i) != ".") && (directoryList.at(i) != "..")) {
        if (!(AddFolder(archive, (folder + QString("/") + directoryList.at(i)), (name + QString("/") + directoryList.at(i))))) {
          printf("Error while directory %s to archive\n", (char *)directoryList.at(i).toStdString().c_str());
          return false;
        }
      }
    }
  }

  return true;
}

bool ZIP::CompressFolder(QString archiveName, QString folder, bool recursive, const char * firstFolder) {
  int archiveError = 0;
  struct zip * archive = zip_open((const char *)archiveName.toStdString().c_str(), ZIP_CREATE, &archiveError);

  if (archiveError != 0) {
    printf("Error while creating archive\n");
    return false;
  }
  else {
    if (!(AddFolder(archive, folder, firstFolder, recursive)))
      return false;
    if (zip_close(archive) == -1) {
      printf("Error while closing archive\n");
      return false;
    }
    return true;
  }
}

bool ZIP::AddFolderToArchive(QString archiveName, QString folder, bool recursive, const char * firstFolder) {
  int archiveError = 0;
  struct zip * archive = zip_open((const char *)archiveName.toStdString().c_str(), ZIP_CREATE, &archiveError);

  if (archiveError != 0) {
    printf("Error while openning archive\n");
    return false;
  }
  else {
    if (!(AddFolder(archive, folder, firstFolder, recursive)))
      return false;
    if (zip_close(archive) == -1) {
      printf("Error while closing archive\n");
      return false;
    }
    return true;
  }
}

bool ZIP::AddFileToArchive(QString archiveName, QString file, const char * nameInArchive) {
  int archiveError = 0;
  struct zip * archive = zip_open((const char *)archiveName.toStdString().c_str(), ZIP_CREATE, &archiveError);

  if (archiveError != 0) {
    printf("Error while openning archive\n");
    return false;
  }
  else {
    if (!(AddFile(archive, file, QString(nameInArchive))))
      return false;
    if (zip_close(archive) == -1) {
      printf("Error while closing archive\n");
      return false;
    }
    return true;
  }
}
