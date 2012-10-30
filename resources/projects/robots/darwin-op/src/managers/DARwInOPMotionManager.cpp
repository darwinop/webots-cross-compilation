#include <managers/DARwInOPMotionManager.hpp>

#include <managers/DARwInOPDirectoryManager.hpp>

#include <webots/Robot.hpp>
#include <webots/Servo.hpp>
#include <Action.h>
#include <MX28.h>

#ifdef CROSSCOMPILATION
#include <managers/DARwInOPMotionTimerManager.hpp>
#include <MotionManager.h>
#include <unistd.h>
#endif

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>

using namespace Robot;
using namespace managers;
using namespace webots;
using namespace std;
static const string servoNames[DMM_NSERVOS] = {

  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */,
  "ArmLowerR" /*ID5 */, "ArmLowerL" /*ID6 */, "PelvYR"    /*ID7 */, "PelvYL"    /*ID8 */,
  "PelvR"     /*ID9 */, "PelvL"     /*ID10*/, "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/,
  "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR"    /*ID15*/, "AnkleL"    /*ID16*/,
  "FootR"     /*ID17*/, "FootL"     /*ID18*/, "Neck"      /*ID19*/, "Head"      /*ID20*/
};

DARwInOPMotionManager::DARwInOPMotionManager(webots::Robot *robot) :
  mRobot(robot),
  mCorrectlyInitialized(true)
{
  if (!mRobot) {
    cerr << "DARwInOPMotionManager: The robot instance is required" << endl;
    mCorrectlyInitialized = false;
    return;
  }
  mBasicTimeStep = mRobot->getBasicTimeStep();
  
  string filename;
  
#ifdef CROSSCOMPILATION
  DARwInOPMotionTimerManager::MotionTimerInit();

  int firm_ver = 0;
  CM730 *cm730 = mRobot->getCM730();
  if(cm730->ReadByte(JointData::ID_HEAD_PAN, MX28::P_VERSION, &firm_ver, 0)  != CM730::SUCCESS) {
    fprintf(stderr, "Can't read firmware version from Dynamixel ID %d!\n", JointData::ID_HEAD_PAN);
    mCorrectlyInitialized = false;
    mAction = NULL;
    return;
  }

  if(0 < firm_ver && firm_ver < 27)
    filename = DARwInOPDirectoryManager::getDataDirectory() + "motion_1024.bin";
  else if (27 <= firm_ver)
    filename = DARwInOPDirectoryManager::getDataDirectory() + "motion_4096.bin";
  else {
    fprintf(stderr, "The firmware version of Dynamixel ID %d is corrupted.\n", JointData::ID_HEAD_PAN);
    mCorrectlyInitialized = false;
    mAction = NULL;
    return;
  }
#else
  for (int i=0; i<DMM_NSERVOS; i++) {
    mCurrentPositions[i] = 0.0;
    mServos[i] = mRobot->getServo(servoNames[i]);
  }
  filename = DARwInOPDirectoryManager::getDataDirectory() + "motion_4096.bin";
#endif
  
  mAction = Action::GetInstance();
  
  if( mAction->LoadFile(const_cast<char *> (filename.c_str())) == false ) {
    cerr << "DARwInOPMotionManager: Cannot load the motion from " << filename << endl;
    mCorrectlyInitialized = false;
    mAction = NULL;
    return;
  }

#ifdef CROSSCOMPILATION
  MotionManager::GetInstance()->AddModule((MotionModule*) mAction);
#endif
}

DARwInOPMotionManager::~DARwInOPMotionManager() {
  if (mAction && mAction->IsRunning())
    mAction->Stop();
}

void DARwInOPMotionManager::playPage(int id) {
  if (!mCorrectlyInitialized)
    return;
  
#ifdef CROSSCOMPILATION
  mAction->m_Joint.SetEnableBody(true, true);
  MotionStatus::m_CurrentJoints.SetEnableBody(true);
  MotionManager::GetInstance()->SetEnable(true);
  
  Action::GetInstance()->Start(id);
  while(Action::GetInstance()->IsRunning())
    usleep(mBasicTimeStep*1000);
    
  // Disable the Joints in the Gait Manager, this allow to control them again 'manualy' //
  mAction->m_Joint.SetEnableBody(false, true);
  MotionStatus::m_CurrentJoints.SetEnableBody(false);
  MotionManager::GetInstance()->SetEnable(false);   
#else
  Action::PAGE page;
  if (mAction->LoadPage(id, &page)) {
    // cout << "Play motion " << setw(2) << id << ": " << page.header.name << endl;
    for(int i=0; i<page.header.repeat; i++) {
      for(int j=0; j<page.header.stepnum; j++) {
         for(int k=0; k<DMM_NSERVOS; k++)
           mTargetPositions[k] = valueToPosition(page.step[j].position[k+1]);
         achieveTarget(8*page.step[j].time);
         wait(8*page.step[j].pause);
      }
    }
    if(page.header.next != 0)
      playPage(page.header.next);
  }
  else
    cerr << "Cannot load the page" << endl;
#endif
}

#ifndef CROSSCOMPILATION
void DARwInOPMotionManager::myStep() {
  int ret = mRobot->step(mBasicTimeStep);
  if (ret == -1)
    exit(EXIT_SUCCESS);
}

void DARwInOPMotionManager::wait(int ms) {
  double startTime = mRobot->getTime();
  double s = (double) ms / 1000.0;
  while (s + startTime >= mRobot->getTime())
    myStep();
}

void DARwInOPMotionManager::achieveTarget(int msToAchieveTarget) {
  int stepNumberToAchieveTarget = msToAchieveTarget / mBasicTimeStep;
  while (stepNumberToAchieveTarget > 0) {
    for (int i=0; i<DMM_NSERVOS; i++) {
      double dX = mTargetPositions[i] - mCurrentPositions[i];
      double newPosition = mCurrentPositions[i] + dX / stepNumberToAchieveTarget;
      mCurrentPositions[i] = newPosition;
      mServos[i]->setPosition(newPosition);
    }
    myStep();
    stepNumberToAchieveTarget--;
  }
}

double DARwInOPMotionManager::valueToPosition(unsigned short value) {
  double degree = MX28::Value2Angle(value);
  double position = degree / 180.0 * M_PI;
  return position;
}
#endif
