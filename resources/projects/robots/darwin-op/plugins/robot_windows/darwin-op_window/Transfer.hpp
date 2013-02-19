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
  void updateStatusSlot(QString);
  void robotInstableSlot();
  void installControllerWarningSlot();
  void ActiveButtonsSlot();
  void UnactiveButtonsSlot();
  void startRemoteControl();
  void activateRemoteControlSlot();
  void remoteCameraWarningSlot();
  void waitRemoteSlot();
  void endWaitRemotSlot();
  void RemoteCanceledSlot();
  void resetRemoteButtonSlot();
  void resetControllerButtonSlot();
  
signals:
  void updateProgressSignal(int);
  void addToConsoleSignal(QString);
  void addToConsoleRedSignal(QString);
  void updateStatusSignal(QString);
  void robotInstableSignal();
  void ActiveButtonsSignal();
  void UnactiveButtonsSignal();
  void activateRemoteControlSignal();
  void remoteCameraWarningSignal();
  void endWaitRemotSignal();
  void resetRemoteButtonSignal();
  void resetControllerButtonSignal();

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
  QLineEdit   *mIPLineEdit1;
  QLineEdit   *mIPLineEdit2;
  QLineEdit   *mIPLineEdit3;
  QLineEdit   *mIPLineEdit4;
  QHBoxLayout *mIPHBoxLayout;
  QLabel      *mIPLabel;
  
  QIntValidator *mIPVAlidator;
  
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
  pthread_t   *mThread;
  int          mStabilityResponse;
  
  // remote control
  QIcon       *mRemoteControlIcon;
  QPushButton *mRemoteControlButton;
  bool         mRemoteEnable;
  
  // Remote control wait during starting
  QProgressDialog *mRemoteProgressDialog;
  QProgressBar    *mRemoteProgressBar;
  QTimer          *mRemoteTime;
  int              mRemoteStartingTime;
  int              mRemoteStartingTimeCounter;

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
  int          killProcessIfRunning(Transfer * instance, QString process);
  void         loadSettings();
  void         ShowOutputSSHCommand();
  bool         isRobotStable();
  bool         isWrapperUpToDate();
  bool         isFrameworkUpToDate();
  static void  wait(int duration);
  static void *thread_uninstall(void *param);
  static void *thread_controller(void *param);
  static void *thread_remote(void *param);
    
};

#endif
