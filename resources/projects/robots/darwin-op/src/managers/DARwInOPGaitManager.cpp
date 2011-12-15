#include <managers/DARwInOPGaitManager.hpp>

#include <webots/Robot.hpp>
#include <webots/Servo.hpp>
#include <MX28.h>
#include <Walking.h>
#include <minIni.h>
#ifdef CROSSCOMPILATION
#include <MotionManager.h>
#endif

#include <iostream>
#include <cmath>

using namespace Robot;
using namespace managers;
using namespace webots;
using namespace std;

static const string servoNames[DGM_NSERVOS] = {
  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */,
  "ArmLowerR" /*ID5 */, "ArmLowerL" /*ID6 */, "PelvYR"    /*ID7 */, "PelvYL"    /*ID8 */,
  "PelvR"     /*ID9 */, "PelvL"     /*ID10*/, "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/,
  "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR"    /*ID15*/, "AnkleL"    /*ID16*/,
  "FootR"     /*ID17*/, "FootL"     /*ID18*/, "Neck"      /*ID19*/, "Head"      /*ID20*/
};

DARwInOPGaitManager::DARwInOPGaitManager(webots::Robot *robot, const std::string &iniFilename) :
  mRobot(robot),
  mCorrectlyInitialized(true),
  mXAmplitude(0.0),
  mAAmplitude(0.0)
{
  if (!mRobot) {
    cerr << "DARwInOPGaitManager: The robot instance is required" << endl;
    mCorrectlyInitialized = false;
    return;
  }

#ifndef CROSSCOMPILATION
  for (int i=0; i<DGM_NSERVOS; i++)
    mServos[i] = mRobot->getServo(servoNames[i]);
#endif
  
  minIni ini(iniFilename.c_str());
  Walking *walking = Walking::GetInstance();
  walking->Initialize();
  walking->LoadINISettings(&ini);
  
#ifdef CROSSCOMPILATION
  for (int i=0; i<DGM_NSERVOS; i++)
    walking->m_Joint.SetEnable(i, true);
  
  MotionManager::GetInstance()->AddModule((MotionModule*)Walking::GetInstance());
  MotionManager::GetInstance()->SetEnable(true);
#endif
}

DARwInOPGaitManager::~DARwInOPGaitManager() {
}

void DARwInOPGaitManager::step(int step) {
  if (step < 8) {
    cerr << "DARwInOPGaitManager: steps of less than 8ms are not supported" << endl;
    return;
  }
#ifdef CROSSCOMPILATION
  if (step != 8) {
    cerr << "DARwInOPGaitManager: steps of 8ms are required" << endl;
    return;
  }
#endif
  
  Walking *walking = Walking::GetInstance();
  walking->X_MOVE_AMPLITUDE = mXAmplitude;
  walking->A_MOVE_AMPLITUDE = mAAmplitude;
  
  int numberOfStepToProcess = step / 8;
  for (int i=0; i<numberOfStepToProcess; i++)
    walking->Process();
#ifndef CROSSCOMPILATION
  for (int i=0; i<DGM_NSERVOS; i++)
    mServos[i]->setPosition(valueToPosition(walking->m_Joint.GetValue(i+1)));
#endif
}

void DARwInOPGaitManager::stop() {
  Walking *walking = Walking::GetInstance();
  walking->Stop();
}

void DARwInOPGaitManager::start() {
  Walking *walking = Walking::GetInstance();
  walking->Start();
}

#ifndef CROSSCOMPILATION
double DARwInOPGaitManager::valueToPosition(unsigned short value) {
  double degree = MX28::Value2Angle(value);
  double position = degree / 180.0 * M_PI;
  return position;
}
#endif
