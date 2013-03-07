#include "Transfer.hpp"
#include <core/StandardPaths.hpp>

#include <QtGui/QtGui>
#include <QtCore/QtCore>

#include <stdlib.h>
#include <stdio.h> 
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <webots/robot.h>
#include <webots/camera.h>

#include "ZIP.hpp"

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

using namespace webotsQtUtils;

Transfer::Transfer(QWidget *parent):
  QScrollArea(parent)
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setMinimumHeight(520);
  mThread = new pthread_t;
  mContainerGridLayout = new QGridLayout;
  mContainerGridLayout->setHorizontalSpacing(150);
  mContainerGridLayout->setVerticalSpacing(30);
  
  //***  SETTINGS  ***//
  mSettingsGridLayout = new QGridLayout;
  mSettingsGroupBox = new QGroupBox("Connection settings");

  // IP
  mIPLineEdit = new QLineEdit;
  mIPLineEdit->setToolTip("IP adress of the robot, the robot must be on the same network that the computer.\nIf the robot is connected with an ethernet cable, the default adress is 192.168.123.1\nIf the robot is connected by wifi, run (on the robot) the command 'ifconfig' to find the IP adress.");
  mIPLabel = new QLabel("IP adresse : ");
  mSettingsGridLayout->addWidget(mIPLabel, 0, 0, 1, 1);
  mSettingsGridLayout->addWidget(mIPLineEdit, 0, 1, 1, 1);
  
  // Username
  mUsernameLineEdit = new QLineEdit;
  mUsernameLineEdit->setToolTip("If it has not been changed the default username is 'darwin'");
  mUsernameLabel = new QLabel("Username : ");
  mSettingsGridLayout->addWidget(mUsernameLabel, 1, 0, 1, 1);
  mSettingsGridLayout->addWidget(mUsernameLineEdit, 1, 1, 1, 1);
  
  // Password
  mPasswordLineEdit = new QLineEdit;
  mPasswordLineEdit->setToolTip("If it has not been changed the default password is '111111'");
  mPasswordLabel = new QLabel("Password : ");
  mSettingsGridLayout->addWidget(mPasswordLabel, 2, 0, 1, 1);
  mSettingsGridLayout->addWidget(mPasswordLineEdit, 2, 1, 1, 1);
  
  // Restore settings button
  mDefaultSettingsButton = new QPushButton("&Restore defaults settings");
  mDefaultSettingsButton->setToolTip("Restore defaults settings for the connection with the real robot.");
  mSettingsGridLayout->addWidget(mDefaultSettingsButton, 3, 0, 1, 2);
  
  // Load settings
  mSettings = new QSettings("Cyberbotics", "Webots");
  loadSettings();
  
  mSettingsGroupBox->setLayout(mSettingsGridLayout);
  mSettingsGroupBox->setToolTip("Specify the parameters of the connection with the robot");
  mSettingsGroupBox->setObjectName("borderedGroupBox");
  mContainerGridLayout->addWidget(mSettingsGroupBox, 0, 0, 1, 1);
  
  //***  Upload controller  ***//
  mActionGridLayout = new QGridLayout;
  mActionGroupBox = new QGroupBox("Upload controller");
  
  // Send Controller
  QString iconPath = StandardPaths::getWebotsHomePath() + QString("resources/projects/robots/darwin-op/plugins/robot_windows/darwin-op_window/images/send.png");
  mSendControllerIcon = new QIcon(QPixmap((char*)iconPath.toStdString().c_str()));
  iconPath = StandardPaths::getWebotsHomePath() + QString("resources/projects/robots/darwin-op/plugins/robot_windows/darwin-op_window/images/stop.png");
  mStopControllerIcon = new QIcon(QPixmap((char*)iconPath.toStdString().c_str()));
  mSendControllerButton = new QPushButton;
  mSendControllerButton->setIcon(*mSendControllerIcon);
  mSendControllerButton->setIconSize(QSize(64,64));
  mSendControllerButton->setToolTip("Send the controller curently used in simulation on the real robot and play it.");
  mActionGridLayout->addWidget(mSendControllerButton, 0, 0, 1, 1);

  // Make default controller
  mMakeDefaultControllerCheckBox = new QCheckBox("Make default controller        ");
  mMakeDefaultControllerCheckBox->setToolTip("Set this controller to be the default controller of the robot.");
  mActionGridLayout->addWidget(mMakeDefaultControllerCheckBox, 1, 0, 1, 2);

  // remote control
  mRemoteEnable = false;
  iconPath = StandardPaths::getWebotsHomePath() + QString("resources/projects/robots/darwin-op/plugins/robot_windows/darwin-op_window/images/remote.png");
  mRemoteControlIcon = new QIcon(QPixmap((char*)iconPath.toStdString().c_str()));
  mRemoteControlButton = new QPushButton;
  mRemoteControlButton->setIcon(*mRemoteControlIcon);
  mRemoteControlButton->setIconSize(QSize(64,64));
  mRemoteControlButton->setToolTip("Start remote control.");
  mActionGridLayout->addWidget(mRemoteControlButton, 0, 1, 1, 1);
  
  // Wrapper
  mWrapperVersionFile = new QFile(QString(QProcessEnvironment::systemEnvironment().value("WEBOTS_HOME")) + QString("/resources/projects/robots/darwin-op/config/version.txt"));
  mFrameworkVersionFile = new QFile(QString(QProcessEnvironment::systemEnvironment().value("WEBOTS_HOME")) + QString("/resources/projects/robots/darwin-op/libraries/darwin/darwin/version.txt"));
  mUninstallButton = new QPushButton;
  iconPath = StandardPaths::getWebotsHomePath() + QString("resources/projects/robots/darwin-op/plugins/robot_windows/darwin-op_window/images/uninstall.png");
  mUninstallButton->setIcon(QIcon(QPixmap((char*)iconPath.toStdString().c_str())));
  mUninstallButton->setIconSize(QSize(64,64));
  mUninstallButton->setToolTip("If you do not want to use it, you can uninstall Webots API from the real robot.");
  mActionGridLayout->addWidget(mUninstallButton, 0, 2, 1, 1);

  mActionGroupBox->setLayout(mActionGridLayout);
  mActionGroupBox->setObjectName("borderedGroupBox");
  mContainerGridLayout->addWidget(mActionGroupBox, 0, 1, 1, 1);
  
  //***  OUTPUT  ***//
  
  mOutputGridLayout = new QGridLayout;
  mOutputGroupBox = new QGroupBox("DARwIn-OP console");

  // Status label and progress bar
  mStatusLabel = new QLabel("Status : Disconnected");
  mStatusLabel->setAlignment(Qt::AlignHCenter);
  mOutputGridLayout->addWidget(mStatusLabel, 1, 0, 1, 1);
  mProgressBar = new QProgressBar();
  mProgressBar->setValue(0);
  mProgressBar->hide();
  mOutputGridLayout->addWidget(mProgressBar, 3, 0, 1, 2);
  
  // Console Show
  mConsoleShowTextEdit = new QTextEdit;
  mConsoleShowTextEdit->setReadOnly(true);
  mConsoleShowTextEdit->setOverwriteMode(false);
  mOutputGridLayout->addWidget(mConsoleShowTextEdit, 0, 0, 1, 2);
  
  mOutputGroupBox->setLayout(mOutputGridLayout);
  mOutputGroupBox->setObjectName("borderedGroupBox");
  mContainerGridLayout->addWidget(mOutputGroupBox, 1, 0, 1, 2);

  mConnectionState = false;

  mContainerWidget = new QWidget;
  mContainerWidget->setLayout(mContainerGridLayout);
  setWidget(mContainerWidget);
  
  mRemoteTime = new QTimer(this);
  mRemoteStartingTime = mSettings->value("darwin-op_window/remote_starting_time", QString::number(-1)).toInt();

  QObject::connect(mRemoteTime, SIGNAL(timeout()), this, SLOT(waitRemoteSlot()));
  QObject::connect(mSendControllerButton, SIGNAL(clicked()), this, SLOT(sendController()));
  QObject::connect(mRemoteControlButton, SIGNAL(clicked()), this, SLOT(startRemoteControl()));
  QObject::connect(mUninstallButton, SIGNAL(clicked()), this, SLOT(uninstall()));
  QObject::connect(mDefaultSettingsButton, SIGNAL(clicked()), this, SLOT(restoreSettings()));
  QObject::connect(this, SIGNAL(updateProgressSignal(int)), this, SLOT(updateProgressSlot(int)));
  QObject::connect(this, SIGNAL(addToConsoleSignal(QString)), this, SLOT(addToConsoleSlot(QString)));
  QObject::connect(this, SIGNAL(addToConsoleRedSignal(QString)), this, SLOT(addToConsoleRedSlot(QString)));
  QObject::connect(this, SIGNAL(robotInstableSignal()), this, SLOT(robotInstableSlot()));
  QObject::connect(this, SIGNAL(UnactiveButtonsSignal()), this, SLOT(UnactiveButtonsSlot()));
  QObject::connect(this, SIGNAL(ActiveButtonsSignal()), this, SLOT(ActiveButtonsSlot()));
  QObject::connect(mMakeDefaultControllerCheckBox, SIGNAL(pressed()), this, SLOT(installControllerWarningSlot()));
  QObject::connect(this, SIGNAL(activateRemoteControlSignal()), this, SLOT(activateRemoteControlSlot()));
  QObject::connect(this, SIGNAL(remoteCameraWarningSignal()), this, SLOT(remoteCameraWarningSlot()));
  QObject::connect(this, SIGNAL(endWaitRemotSignal()), this, SLOT(endWaitRemotSlot()));
  QObject::connect(this, SIGNAL(resetRemoteButtonSignal()), this, SLOT(resetRemoteButtonSlot()));
  QObject::connect(this, SIGNAL(resetControllerButtonSignal()), this, SLOT(resetControllerButtonSlot()));
  QObject::connect(this, SIGNAL(updateStatusSignal(QString)), this, SLOT(updateStatusSlot(QString)));
  QObject::connect(this, SIGNAL(SendControllerButtonSignal()), this, SLOT(SendControllerButtonSlot()));
}

Transfer::~Transfer() {
  free(mThread);
}

// ----------------------------------------------------------------- //
// *** Remote-Control function and corresponding thread function *** //
// ----------------------------------------------------------------- //

void Transfer::startRemoteControl() {
  if(mRemoteEnable == false) {
	emit UnactiveButtonsSignal();
	mRemoteEnable = true;
    mRemoteControlButton->setIcon(*mStopControllerIcon);
    mRemoteControlButton->setToolTip("Stop remote control.");

    mRemoteProgressDialog = new QProgressDialog("Copying files...", "Cancel", 0, 100, this);
    QObject::connect(mRemoteProgressDialog, SIGNAL(canceled()), this, SLOT(RemoteCanceledSlot()));
    mRemoteProgressDialog->setWindowModality(Qt::WindowModal);
    mRemoteProgressDialog->setWindowTitle("Remote control");
    mRemoteProgressDialog->setLabelText("Starting remot-control.\nPlease wait, it can take a few seconds.");
    mRemoteProgressBar = new QProgressBar(mRemoteProgressDialog);
    mRemoteProgressBar->setTextVisible(false);
    mRemoteProgressBar->setMinimum(0);
    mRemoteProgressBar->setMaximum(100);
    mRemoteProgressDialog->setBar(mRemoteProgressBar);
    mRemoteProgressDialog->show();
    mRemoteTime->start(10);
    
    pthread_create(mThread, NULL, this->thread_remote, this);
  }
  else {
    UnactiveButtonsSlot();
    pthread_cancel(*mThread);
    wb_robot_set_mode(WB_MODE_SIMULATION, NULL);
    mStatusLabel->setText(QString("Status : Disconnected"));
    ExecuteSSHCommand("killall remote_control");
    emit resetRemoteButtonSignal();
    emit ActiveButtonsSignal();
    // Clear SSH
    CloseAllSSH();
  }
}

void * Transfer::thread_remote(void *param) {
  Transfer * instance = ((Transfer*)param);
  emit instance->UnactiveButtonsSignal();
  
  // Create SSH session and channel
  emit instance->updateStatusSignal("Status : Connection to the robot");
  if(instance->StartSSH() < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->endWaitRemotSignal();
    emit instance->resetRemoteButtonSignal();
    instance->CloseAllSSH();
    return NULL;
  }
  
  // Create SFTP Channel
  if(instance->OpenSFTPChannel() < 0) {
	emit instance->updateStatusSignal(QString("Status : ") + QString(instance->mSSHError));
    emit instance->ActiveButtonsSignal();
    emit instance->endWaitRemotSignal();
    emit instance->resetRemoteButtonSignal();
    instance->CloseAllSSH();
    return NULL;
  }
  
  emit instance->updateStatusSignal("Status : Stopping current controller");
  // kill demo process (if any)
  if(instance->killProcessIfRunning(instance, QString("demo")) < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->endWaitRemotSignal();
    emit instance->resetRemoteButtonSignal();
    instance->CloseAllSSH();
    return NULL;
  }
  // kill default process (if any)
  if(instance->killProcessIfRunning(instance, QString("default")) < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->endWaitRemotSignal();
    emit instance->resetRemoteButtonSignal();
    instance->CloseAllSSH();
    return NULL;
  }
  // kill controller process (if any)
  if(instance->killProcessIfRunning(instance, QString("controller")) < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->endWaitRemotSignal();
    emit instance->resetRemoteButtonSignal();
    instance->CloseAllSSH();
    return NULL;
  }
  // kill any older remote_control process
  instance->ExecuteSSHCommand("killall remote_control");
  
  // Verfify Framework version and update it if needed
  if(instance->isFrameworkUpToDate())
    emit instance->updateStatusSignal("Status : Framework up-to-date");
  else if(instance->updateFramework() < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->endWaitRemotSignal();
    emit instance->resetRemoteButtonSignal();
    instance->CloseAllSSH();
    return NULL;
  }
  	
  // Verfify wrapper version and update it if needed
  if(instance->isWrapperUpToDate())
    emit instance->updateStatusSignal("Status : Webots API up-to-date");
  else if(instance->installAPI() < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->endWaitRemotSignal();
    emit instance->resetRemoteButtonSignal();
    instance->CloseAllSSH();
    return NULL;
  }
  
  
  int cameraWidth = wb_camera_get_width(wb_robot_get_device("Camera"));
  int cameraHeight = wb_camera_get_height(wb_robot_get_device("Camera"));
  
  if((cameraWidth != 320) && (cameraWidth != 160) && (cameraWidth != 80) && (cameraWidth != 40) && (cameraWidth != 20)) {
    printf("camera width not supported!\n");
    emit instance->ActiveButtonsSignal();
    emit instance->remoteCameraWarningSignal();
    emit instance->endWaitRemotSignal();
    emit instance->resetRemoteButtonSignal();
    instance->CloseAllSSH();
    return NULL;
  }
    
  if((cameraHeight != 240) && (cameraHeight != 120) && (cameraHeight != 80) && (cameraHeight != 60) && (cameraHeight != 40) && (cameraHeight != 30)) {
    printf("camera height not supported!\n");
    emit instance->ActiveButtonsSignal();
    emit instance->remoteCameraWarningSignal();
    emit instance->endWaitRemotSignal();
    emit instance->resetRemoteButtonSignal();
    instance->CloseAllSSH();
    return NULL;
  }
  
  QString command;
  command = QString("/darwin/Linux/project/webots/remote_control/remote_control ") + QString::number(320/cameraWidth) + QString(" ") + QString::number(240/cameraHeight);
  emit instance->updateStatusSignal("Status : Starting remote control");
  
  if(instance->ExecuteSSHCommand((char*)(command.toStdString().c_str())) < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->endWaitRemotSignal();
    emit instance->resetRemoteButtonSignal();
    instance->CloseAllSSH();
    return NULL; 
  }
  wait(2000); // wait robot initialisation (same delay as in constructor of Robot)
  
  
  emit instance->endWaitRemotSignal();
  emit instance->activateRemoteControlSignal();
  emit instance->ActiveButtonsSignal();

  // Show output
#ifndef WIN32  // because otherwise stopping the thread while using the ssh connection cause problem on Windows
  instance->ShowOutputSSHCommand();
#endif

  return NULL;
}

// -------------------------------------------------------------------- //
// *** Cross-Compilation function and corresponding thread function *** //
// -------------------------------------------------------------------- //

void Transfer::sendController() {

  QString controller;
  controller = QString(wb_robot_get_controller_name());

  if(mConnectionState == false) {
    pthread_create(mThread, NULL, this->thread_controller, this);
  } 
  else {
    // Stop Thread
    pthread_cancel(*mThread);
	  
	// Stop controller and clear 'controllers' directory
	QString killallController = QString("killall controller");
    ExecuteSSHCommand((char*)killallController.toStdString().c_str());
    WaitEndSSHCommand();
    ExecuteSSHCommand("rm -r /darwin/Linux/project/webots/controllers");
    WaitEndSSHCommand();
    ExecuteSSHCommand("mkdir /darwin/Linux/project/webots/controllers");
    WaitEndSSHCommand();
    
    // Clear SSH
    CloseAllSSH();
    
    // Update Status
    mStatusLabel->setText(QString("Status : Disconnected"));
    emit resetControllerButtonSignal();
    emit ActiveButtonsSignal();
  }
}

void * Transfer::thread_controller(void *param) {
  Transfer * instance = ((Transfer*)param);
  emit instance->UnactiveButtonsSignal();
  emit instance->updateProgressSignal(0);
  
  // Create SSH session and channel
  emit instance->updateStatusSignal("Status : Connection in progress (1/7)");
  if(instance->StartSSH() < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->resetControllerButtonSignal();
    instance->CloseAllSSH();
    emit instance->updateProgressSignal(100);
    return NULL;
  }
  emit instance->updateProgressSignal(2);
  
  // Create SFTP Channel
  if(instance->OpenSFTPChannel() < 0) {
	emit instance->updateStatusSignal(QString("Status : ") + QString(instance->mSSHError));
    emit instance->ActiveButtonsSignal();
    emit instance->resetControllerButtonSignal();
    instance->CloseAllSSH();
    emit instance->updateProgressSignal(100);
    return NULL;
  }
  	
  // Verfify Framework version and update it if needed
  if(instance->isFrameworkUpToDate())
    emit instance->updateStatusSignal("Status : Framework up-to-date");
  else if(instance->updateFramework() < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->resetControllerButtonSignal();
    instance->CloseAllSSH();
    emit instance->updateProgressSignal(100);
    return NULL;
  }
  	
  // Verfify wrapper version and update it if needed
  if(instance->isWrapperUpToDate())
    emit instance->updateStatusSignal("Status : Webots API up-to-date");
  else if(instance->installAPI() < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->resetControllerButtonSignal();
    instance->CloseAllSSH();
    emit instance->updateProgressSignal(100);
    return NULL;
  }

  // Creat archive
  QString controller, controllerArchive;
  controller = QString(wb_robot_get_controller_name());
  controllerArchive = QDir::tempPath() + QString("/webots_darwin_") + QString::number((int)QCoreApplication::applicationPid()) + QString("_controller.zip");
  ZIP::CompressFolder(controllerArchive, QString(wb_robot_get_project_path()) + QString("/controllers/") + controller, true, (const char *)controller.toStdString().c_str());
  
  emit instance->updateProgressSignal(5);
  
  emit instance->updateStatusSignal("Status : Stopping current controller (3/7)");
  // kill demo process (if any)
  if(instance->killProcessIfRunning(instance, QString("demo")) < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->resetControllerButtonSignal();
    instance->CloseAllSSH();
    emit instance->updateProgressSignal(100);
    return NULL;
  }
  emit instance->updateProgressSignal(15);
  // kill default process (if any)
  if(instance->killProcessIfRunning(instance, QString("default")) < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->resetControllerButtonSignal();
    instance->CloseAllSSH();
    emit instance->updateProgressSignal(100);
    return NULL;
  }
  emit instance->updateProgressSignal(25);
  // kill controller process (if any)
  if(instance->killProcessIfRunning(instance, QString("controller")) < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->resetControllerButtonSignal();
    instance->CloseAllSSH();
    emit instance->updateProgressSignal(100);
    return NULL;
  }
  emit instance->updateProgressSignal(30);
  // kill remote_control process (if any)
  if(instance->killProcessIfRunning(instance, QString("remote_control")) < 0) {
    emit instance->ActiveButtonsSignal();
    emit instance->resetControllerButtonSignal();
    instance->CloseAllSSH();
    return NULL;
  }
  emit instance->updateProgressSignal(35);
    
  // Clean directory controllers
  instance->ExecuteSSHCommand("rm -r /darwin/Linux/project/webots/controllers");
  instance->WaitEndSSHCommand();
  instance->ExecuteSSHCommand("mkdir /darwin/Linux/project/webots/controllers");
  instance->WaitEndSSHCommand();
	  
  // Send archive file
  emit instance->updateStatusSignal("Status : Sending files to the robot (4/7)"); 
  if(instance->SendFile((char*)controllerArchive.toStdString().c_str(), "/darwin/Linux/project/webots/controllers/controller.zip") < 0) {
    emit instance->updateStatusSignal(QString("Status : ") + QString(instance->mSSHError));
    emit instance->ActiveButtonsSignal();
    emit instance->resetControllerButtonSignal();
    instance->CloseAllSSH();
    emit instance->updateStatusSignal(".\nMaybe Webots API is not installed.");
    // Delete local archive
    QFile deleteArchive(controllerArchive);
    if(deleteArchive.exists())
      deleteArchive.remove();
    emit instance->updateProgressSignal(100);
    return NULL;
  }
  emit instance->updateProgressSignal(50);
    
  // Close SFTP channel
  instance->CloseSFTPChannel();

  instance->ExecuteSSHCommand("chmod 755 /darwin/Linux/project/webots/controllers/controller.zip");
  instance->WaitEndSSHCommand();

  // Delete local archive
  QFile deleteArchive(controllerArchive);
  if(deleteArchive.exists())
    deleteArchive.remove();
  emit instance->updateProgressSignal(55);

  // Decompress remote controller files
  emit instance->updateStatusSignal("Status : Decompressing files (5/7)"); 
  instance->ExecuteSSHCommand("unzip /darwin/Linux/project/webots/controllers/controller.zip -d /darwin/Linux/project/webots/controllers");
  emit instance->updateProgressSignal(60);
  instance->WaitEndSSHCommand();
    
  // Deleting archive
  instance->ExecuteSSHCommand("rm /darwin/Linux/project/webots/controllers/controller.zip");
  emit instance->updateProgressSignal(70);
    
  // Compile controller
  emit instance->addToConsoleSignal(QString("\n------ Compiling controller ------\n"));
  emit instance->updateStatusSignal("Status : Compiling controller (6/7)"); 
  QString makeClean = QString("make -C /darwin/Linux/project/webots/controllers/") + controller + QString(" -f Makefile.darwin-op clean");
  instance->ExecuteSSHCommand((char*)makeClean.toStdString().c_str());
  QString makeController = QString("make -C /darwin/Linux/project/webots/controllers/") + controller + QString(" -f Makefile.darwin-op");
  instance->ExecuteSSHCommand((char*)makeController.toStdString().c_str());
  emit instance->updateProgressSignal(80);

  // Show compilation
  instance->ShowOutputSSHCommand();
  emit instance->updateProgressSignal(90);
    
  if(instance->mMakeDefaultControllerCheckBox->isChecked()) {
	emit instance->updateStatusSignal("Status : Installing controller (7/7)"); 
	// Install controller
    QString CPCommande = QString("cp /darwin/Linux/project/webots/controllers/") + controller + QString("/") + controller + QString(" /darwin/Linux/project/webots/default\n");
    instance->ExecuteSSHCommand((char*)CPCommande.toStdString().c_str());
    instance->WaitEndSSHCommand();
    // Remove compilation files
    instance->ExecuteSSHCommand("rm -r /darwin/Linux/project/webots/controllers");
    instance->WaitEndSSHCommand();
    instance->ExecuteSSHCommand("mkdir /darwin/Linux/project/webots/controllers");
    instance->WaitEndSSHCommand();
    // Clear SSH
    instance->CloseAllSSH();
    // End
    emit instance->ActiveButtonsSignal();
    emit instance->updateStatusSignal("Status : Controller installed"); 
    emit instance->updateProgressSignal(100);
  }
  else {
	if(!instance->isRobotStable()) {
      emit instance->updateStatusSignal("Status : Robot not in a stable position");
      // Remove compilation files
      QString removeController = QString("rm -r /darwin/Linux/project/webots/controllers/") + controller;
      instance->ExecuteSSHCommand((char*)removeController.toStdString().c_str());
      instance->WaitEndSSHCommand();
      // Clear SSH
      instance->CloseAllSSH();
      // End
      emit instance->ActiveButtonsSignal();
      emit instance->updateProgressSignal(100);
    }
    else {
      // Verify that controller exist -> no compilation error
      QString controllerExist;
      char processOutput[256];
      controllerExist = QString("ls /darwin/Linux/project/webots/controllers/") + controller + QString("/") + controller + QString(" | grep -c ") + controller;
      instance->ExecuteSSHCommand((char*)controllerExist.toStdString().c_str());
      instance->ChannelRead(processOutput, false);
      QString process(processOutput);
      if(process.left(1).toInt() > 0) { // OK controller exist
        // Start controller    
        emit instance->addToConsoleSignal(QString("\n------ Starting controller ------\n"));
        emit instance->updateStatusSignal("Status : Starting controller (7/7)"); 
        QString renameController = QString("mv /darwin/Linux/project/webots/controllers/") + controller + QString("/") + controller + QString(" /darwin/Linux/project/webots/controllers/") + controller + QString("/controller");
        instance->ExecuteSSHCommand((char*)renameController.toStdString().c_str());
        instance->ShowOutputSSHCommand();
        QString controllerPath = QString("/darwin/Linux/project/webots/controllers/") + controller + QString("/controller\n");
        instance->ExecuteSSHGraphicCommand((char*)controllerPath.toStdString().c_str(), controllerPath.size()); // need to use graphic in order to open the keyboard window
        emit instance->updateProgressSignal(100);
      
        instance->mConnectionState = true;
        emit instance->ActiveButtonsSignal();
        emit instance->SendControllerButtonSignal();
        emit instance->updateStatusSignal("Status : Controller running"); 

        // Show controller output
#ifndef WIN32 // because otherwise stopping the thread while using the ssh connection cause problem on Windows
        while(1)
          instance->ShowOutputSSHCommand();
#endif
        }
        else { // controller do not exist
          // Remove compilation files
          instance->ExecuteSSHCommand("rm -r /darwin/Linux/project/webots/controllers");
          instance->WaitEndSSHCommand();
          instance->ExecuteSSHCommand("mkdir /darwin/Linux/project/webots/controllers");
          instance->WaitEndSSHCommand();
          // Clear SSH
          instance->CloseAllSSH();
          // End
          emit instance->resetControllerButtonSignal();
          emit instance->ActiveButtonsSignal();
          emit instance->updateStatusSignal("Status : disconnected"); 
          emit instance->updateProgressSignal(100);
        }
    }
  }
  pthread_exit(NULL);
  return NULL;
}

// -------------------------------------------------------------------------- //
// *** Webots API installation function and corresponding thread function *** //
// -------------------------------------------------------------------------- //

int Transfer::installAPI() {

  emit updateStatusSignal("Status : Installation/Update of Webots API");

  QString managerDir, darwinDir, installArchive, webotsHome;
  webotsHome = QString(QProcessEnvironment::systemEnvironment().value("WEBOTS_HOME"));
  managerDir = webotsHome + QString("/resources/projects/robots/darwin-op/libraries/managers");
  darwinDir = webotsHome + QString("/resources/projects/robots/darwin-op");
  installArchive = QDir::tempPath() + QString("/webots_darwin_") + QString::number((int)QCoreApplication::applicationPid()) + QString("_install.zip");

  emit updateProgressSignal(10);
  // Creat archive
  ZIP::CompressFolder(installArchive, managerDir + QString("/include"), true, "include");
  ZIP::AddFolderToArchive(installArchive, managerDir + QString("/src"), true, "src");
  ZIP::AddFolderToArchive(installArchive, managerDir + QString("/lib"), true, "lib");
  ZIP::AddFolderToArchive(installArchive, darwinDir + QString("/transfer"), true, "transfer");
  ZIP::AddFolderToArchive(installArchive, darwinDir + QString("/config"), true, "config");
  ZIP::AddFolderToArchive(installArchive, darwinDir + QString("/check_start_position"), true, "check_start_position");
  ZIP::AddFolderToArchive(installArchive, darwinDir + QString("/remote_control"), true, "remote_control");
  
  emit updateProgressSignal(20);

  // Clean directory /darwin/Linux/project/webots
  // Remove directory webots but save controller installed
  ExecuteSSHCommand("mv /darwin/Linux/project/webots/default /home/darwin/default");
  WaitEndSSHCommand();
  ExecuteSSHCommand("rm -r /darwin/Linux/project/webots");
  WaitEndSSHCommand();
    
  // Create new directory webots
  if(MakeRemoteDirectory("/darwin/Linux/project/webots") < 0) {
    emit updateStatusSignal("Status : Problem while creating directory webots"); 
    // Delete local archive
    QFile deleteArchive(installArchive);
    if(deleteArchive.exists())
      deleteArchive.remove();
    return -1;
  }
  // Create new directory controller
  if(MakeRemoteDirectory("/darwin/Linux/project/webots/controllers") < 0) {
    emit updateStatusSignal("Status : Problem while creating directory controller"); 
    // Delete local archive
    QFile deleteArchive(installArchive);
    if(deleteArchive.exists())
      deleteArchive.remove();
    return -1;
  }
  // Create new directory backup
  if(MakeRemoteDirectory("/darwin/Linux/project/webots/backup") < 0) {
    emit updateStatusSignal("Status : Problem while creating directory backup"); 
    // Delete local archive
    QFile deleteArchive(installArchive);
    if(deleteArchive.exists())
      deleteArchive.remove();
    return -1;
  }
  emit updateProgressSignal(30);
  
  // Reinstall previous controller installed
  ExecuteSSHCommand("mv /home/darwin/default /darwin/Linux/project/webots/default");
  WaitEndSSHCommand();

  // Send archive file
  if(SendFile((char*)installArchive.toStdString().c_str(), "/darwin/Linux/project/webots/install.zip") < 0) {
    // Delete local archive
    QFile deleteArchive(installArchive);
    if(deleteArchive.exists())
      deleteArchive.remove();
    return -1;
  }
  emit updateProgressSignal(40);

  ExecuteSSHCommand("chmod 755 /darwin/Linux/project/webots/install.zip");
  WaitEndSSHCommand();

  // Delete local archive
  QFile deleteArchive(installArchive);
  if(deleteArchive.exists())
    deleteArchive.remove();
  emit updateProgressSignal(45);

  // Decompress remote controller files
  ExecuteSSHCommand("unzip /darwin/Linux/project/webots/install.zip");
  emit updateProgressSignal(50);
  WaitEndSSHCommand();
  
  // Move files and delete archive
  ExecuteSSHCommand("mv /home/darwin/config/rc.local_original /darwin/Linux/project/webots/backup/rc.local_original");
  ExecuteSSHCommand("mv /home/darwin/include /darwin/Linux/project/webots/include");
  ExecuteSSHCommand("mv /home/darwin/lib /darwin/Linux/project/webots/lib");
  ExecuteSSHCommand("mv /home/darwin/src /darwin/Linux/project/webots/src");
  ExecuteSSHCommand("mv /home/darwin/transfer /darwin/Linux/project/webots/transfer");
  ExecuteSSHCommand("mv /home/darwin/check_start_position /darwin/Linux/project/webots/check_start_position");
  ExecuteSSHCommand("mv /home/darwin/config /darwin/Linux/project/webots/config");
  ExecuteSSHCommand("mv /home/darwin/remote_control /darwin/Linux/project/webots/remote_control");
  ExecuteSSHCommand("rm /darwin/Linux/project/webots/install.zip");
  emit updateProgressSignal(60);
  
  // Compile Wrapper 
  ExecuteSSHCommand("make -C /darwin/Linux/project/webots/check_start_position -f Makefile.darwin-op clean");
  ShowOutputSSHCommand();
  ExecuteSSHCommand("make -C /darwin/Linux/project/webots/remote_control -f Makefile.darwin-op clean");
  ShowOutputSSHCommand();
  ExecuteSSHCommand("make -C /darwin/Linux/project/webots/transfer/lib -f Makefile clean");
  ShowOutputSSHCommand();
  emit updateProgressSignal(65);
  ExecuteSSHCommand("make -C /darwin/Linux/project/webots/lib -f Makefile.darwin-op clean");
  ShowOutputSSHCommand();
  emit updateProgressSignal(70);
  ExecuteSSHCommand("make -C /darwin/Linux/project/webots/check_start_position -f Makefile.darwin-op");
  ShowOutputSSHCommand();
  ExecuteSSHCommand("make -C /darwin/Linux/project/webots/remote_control -f Makefile.darwin-op");
  ShowOutputSSHCommand();
  ExecuteSSHCommand("make -C /darwin/Linux/project/webots/transfer/lib -f Makefile");
  emit updateProgressSignal(75);
  ShowOutputSSHCommand();
  ExecuteSSHCommand("make -C /darwin/Linux/project/webots/lib -f Makefile.darwin-op");
  emit updateProgressSignal(80);
  ShowOutputSSHCommand();  

  emit updateProgressSignal(85);
  
  // Permanently stop demo program
  if(ExecuteSSHSudoCommand("cp /darwin/Linux/project/webots/config/rc.local_custom /etc/rc.local\n", sizeof("cp /darwin/Linux/project/webots/config/rc.local_custom /etc/rc.local\n"), ((char*)(mPasswordLineEdit->text() + "\n").toStdString().c_str()), sizeof((char*)(mPasswordLineEdit->text() + "\n").toStdString().c_str())) < 0) {
    emit ActiveButtonsSignal();
    CloseAllSSH();
    emit updateProgressSignal(100);
    return 0;
  }
  WaitEndSSHCommand();
  emit updateProgressSignal(99);
  
  emit updateStatusSignal("Status : Webots API installed"); 
  
  return 1;
}

// ---------------------------------------------------------------------------- //
// *** Webots API uninstallation function and corresponding thread function *** //
// ---------------------------------------------------------------------------- //

void Transfer::uninstall() {
  QMessageBox msgBox;
  msgBox.setWindowTitle("Webots API uninstallation");
  msgBox.setText("You are going to completely uninstall Webots API from DARwIn-OP");
  msgBox.setInformativeText("Are you sure is it what do you want to do?");
  msgBox.setStandardButtons(QMessageBox ::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setDetailedText("Warning all the Webots API will be uninstall.\nThe controllers installed with Webots will be unistall too and the demo program will be restore.\n");
  int warning = msgBox.exec();
  if(warning == QMessageBox::Yes) {
    // The Wrapper is uninstalled from a Thread in order to not freeze the window
    pthread_t uninstallThread;
    pthread_create(&uninstallThread, NULL, this->thread_uninstall, this);
  }
}

void * Transfer::thread_uninstall(void *param) {
  Transfer * instance = ((Transfer*)param);
  emit instance->UnactiveButtonsSignal();
  emit instance->updateProgressSignal(0);
  
  // The Wrapper is uninstalled in a Thread in order to not freeze the window
  // Create SSH session and channel
  emit instance->updateStatusSignal("Status : Connection in progress (1/3)");
  if(instance->StartSSH() < 0) {
    instance->CloseAllSSH();
    emit instance->ActiveButtonsSignal();
    emit instance->updateProgressSignal(100);
    return NULL;
  }
  
  emit instance->updateProgressSignal(10);
  emit instance->updateStatusSignal("Status : Restoring demo controller (2/3)");
  // Restore rc.local
  if(instance->ExecuteSSHSudoCommand("cp /darwin/Linux/project/webots/backup/rc.local_original /etc/rc.local\n", sizeof("cp /darwin/Linux/project/webots/backup/rc.local_original /etc/rc.local\n"), (char*)(instance->mPasswordLineEdit->text() + "\n").toStdString().c_str(), sizeof((char*)(instance->mPasswordLineEdit->text() + "\n").toStdString().c_str())) < 0) {
    instance->CloseAllSSH();
    emit instance->ActiveButtonsSignal();
    emit instance->updateProgressSignal(100);
    return NULL;
  }
  emit instance->updateProgressSignal(75);
  
  emit instance->updateStatusSignal("Status : Removing files (3/3)");
  // Remove directory webots
  emit instance->updateProgressSignal(85);
  instance->ExecuteSSHCommand("rm -r /darwin/Linux/project/webots");
  instance->WaitEndSSHCommand();
  // Clear SSH
  emit instance->updateProgressSignal(90);
  instance->CloseAllSSH();
  emit instance->updateStatusSignal("Status : Webots API uninstalled");
  emit instance->ActiveButtonsSignal();
  
  emit instance->updateProgressSignal(100);
  pthread_exit(NULL);
  return NULL;
}

// --------------------------- //
// *** Auxiliary functions *** //
// --------------------------- //

void Transfer::waitRemoteSlot() {
  int static i = 5;
  int static direction = 1;
  
  if(mRemoteProgressDialog != NULL) {
    if(!mRemoteProgressDialog->isVisible()) {
	  direction = 5;
      i = 1;
      mRemoteStartingTimeCounter = 0;
    }
      
    mRemoteStartingTimeCounter++; // Count the number of times that the loop of 10ms is called -> time estimation for next time
    
    if(mRemoteStartingTime > 0) {  // time has already been tested and saved
    
	  int minutes = (mRemoteStartingTime - mRemoteStartingTimeCounter) / 6000;  // 6000 = 0.01 * 60, 0.01 because loop is called every 10ms
	  if(minutes < 0)
	    minutes = 0;
	  
      int seconds = (int)((mRemoteStartingTime - mRemoteStartingTimeCounter) * 0.01) % 60;
	  if(seconds < 0)
	    seconds = 0;
	
      mRemoteProgressDialog->setLabelText(QString("Starting remot-control.\nPlease wait.\nApproximated remaining time : ") + QString::number(minutes) + QString("m") + QString::number(seconds) + QString("s"));
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

void Transfer::RemoteCanceledSlot() {
  endWaitRemotSlot();
  pthread_cancel(*mThread);
  emit ActiveButtonsSignal();
  CloseAllSSH();
  mStatusLabel->setText(QString("Status : Disconnected"));
  mRemoteEnable = false;
  emit resetRemoteButtonSignal();
}

void Transfer::endWaitRemotSlot() {
  mRemoteTime->stop();
  delete mRemoteProgressBar;
  delete mRemoteProgressDialog;
  mRemoteProgressDialog = NULL;
  mRemoteProgressBar = NULL;
}

void Transfer::activateRemoteControlSlot() {
  mSettings->setValue("darwin-op_window/remote_starting_time", QString::number(mRemoteStartingTimeCounter));  // Save time needed to start remote-control for next time
  wb_robot_set_mode(WB_MODE_REMOTE_CONTROL, (void *)(mIPLineEdit->text().toStdString().c_str()));
  mStatusLabel->setText(QString("Status : Remote control started"));
}

void Transfer::ShowOutputSSHCommand() {
  char console1[1024];
  QChar console2[1024];
  QChar newLineCaractere = QChar('\n'), endCaractere = QChar('\0');
  unsigned int nbytesStandard = 0, nbytesError = 0, i = 0, j = 0, k = 0;
  QString buffer1(""), buffer2("");
  do {
	  
    // Read standard output
    if((nbytesStandard = ChannelRead(console1, false)) > 0 && nbytesStandard < 250) { // if to many bytes are read, it means their was an error -> just skip it
      for(i = 0, j = 0; i < nbytesStandard; i++, j++) {
        console2[j] = QChar::fromAscii(console1[i]);
        if(console2[j] == newLineCaractere) { // if new line detected send this line
          console2[j] = endCaractere;
		  buffer1 += QString(console2);
          emit addToConsoleSignal(buffer1);
          buffer1 = QString(""); 
          j = -1;
        }
      }
      console2[j] = endCaractere;
      buffer1 += QString(console2);
    }
    
    // Read stderr output
    if(nbytesStandard == 0 && (nbytesError = ChannelRead(console1, true)) > 0) {
      for(i = 0, j = 0; i < nbytesError; i++, j++) {
		while(console1[i] < 0) { // remove all bad caracters
		  for(k = i; k < nbytesError; k++) {
		    console1[k] = console1[k+1];
		  }
		  console1[nbytesError] = '\0';
		}
        console2[j] = QChar::fromLatin1(console1[i]);
        if(console2[j] == newLineCaractere) { // if new line detected send this line
          console2[j] = endCaractere;
		  buffer2 += QString(console2);
          emit addToConsoleRedSignal(buffer2);
          buffer2 = QString(""); 
          j = -1;
        }
      }
      console2[j] = endCaractere;
      buffer2 += QString(console2);
    }
  } while ((nbytesStandard > 0) || (nbytesError > 0)); // Nothing else to read anymore
}

void Transfer::restoreSettings() {
  mIPLineEdit->setText(QString("192.168.123.1"));
  mUsernameLineEdit->setText(QString("darwin"));
  mPasswordLineEdit->setText(QString("111111"));
}

void Transfer::saveSettings() {
  mSettings->setValue("darwin-op_window/IP", mIPLineEdit->text());
  mSettings->setValue("darwin-op_window/username", mUsernameLineEdit->text());
  mSettings->setValue("darwin-op_window/password", mPasswordLineEdit->text());
}

void Transfer::loadSettings() {
  mIPLineEdit->setText(mSettings->value("darwin-op_window/IP", QString("192.168.123.1")).toString());
  mUsernameLineEdit->setText(mSettings->value("darwin-op_window/username", QString("darwin")).toString());
  mPasswordLineEdit->setText(mSettings->value("darwin-op_window/password", QString("111111")).toString());
}

void Transfer::ActiveButtonsSlot() {
  mUninstallButton->setEnabled(true);
  mSendControllerButton->setEnabled(true);
  mMakeDefaultControllerCheckBox->setEnabled(true);
  mRemoteControlButton->setEnabled(true);
}

void Transfer::UnactiveButtonsSlot() {
  mUninstallButton->setEnabled(false);
  mSendControllerButton->setEnabled(false);
  mMakeDefaultControllerCheckBox->setEnabled(false);
  mRemoteControlButton->setEnabled(false);
}

void Transfer::updateProgressSlot(int percent) {
  mProgressBar->setValue(percent);
  if(percent == 0)
    mProgressBar->show();
  else if(percent == 100)
    mProgressBar->hide();
}

void Transfer::addToConsoleSlot(QString output) {
  mConsoleShowTextEdit->setTextColor(QColor( 0, 0, 0));
  mConsoleShowTextEdit->append(output);
  mConsoleShowTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}

void Transfer::addToConsoleRedSlot(QString output) {
  mConsoleShowTextEdit->setTextColor(QColor( 255, 0, 0));
  mConsoleShowTextEdit->append(output);
  mConsoleShowTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}

bool Transfer::isWrapperUpToDate() {
  int index1 = 0, index2 = 0, index3 = 0;
  QString version, versionInstalled;
  QStringList versionList, versionInstalledList;
  
  if(mWrapperVersionFile->exists()) {
    if(mWrapperVersionFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream versionStream(mWrapperVersionFile);
      
      if(!versionStream.atEnd()) {
        version = versionStream.readLine();
        versionList = version.split(".");
        index1 = versionList.at(0).toInt();
        index2 = versionList.at(1).toInt();
        index3 = versionList.at(2).toInt();
      }
      else
        return false; //PROBLEM file empty
    }
    else
      return false; //PROBLEM could not open file
    mWrapperVersionFile->close();
  }
  else
    return false; //PROBLEM file do not exist
    
  char buffer[32];
  if(ReadRemotFile("/darwin/Linux/project/webots/config/version.txt", buffer) < 0)
    return false;
  
  versionInstalled = QString(buffer);
  versionInstalledList = versionInstalled.split(".");
  if( (index1 > versionInstalledList.at(0).toInt()) || (index2 > versionInstalledList.at(1).toInt()) || (index3 > versionInstalledList.at(2).toInt()) )
    return false; // new version of the Wrapper*/
  
  return true; 
}

bool Transfer::isRobotStable() {
  // Start controller check_start_position, if output is OK => robot is stable
  ExecuteSSHCommand("/darwin/Linux/project/webots/check_start_position/check_start_position | grep -c OK");
  char processOutput[256];
  ChannelRead(processOutput, false);
  QString process(processOutput[0]);
  if(process.toInt() > 0) 
    return true;
  else {
    mStabilityResponse = -1;
	emit robotInstableSignal();
    while(mStabilityResponse == -1)
      {wait(1000);}
    return mStabilityResponse;
  }
}

void Transfer::robotInstableSlot() {
	QString imagePath;
	imagePath = StandardPaths::getWebotsHomePath() + QString("resources/projects/robots/darwin-op/plugins/robot_windows/darwin-op_window/images/start_position.png.png");
    QMessageBox msgBox;
    msgBox.setWindowTitle("Stability check");
    msgBox.setText("The robot doesn't seems to be in a stable position.");
    msgBox.setInformativeText("What do you want to do?");
    msgBox.setDetailedText("Warning, the robot doesn't seems to be in it's stable start-up position.\nWe recommand you to put the robot in the stable position and to retry.\nThe stable position is when the robot is sit down (illustration above).\nNevertheless if you want to start the controller in this position you can press Ignore, but be aware that the robot can make sudden movements to reach its start position and this can damage it!");
    msgBox.setIconPixmap(QPixmap((char*)imagePath.toStdString().c_str()));
    msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Abort | QMessageBox::Ignore);
    msgBox.setDefaultButton(QMessageBox::Retry);
    int warning = msgBox.exec();
    if(warning == QMessageBox::Abort)
      mStabilityResponse = false;
    else if(warning == QMessageBox::Ignore)
      mStabilityResponse = true;
    else 
      mStabilityResponse = isRobotStable();
}

bool Transfer::isFrameworkUpToDate() {
  int index1 = 0, index2 = 0, index3 = 0;
  QString version, versionInstalled;
  QStringList versionList, versionInstalledList;
  
  if(mFrameworkVersionFile->exists()) {
    if(mFrameworkVersionFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream versionStream(mFrameworkVersionFile);
      
      if(!versionStream.atEnd()) {
        version = versionStream.readLine();
        versionList = version.split(".");
        index1 = versionList.at(0).toInt();
        index2 = versionList.at(1).toInt();
        index3 = versionList.at(2).toInt();
      }
      else
        return false; //PROBLEM file empty
    }
    else
      return false; //PROBLEM could not open file
    mFrameworkVersionFile->close();
  }
  else
    return false; //PROBLEM file do not exist
    

  char buffer[32];
  if(ReadRemotFile("/darwin/version.txt", buffer) < 0)
    return false;
  
  versionInstalled = QString(buffer);
  versionInstalledList = versionInstalled.split(".");
  if( (index1 > versionInstalledList.at(0).toInt()) || (index2 > versionInstalledList.at(1).toInt()) || (index3 > versionInstalledList.at(2).toInt()) )
    return false; // new version of the Framework*/
  
  return true; 
}

int Transfer::updateFramework() {

  emit updateStatusSignal("Status : Updating the Framework");

  QString darwinDir, installArchive, webotsHome;
  webotsHome = QString(QProcessEnvironment::systemEnvironment().value("WEBOTS_HOME"));
  darwinDir = webotsHome + QString("/resources/projects/robots/darwin-op/libraries/darwin/darwin");
  installArchive = QDir::tempPath() + QString("/webots_darwin_") + QString::number((int)QCoreApplication::applicationPid()) + QString("_update.zip");

  emit updateProgressSignal(10);
  // Creat archive
  ZIP::CompressFolder(installArchive, darwinDir + QString("/Data"), true, "Data");
  ZIP::AddFolderToArchive(installArchive, darwinDir + QString("/Linux"), true, "Linux");
  ZIP::AddFolderToArchive(installArchive, darwinDir + QString("/Framework"), true, "Framework");
  ZIP::AddFileToArchive(installArchive, darwinDir + QString("/version.txt"), "version.txt");
  
  emit updateProgressSignal(20);
    

  // Send archive file
  if(SendFile((char*)installArchive.toStdString().c_str(), "/darwin/update.zip") < 0) {
    // Delete local archive
    QFile deleteArchive(installArchive);
    if(deleteArchive.exists())
      deleteArchive.remove();
    return -1;
  }
  emit updateProgressSignal(30);

  ExecuteSSHCommand("chmod 755 /darwin/update.zip");
  WaitEndSSHCommand();

  // Delete local archive
  QFile deleteArchive(installArchive);
  if(deleteArchive.exists())
    deleteArchive.remove();
  emit updateProgressSignal(35);

  // Decompress remote controller files
  ExecuteSSHCommand("unzip /darwin/update.zip");
  emit updateProgressSignal(40);
  WaitEndSSHCommand();

  // Move files and delete archive
  ExecuteSSHCommand("cp /home/darwin/version.txt /darwin/version.txt");
  ExecuteSSHCommand("cp -r /home/darwin/Framework /darwin");
  ExecuteSSHCommand("cp -r /home/darwin/Linux /darwin");
  ExecuteSSHCommand("cp -r /home/darwin/Data /darwin");
  ExecuteSSHCommand("rm /darwin/update.zip");
  ExecuteSSHCommand("rm /home/darwin/version.txt");
  ExecuteSSHCommand("rm -r /home/darwin/Framework");
  ExecuteSSHCommand("rm -r /home/darwin/Linux");
  ExecuteSSHCommand("rm -r /home/darwin/Data");
  emit updateProgressSignal(50);
  
  // Compile Framework 
  ExecuteSSHCommand("make -C /darwin/Linux/build -f Makefile clean");
  ShowOutputSSHCommand();
  emit updateProgressSignal(60);
  ExecuteSSHCommand("make -C /darwin/Linux/build -f Makefile");
  ShowOutputSSHCommand();  
  
  // Delete version file of the Wrapper in order to force the update/recompilation of it
  ExecuteSSHCommand("rm /darwin/Linux/project/webots/config/version.txt");
  WaitEndSSHCommand();
  
  emit updateStatusSignal("Status : Framework Updated"); 
  
  return 1;
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
    msgBox.exec();
  }
}

void Transfer::remoteCameraWarningSlot() {
  QMessageBox msgBox;
  msgBox.setWindowTitle("Warning camera resolution not supported");
  msgBox.setText("Warning this camera resolution is not supported in remote control.");
  msgBox.setInformativeText("The following resolutions are available:\n\tWidth :  320/160/80/40\n\tHeight : 240/120/60/30");
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.exec();
}

void Transfer::updateStatusSlot(QString status) {
  mStatusLabel->setText(status);
}

int Transfer::StartSSH() {
  if(OpenSSHSession(mIPLineEdit->text(), mUsernameLineEdit->text(), mPasswordLineEdit->text()) < 0) {
    mStatusLabel->setText(QString("Connection error : ")+ QString(mSSHError));
    return -1;
  }
  saveSettings();
  return 1;
}

int Transfer::killProcessIfRunning(Transfer * instance, QString process) {
  instance->ExecuteSSHCommand((char*)(QString("pstree | grep -c ") + process).toStdString().c_str());
  char processOutput[256];
  instance->ChannelRead(processOutput, false);
  QString result(processOutput);
  if(result.left(1).toInt() > 0) {  // process is running
    // kill process
    if(instance->ExecuteSSHSudoCommand((char*)(QString("killall ") + process + QString("\n")).toStdString().c_str(), (QString("killall ") + process + QString("\n")).size(), ((char*)(instance->mPasswordLineEdit->text() + "\n").toStdString().c_str()), sizeof((char*)(instance->mPasswordLineEdit->text() + "\n").toStdString().c_str())) < 0) {
      return -1;
    }
    instance->WaitEndSSHCommand();
  }
  return 1;
}

void Transfer::resetRemoteButtonSlot() {
  mRemoteControlButton->setIcon(*mRemoteControlIcon);
  mRemoteControlButton->setToolTip("Start remote control.");
  mRemoteEnable = false;
}

void Transfer::resetControllerButtonSlot() {
  mSendControllerButton->setIcon(*mSendControllerIcon);
  mSendControllerButton->setToolTip("Send the controller curently used in simulation on the real robot and play it.");
  mConnectionState = false;
}

void Transfer::wait(int duration) {
#ifdef WIN32
  Sleep(duration);
#else
  // because usleep cannot handle values
  // bigger than 1 second (1'000'000 nanoseconds)
  int seconds = duration / 1000;
  int milliSeconds = duration - 1000 * seconds;
  for (int i=0; i<seconds; i++)
    usleep(1000000);
  usleep(1000 * milliSeconds);
#endif
}

void Transfer::SendControllerButtonSlot() {
  mSendControllerButton->setIcon(*mStopControllerIcon);
  mSendControllerButton->setToolTip("Stop the controller on the real robot.");
}
