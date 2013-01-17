/*
 * File:         Transfer.hpp
 * Date:         January 2013
 * Description:  Widget for communicating with the robot DARwIn-OP
 * Author:       david.mansolino@epfl.ch
 * Modifications:
 */

#ifndef TRANSFER_HPP
#define TRANSFER_HPP

#include <QtGui/QtGui>
#include <QtCore/QtCore>

#include <pthread.h>
#include "SSH.hpp"

class Transfer : public QScrollArea, public SSH
{

  Q_OBJECT
	
public:
  Transfer();
  virtual ~Transfer();

public slots:
  void sendController();
  void uninstall();
  void restoreSettings();
  void saveSettings();
  void updateProgressSlot(int);
  void addToConsoleSlot(QString);
  void addToConsoleRedSlot(QString);
  void robotInstableSlot();
  void installControllerWarningSlot();
  void ActiveButtonsSlot();
  void UnactiveButtonsSlot();
  
signals:
  void updateProgressSignal(int);
  void addToConsoleSignal(QString);
  void addToConsoleRedSignal(QString);
  void robotInstableSignal();
  void ActiveButtonsSignal();
  void UnactiveButtonsSignal();

private:
  //***  SSH  ***//
  bool         mConnectionState;

  QWidget     *mContainerWidget;
  QGridLayout *mContainerGridLayout;
  
  //***  SETTINGS  ***//
  QGridLayout *mSettingsGridLayout;
  QGroupBox   *mSettingsGroupBox;

  // IP adresse
  QLineEdit   *mIPLineEdit;
  QLabel      *mIPLabel;
  
  // Username
  QLineEdit   *mUsernameLineEdit;
  QLabel      *mUsernameLabel;
  
  // Password
  QLineEdit   *mPasswordLineEdit;
  QLabel      *mPasswordLabel;
  
  // Restore
  QPushButton *mDefaultSettingsButton;
  QSettings   *mSettings;
  
  //***  Upload controller  ***//
  QGridLayout *mActionGridLayout;
  QGroupBox   *mActionGroupBox;

  // Controller
  QIcon       *mSendControllerIcon;
  QIcon       *mStopControllerIcon;
  QPushButton *mSendControllerButton;
  QCheckBox   *mMakeDefaultControllerCheckBox;
  pthread_t   *mControllerThread;
  int          mStabilityResponse;

  // Wrapper
  QPushButton *mUninstallButton;
  QFile       *mWrapperVersionFile;
  QFile       *mFrameworkVersionFile;
  
  //***  OUTPUT  ***//
  QGridLayout *mOutputGridLayout;
  QGroupBox   *mOutputGroupBox;
  
  // Status label and progress bar
  QLabel      *mStatusLabel;
  QProgressBar *mProgressBar;

  // Console Show
  QTextEdit   *mConsoleShowTextEdit;
  
  int          installAPI();
  int          updateFramework();
  int          StartSSH();
  void         loadSettings();
  void         ShowOutputSSHCommand();
  bool         isRobotStable();
  bool         isWrapperUpToDate();
  bool         isFrameworkUpToDate();
  static void *thread_uninstall(void *param);
  static void *thread_controller(void *param);
    
};

#endif
