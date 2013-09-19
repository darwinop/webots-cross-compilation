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
#include <QtWidgets/QtWidgets>

#include <pthread.h>

class SSH;
struct Parameters;

class Transfer : public QScrollArea
{
  Q_OBJECT

  public:
    enum Status {
      DISCONNECTED,
      START_REMOTE_CONTROL,
      RUN_REMOTE_CONTROL,
      STOP_REMOTE_CONTROL,
      UNINSTALL,
      START_REMOTE_COMPILATION,
      RUN_REMOTE_COMPILATION,
      STOP_REMOTE_COMPILATION
    };

                        Transfer(QWidget *parent = 0);
    virtual            ~Transfer();

    // void                robotInstableSlot(); // TODO: this function is never call: when has it been unused?

  public slots:
    void                installControllerWarningSlot();
    void                restoreSettings();
    void                uninstall();
    void                sendController();
    void                remoteControl();
    void                timerCallback();
    void                SSHSessionComplete();
    void                SSHSessionDone();
    void                print(const QString &message, bool error);
    void                status(const QString &message);

  signals:
    void                setStabilityResponseSignal(int stability);
    void                isRobotStableSignal();
    void                stopRemoteSignal();
    void                stopControllerSignal();
    void                testSignal();

  private:
    void                saveSettings();
    void                enableButtons();
    void                disableButtons();

    void                showProgressBox(const QString &title,const QString &message);
    void                finishStartRemoteCompilation();
    void                finishStartRemoteControl();
    void                finish();
    void                loadSettings();

    QFutureWatcher<int> mFutureWatcher;
    QFuture<int>        mFuture;

    Status              mStatus;

    //***  SSH  ***//
    SSH                *mSSH;
    bool                mConnectionState;

    QWidget            *mContainerWidget;
    QGridLayout        *mContainerGridLayout;

    //***  SETTINGS  ***//
    QGridLayout        *mSettingsGridLayout;
    QGroupBox          *mSettingsGroupBox;

    // IP adresse
    QLineEdit          *mIPAddressLineEdit;
    QLabel             *mIPLabel;

    // Username
    QLineEdit          *mUsernameLineEdit;
    QLabel             *mUsernameLabel;

    // Password
    QLineEdit          *mPasswordLineEdit;
    QLabel             *mPasswordLabel;

    // Restore
    QPushButton        *mDefaultSettingsButton;
    QSettings          *mSettings;

    //***  Upload controller  ***//
    QGridLayout        *mActionGridLayout;
    QGroupBox          *mActionGroupBox;

    // Controller
    QIcon              *mSendControllerIcon;
    QIcon              *mStopControllerIcon;
    QPushButton        *mSendControllerButton;
    QCheckBox          *mMakeDefaultControllerCheckBox;

    // remote control
    QIcon              *mRemoteControlIcon;
    QPushButton        *mRemoteControlButton;
    bool                mRemoteEnable;

    // Remote control wait during starting
    QProgressDialog    *mRemoteProgressDialog;
    QProgressBar       *mRemoteProgressBar;
    QTimer             *mTimer;

    // Wrapper
    QPushButton        *mUninstallButton;

    //***  OUTPUT  ***//
    QGridLayout        *mOutputGridLayout;
    QGroupBox          *mOutputGroupBox;

    // Status label and progress bar
    QLabel             *mStatusLabel;
    QProgressBar       *mProgressBar;

    // Console Show
    QTextEdit          *mConsoleShowTextEdit;
};

#endif
