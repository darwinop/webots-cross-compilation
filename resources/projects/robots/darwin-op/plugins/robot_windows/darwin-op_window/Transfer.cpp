#include "SSH.hpp"
#include "Transfer.hpp"
#include "ZIP.hpp"

#include <webots/robot.h>
#include <webots/camera.h>
#include <core/StandardPaths.hpp>

#include <QtConcurrent/QtConcurrent>

#include <cassert>

using namespace webotsQtUtils;

Transfer::Transfer(QWidget *parent): QScrollArea(parent) {
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setMinimumHeight(520);
  mContainerGridLayout = new QGridLayout();
  mContainerGridLayout->setHorizontalSpacing(150);
  mContainerGridLayout->setVerticalSpacing(30);

  //***  SETTINGS  ***//
  mSettingsGridLayout = new QGridLayout();
  mSettingsGroupBox = new QGroupBox(tr("Connection settings"));

  // IP
  mIPAddressLineEdit = new QLineEdit(this);
  mIPAddressLineEdit->setToolTip(tr("IP address or name of the robot, the robot must be on the same network that the computer.\nIf the robot is connected with an Ethernet cable, the default address is 192.168.123.1\nIf the robot is connected by Wifi, run (on the robot) the 'ifconfig' command to display the IP address"));
  mIPLabel = new QLabel(tr("IP address:"));
  mSettingsGridLayout->addWidget(mIPLabel, 0, 0, 1, 1);
  mSettingsGridLayout->addWidget(mIPAddressLineEdit, 0, 1, 1, 1);

  // Username
  mUsernameLineEdit = new QLineEdit(this);
  mUsernameLineEdit->setToolTip(tr("The default username is 'darwin'"));
  mUsernameLabel = new QLabel(tr("Username:"));
  mSettingsGridLayout->addWidget(mUsernameLabel, 1, 0, 1, 1);
  mSettingsGridLayout->addWidget(mUsernameLineEdit, 1, 1, 1, 1);

  // Password
  mPasswordLineEdit = new QLineEdit(this);
  mPasswordLineEdit->setToolTip(tr("The default password is '111111'"));
  mPasswordLabel = new QLabel(tr("Password:"));
  mSettingsGridLayout->addWidget(mPasswordLabel, 2, 0, 1, 1);
  mSettingsGridLayout->addWidget(mPasswordLineEdit, 2, 1, 1, 1);

  // Restore settings button
  mDefaultSettingsButton = new QPushButton(tr("&Restore defaults settings"), this);
  mDefaultSettingsButton->setToolTip(tr("Restore defaults settings for the connection with the real robot"));
  mSettingsGridLayout->addWidget(mDefaultSettingsButton, 3, 0, 1, 2);

  // Load settings
  mSettings = new QSettings("Cyberbotics", "Webots", this);
  loadSettings();

  mSettingsGroupBox->setLayout(mSettingsGridLayout);
  mSettingsGroupBox->setToolTip(tr("Specify the parameters of the connection with the robot"));
  mContainerGridLayout->addWidget(mSettingsGroupBox, 0, 0, 1, 1);

  //***  Upload controller  ***//
  mActionGridLayout = new QGridLayout();
  mActionGroupBox = new QGroupBox(tr("Upload controller"), this);

  // Send Controller
  QString iconPath = StandardPaths::getWebotsHomePath() + QString("resources/projects/robots/darwin-op/plugins/robot_windows/darwin-op_window/images/send.png");
  mSendControllerIcon = new QIcon(QPixmap((char*)iconPath.toStdString().c_str()));
  iconPath = StandardPaths::getWebotsHomePath() + QString("resources/projects/robots/darwin-op/plugins/robot_windows/darwin-op_window/images/stop.png");
  mStopControllerIcon = new QIcon(QPixmap((char*)iconPath.toStdString().c_str()));
  mSendControllerButton = new QPushButton(this);
  mSendControllerButton->setIcon(*mSendControllerIcon);
  mSendControllerButton->setIconSize(QSize(64,64));
  mSendControllerButton->setToolTip(tr("Send the controller curently used in simulation on the real robot and play it"));
  mActionGridLayout->addWidget(mSendControllerButton, 0, 0, 1, 1);

  // Make default controller
  mMakeDefaultControllerCheckBox = new QCheckBox(tr("Make default controller"), this);
  mMakeDefaultControllerCheckBox->setToolTip(tr("Set this controller to be the default controller of the robot"));
  mActionGridLayout->addWidget(mMakeDefaultControllerCheckBox, 1, 0, 1, 2);

  // remote control
  mSSH = NULL;
  mRemoteEnable = false;
  iconPath = StandardPaths::getWebotsHomePath() + QString("resources/projects/robots/darwin-op/plugins/robot_windows/darwin-op_window/images/remote.png");
  mRemoteControlIcon = new QIcon(QPixmap((char*)iconPath.toStdString().c_str()));
  mRemoteControlButton = new QPushButton(this);
  mRemoteControlButton->setIcon(*mRemoteControlIcon);
  mRemoteControlButton->setIconSize(QSize(64,64));
  mRemoteControlButton->setToolTip(tr("Start remote control"));
  mActionGridLayout->addWidget(mRemoteControlButton, 0, 1, 1, 1);

  // Wrapper
  mUninstallButton = new QPushButton(this);
  iconPath = StandardPaths::getWebotsHomePath() + QString("resources/projects/robots/darwin-op/plugins/robot_windows/darwin-op_window/images/uninstall.png");
  mUninstallButton->setIcon(QIcon(QPixmap((char*)iconPath.toStdString().c_str())));
  mUninstallButton->setIconSize(QSize(64,64));
  mUninstallButton->setToolTip(tr("If you don't need it any more, you can uninstall Webots API from the real robot"));
  mActionGridLayout->addWidget(mUninstallButton, 0, 2, 1, 1);

  mActionGroupBox->setLayout(mActionGridLayout);
  mContainerGridLayout->addWidget(mActionGroupBox, 0, 1, 1, 1);

  //***  OUTPUT  ***//

  mOutputGridLayout = new QGridLayout(this);
  mOutputGroupBox = new QGroupBox(tr("DARwIn-OP console"), this);

  // Status label and progress bar
  mStatus = DISCONNECTED;
  mStatusLabel = new QLabel(tr("Status: Disconnected"), this);
  mOutputGridLayout->addWidget(mStatusLabel, 1, 0, 1, 1);
  mProgressBar = new QProgressBar(this);
  mProgressBar->setValue(0);
  mProgressBar->hide();
  mOutputGridLayout->addWidget(mProgressBar, 3, 0, 1, 2);

  // Console Show
  mConsoleShowTextEdit = new QTextEdit(this);
  mConsoleShowTextEdit->setReadOnly(true);
  mConsoleShowTextEdit->setOverwriteMode(false);
  mOutputGridLayout->addWidget(mConsoleShowTextEdit, 0, 0, 1, 2);

  mOutputGroupBox->setLayout(mOutputGridLayout);
  mContainerGridLayout->addWidget(mOutputGroupBox, 1, 0, 1, 2);

  mConnectionState = false;

  mContainerWidget = new QWidget(this);
  mContainerWidget->setLayout(mContainerGridLayout);
  setWidget(mContainerWidget);

  mTimer = new QTimer(this);

  // Signals Transfer->Transfer
  QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(timerCallback()));
  QObject::connect(mSendControllerButton, SIGNAL(clicked()), this, SLOT(sendController()));
  QObject::connect(mRemoteControlButton, SIGNAL(clicked()), this, SLOT(remoteControl()));
  QObject::connect(mUninstallButton, SIGNAL(clicked()), this, SLOT(uninstall()));
  QObject::connect(mDefaultSettingsButton, SIGNAL(clicked()), this, SLOT(restoreSettings()));
  QObject::connect(mMakeDefaultControllerCheckBox, SIGNAL(pressed()), this, SLOT(installControllerWarningSlot()));
  
  mSSH = new SSH(this);
  mSSH->connect(mSSH,SIGNAL(print(const QString &,bool)),this,SLOT(print(const QString &,bool)),Qt::QueuedConnection);
  mSSH->connect(mSSH,SIGNAL(status(const QString &)),this,SLOT(status(const QString &)),Qt::QueuedConnection);
  mSSH->connect(mSSH,SIGNAL(done()),this,SLOT(SSHSessionDone()));
  connect(&mFutureWatcher, SIGNAL(finished()),this, SLOT(SSHSessionComplete()));
}

Transfer::~Transfer() {
  delete mSSH;
}

void Transfer::showProgressBox(const QString &title, const QString &message) {
  disableButtons();
  mRemoteProgressDialog = new QProgressDialog(title, QString(), 0, 100, this, Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
  mRemoteProgressDialog->setWindowTitle(title);
  mRemoteProgressDialog->setLabelText(tr("%1...").arg(message) + "\n\n"+tr("Please wait, it can take a few seconds."));
  mRemoteProgressBar = new QProgressBar(mRemoteProgressDialog);
  mRemoteProgressBar->setTextVisible(false);
  mRemoteProgressBar->setMinimum(0);
  mRemoteProgressBar->setMaximum(100);
  mRemoteProgressDialog->setBar(mRemoteProgressBar);
  mRemoteProgressDialog->show();
  mTimer->start(10);
}

void Transfer::print(const QString &c,bool err) {
  static QString str_out;
  static QString str_err;
  if (err)
    str_err += c;
  else
    str_out += c;
  do {
    int n = str_out.indexOf("\n");
    if (n==-1) break;;
    QString pr = str_out.left(n);
    mConsoleShowTextEdit->setTextColor(QColor( 0, 0, 0));
    mConsoleShowTextEdit->append(pr);
    mConsoleShowTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    str_out = str_out.mid(n+1);
  } while(!str_out.isEmpty());
  do {
    int n = str_err.indexOf("\n");
    if (n==-1) break;;
    QString pr = str_err.left(n);
    mConsoleShowTextEdit->setTextColor(QColor( 255, 0, 0));
    mConsoleShowTextEdit->append(pr);
    mConsoleShowTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    str_err = str_err.mid(n+1);
  } while(!str_err.isEmpty());
}

void Transfer::status(const QString &message) {
  mStatusLabel->setText(tr("Status: %1...").arg(message));
  if (mRemoteProgressDialog)
    mRemoteProgressDialog->setLabelText(tr("%1...").arg(message) + "\n\n"+tr("Please wait, it can take a few seconds."));
}

void Transfer::sendController() {
  QString controller;
  controller = QString(wb_robot_get_controller_name());
  if (mStatus == DISCONNECTED) { // send controller
    mRemoteControlButton->setEnabled(false);
    mUninstallButton->setEnabled(false);
    mSendControllerButton->setEnabled(true);
    mMakeDefaultControllerCheckBox->setEnabled(false);
    showProgressBox(tr("Starting remote compilation..."),tr("Initializing"));
    mFuture = QtConcurrent::run(mSSH,&SSH::startRemoteCompilation,mIPAddressLineEdit->text(),mUsernameLineEdit->text(),mPasswordLineEdit->text(),mMakeDefaultControllerCheckBox->isChecked());
    mFutureWatcher.setFuture(mFuture);
    mStatus = START_REMOTE_COMPILATION;
    mStatusLabel->setText(tr("Status: Starting remote compilation")); 
  } else { // stop controller
    assert(mStatus == RUN_REMOTE_COMPILATION);
    showProgressBox(tr("Stopping remote controller..."),tr("Initializing"));
    mSSH->terminate();
    mStatus = STOP_REMOTE_COMPILATION;
    mStatusLabel->setText(tr("Status: Stopping remote controller")); 
  }
}

void Transfer::remoteControl() {
  if (mStatus==DISCONNECTED) { // start the remote control
    showProgressBox(tr("Starting remote control..."),tr("Initializing"));
    static QString ip = mIPAddressLineEdit->text();
    mFuture = QtConcurrent::run(mSSH,&SSH::startRemoteControl,ip,mUsernameLineEdit->text(),mPasswordLineEdit->text());
    mFutureWatcher.setFuture(mFuture);
    mStatus = START_REMOTE_CONTROL;
  } else if (mStatus==RUN_REMOTE_CONTROL) { // stop the remote control
    showProgressBox(tr("Stopping remote control..."),tr("Initializing"));
    wb_robot_set_mode(WB_MODE_SIMULATION, NULL);
    mSSH->terminate();
    mStatus = STOP_REMOTE_CONTROL;
    mStatusLabel->setText(tr("Status: Stopping remote control")); 
  }
}

void Transfer::uninstall() {
  QMessageBox msgBox;
  msgBox.setWindowTitle(tr("Webots API uninstallation"));
  msgBox.setText(tr("You are going to completely uninstall Webots API from DARwIn-OP"));
  msgBox.setInformativeText(tr("Are you sure this is what do you want to do?"));
  msgBox.setStandardButtons(QMessageBox ::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
  msgBox.setDetailedText(tr("All the Webots API and controllers will be uninstalled, the original demo program will be restored"));
  int warning = msgBox.exec();
  if(warning == QMessageBox::Yes) {
    showProgressBox(tr("Uninstall"),tr("Uninstalling the Webots API"));
    mFuture = QtConcurrent::run(mSSH,&SSH::uninstall,mIPAddressLineEdit->text(),mUsernameLineEdit->text(),mPasswordLineEdit->text());
    mFutureWatcher.setFuture(mFuture);
    mStatus = UNINSTALL;
  }
}

void Transfer::finishStartRemoteCompilation() {
  delete mRemoteProgressDialog;
  mRemoteProgressDialog = NULL;
  mSendControllerButton->setIcon(*mStopControllerIcon);
  mSendControllerButton->setToolTip(tr("Stop the controller on the real robot"));
  mSendControllerButton->setEnabled(true);
  mRemoteControlButton->setEnabled(false);
  mUninstallButton->setEnabled(false);
  mMakeDefaultControllerCheckBox->setEnabled(false);
  wb_robot_set_mode(WB_MODE_SIMULATION, (void *)(mIPAddressLineEdit->text().toStdString().c_str()));
  mStatus = RUN_REMOTE_COMPILATION;
  mStatusLabel->setText(tr("Status: Running remote controller"));
}

void Transfer::finishStartRemoteControl() {
  delete mRemoteProgressDialog;
  mRemoteProgressDialog = NULL;
  mRemoteControlButton->setIcon(*mStopControllerIcon);
  mRemoteControlButton->setToolTip(tr("Stop remote control"));
  mRemoteControlButton->setEnabled(true);
  mUninstallButton->setEnabled(false);
  mSendControllerButton->setEnabled(false);
  mMakeDefaultControllerCheckBox->setEnabled(false);
  wb_robot_set_mode(WB_MODE_REMOTE_CONTROL, (void *)(mIPAddressLineEdit->text().toStdString().c_str()));
  mStatus = RUN_REMOTE_CONTROL;
  mStatusLabel->setText(tr("Status: Running remote control"));
}

void Transfer::finish() {
  delete mRemoteProgressDialog;
  mRemoteProgressDialog = NULL;
  mStatus = DISCONNECTED;
  enableButtons();
}

void Transfer::SSHSessionDone() {
  saveSettings(); // connection was successful, so we want to save the network settings
  switch(mStatus) {
  case START_REMOTE_COMPILATION: finishStartRemoteCompilation(); break;
  case START_REMOTE_CONTROL:     finishStartRemoteControl();     break;
  default: assert(false); break;
  }
}

void Transfer::SSHSessionComplete() {
  if (mFutureWatcher.result()<0) {
    delete mRemoteProgressDialog;
    mRemoteProgressDialog = NULL;
    mStatusLabel->setText(mSSH->error());
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Connection failed"));
    msgBox.setText(tr("Unable to establish connection with DARwIn-OP."));
    msgBox.setInformativeText(mSSH->error());
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox.exec();
    enableButtons();
    mStatusLabel->setText(tr("Status: Disconnected"));
    mStatus = DISCONNECTED;
  } else {
    saveSettings(); // connection was successful, so we want to save the network settings
    switch(mStatus) {
    case STOP_REMOTE_CONTROL:
    case STOP_REMOTE_COMPILATION:
    case UNINSTALL:
    case RUN_REMOTE_COMPILATION: // the remote controller crashed or could not start
    case RUN_REMOTE_CONTROL: // this one should not happen
      finish();
      break;
    case START_REMOTE_COMPILATION: {// the robot was not in a stable position
      finish();
      if (!mSSH->error().isEmpty()) break; // we don't want to display the following message box in case of a compilation error
      QMessageBox msgBox;
      msgBox.setWindowTitle(tr("Unstable position"));
      msgBox.setText(tr("The DARwIn-OP robot is not in ready position."));
      msgBox.setInformativeText(tr("Please set the robot in ready position (refer to DARwIn-OP documentation) and retry."));
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
      msgBox.exec();
      }
      break;
    default: fprintf(stderr,"Unknown status: %d\n",mStatus);       break;
    }
  }
}

// ---------------------------------------------------------------------------- //
// ***                        Auxiliary functions                           *** //
// ---------------------------------------------------------------------------- //

void Transfer::timerCallback() {
  int static i = 5;
  int static direction = 1;

  if(mRemoteProgressDialog != NULL) {
    if(!mRemoteProgressDialog->isVisible()) {
      direction = 5;
      i = 1;
    }
    mRemoteProgressDialog->setValue(i);
    i = i + direction;

    if(i > 95) {
      direction = -1;
      mRemoteProgressBar->setInvertedAppearance(true);
    }
    else if(i < 5) {
      direction = 1;
      mRemoteProgressBar->setInvertedAppearance(false);
    }
  }
}

void Transfer::restoreSettings() {
  mIPAddressLineEdit->setText("192.168.123.1");
  mUsernameLineEdit->setText("darwin");
  mPasswordLineEdit->setText("111111");
}

void Transfer::saveSettings() {
  mSettings->setValue("darwin-op_window/IP", mIPAddressLineEdit->text()); 
  mSettings->setValue("darwin-op_window/username", mUsernameLineEdit->text());
  mSettings->setValue("darwin-op_window/password", mPasswordLineEdit->text());
}

void Transfer::loadSettings() {
  mIPAddressLineEdit->setText(mSettings->value("darwin-op_window/IP", QString("192.168.123.1")).toString());
  mUsernameLineEdit->setText(mSettings->value("darwin-op_window/username", QString("darwin")).toString());
  mPasswordLineEdit->setText(mSettings->value("darwin-op_window/password", QString("111111")).toString());
}

void Transfer::enableButtons() {
  mSendControllerButton->setIcon(*mSendControllerIcon);
  mRemoteControlButton->setIcon(*mRemoteControlIcon);
  mUninstallButton->setEnabled(true);
  mSendControllerButton->setEnabled(true);
  mMakeDefaultControllerCheckBox->setEnabled(true);
  mRemoteControlButton->setEnabled(true);
}

void Transfer::disableButtons() {
  mUninstallButton->setEnabled(false);
  mSendControllerButton->setEnabled(false);
  mMakeDefaultControllerCheckBox->setEnabled(false);
  mRemoteControlButton->setEnabled(false);
}

void Transfer::robotInstableSlot() {
  QString imagePath;
  imagePath = StandardPaths::getWebotsHomePath() + QString("resources/projects/robots/darwin-op/plugins/robot_windows/darwin-op_window/images/start_position.png");
  QMessageBox msgBox;
  msgBox.setWindowTitle(tr("Stability check"));
  msgBox.setText("The robot doesn't seems to be in a stable position.");
  msgBox.setInformativeText("What do you want to do?");
  msgBox.setDetailedText("Warning, the robot doesn't seems to be in it's stable start-up position.\nWe recommand you to put the robot in the stable position and to retry.\nThe stable position is when the robot is sit down (illustration above).\nNevertheless if you want to start the controller in this position you can press Ignore, but be aware that the robot can make sudden movements to reach its start position and this can damage it!");
  msgBox.setIconPixmap(QPixmap((char*)imagePath.toStdString().c_str()));
  msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Abort | QMessageBox::Ignore);
  msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
  msgBox.setDefaultButton(QMessageBox::Retry);
  int warning = msgBox.exec();
  if(warning == QMessageBox::Abort)
    emit setStabilityResponseSignal(false);
  else if(warning == QMessageBox::Ignore)
    emit setStabilityResponseSignal(true);
  else
    emit isRobotStableSignal();
}

void Transfer::installControllerWarningSlot() {
  if(!mMakeDefaultControllerCheckBox->isChecked()) {
#ifdef WIN32
    mMakeDefaultControllerCheckBox->setChecked(true);
#endif
    QMessageBox msgBox;
    msgBox.setWindowTitle("Controller installation");
    msgBox.setText("Installation of controller on the DARwIn-OP                     ");
    msgBox.setInformativeText("The controller will start automaticaly when the robot starts.\nWarning the position of the robot will not be checked, so make sure that the robot is allways in its stable position before to start it.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox.exec();
  }
}
