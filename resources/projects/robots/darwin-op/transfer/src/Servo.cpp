#include <webots/Servo.hpp>
#include <webots/Robot.hpp>

#include <JointData.h>
#include <MX28.h>
#include <CM730.h>

#include <cmath>
#include <fstream>
#include <algorithm>

using namespace webots;
using namespace Robot;

std::map<const std::string, int> Servo::mNamesToIDs;
std::map<const std::string, int> Servo::mNamesToLimUp;
std::map<const std::string, int> Servo::mNamesToLimDown;
std::map<const std::string, int> mNamesToInitPos;

template <typename T> int sgn(T val) {
  if(val >= 0)
    return 1;
  else
    return -1;
}

Servo::Servo(const std::string &name, const Robot *robot) :
  Device(name, robot)
{
  initStaticMap();
  mAcceleration   = -1;
  mMaxVelocity    = 10;
  mActualVelocity =  0;
  mGoalPosition = mNamesToInitPos[getName()];
  mTorqueEnable = 1;    // Yes
  mPGain = 32;
  mMovingSpeed = 1023;  // Max speed
  mTorqueLimit = 1023;  // Max torque
}

Servo::~Servo() {
}

void Servo::initStaticMap() {
  static bool firstCall=true;
  if (firstCall) {
    firstCall = false;

    mNamesToIDs["ShoulderR"] = JointData::ID_R_SHOULDER_PITCH;
    mNamesToIDs["ShoulderL"] = JointData::ID_L_SHOULDER_PITCH;
    mNamesToIDs["ArmUpperR"] = JointData::ID_R_SHOULDER_ROLL;
    mNamesToIDs["ArmUpperL"] = JointData::ID_L_SHOULDER_ROLL;
    mNamesToIDs["ArmLowerR"] = JointData::ID_R_ELBOW;
    mNamesToIDs["ArmLowerL"] = JointData::ID_L_ELBOW;
    mNamesToIDs["PelvYR"]    = JointData::ID_R_HIP_YAW;
    mNamesToIDs["PelvYL"]    = JointData::ID_L_HIP_YAW;
    mNamesToIDs["PelvR"]     = JointData::ID_R_HIP_ROLL;
    mNamesToIDs["PelvL"]     = JointData::ID_L_HIP_ROLL;
    mNamesToIDs["LegUpperR"] = JointData::ID_R_HIP_PITCH;
    mNamesToIDs["LegUpperL"] = JointData::ID_L_HIP_PITCH;
    mNamesToIDs["LegLowerR"] = JointData::ID_R_KNEE;
    mNamesToIDs["LegLowerL"] = JointData::ID_L_KNEE;
    mNamesToIDs["AnkleR"]    = JointData::ID_R_ANKLE_PITCH;
    mNamesToIDs["AnkleL"]    = JointData::ID_L_ANKLE_PITCH;
    mNamesToIDs["FootR"]     = JointData::ID_R_ANKLE_ROLL;
    mNamesToIDs["FootL"]     = JointData::ID_L_ANKLE_ROLL;
    mNamesToIDs["Neck"]      = JointData::ID_HEAD_PAN;
    mNamesToIDs["Head"]      = JointData::ID_HEAD_TILT;

    mNamesToLimUp["ShoulderR"] = 4095;
    mNamesToLimUp["ShoulderL"] =    0;
    mNamesToLimUp["ArmUpperR"] = 3548;
    mNamesToLimUp["ArmUpperL"] =  583;
    mNamesToLimUp["ArmLowerR"] = 2804;
    mNamesToLimUp["ArmLowerL"] = 1278;
    mNamesToLimUp["PelvYR"]    = 2480;
    mNamesToLimUp["PelvYL"]    = 1600;
    mNamesToLimUp["PelvR"]     = 2708;
    mNamesToLimUp["PelvL"]     = 2650;
    mNamesToLimUp["LegUpperR"] =  889;
    mNamesToLimUp["LegUpperL"] = 3145;
    mNamesToLimUp["LegLowerR"] = 2038;
    mNamesToLimUp["LegLowerL"] = 2067;
    mNamesToLimUp["AnkleR"]    = 2947;
    mNamesToLimUp["AnkleL"]    = 1142;
    mNamesToLimUp["FootR"]     = 2728;
    mNamesToLimUp["FootL"]     = 1385;
    mNamesToLimUp["Neck"]      =  790;
    mNamesToLimUp["Head"]      = 2660;

    mNamesToLimDown["ShoulderR"] =  191;
    mNamesToLimDown["ShoulderL"] = 3095;
    mNamesToLimDown["ArmUpperR"] = 1605;
    mNamesToLimDown["ArmUpperL"] = 2549;
    mNamesToLimDown["ArmLowerR"] =  970;
    mNamesToLimDown["ArmLowerL"] = 3109;
    mNamesToLimDown["PelvYR"]    =  470;
    mNamesToLimDown["PelvYL"]    = 3680;
    mNamesToLimDown["PelvR"]     = 1390;
    mNamesToLimDown["PelvL"]     = 1400;
    mNamesToLimDown["LegUpperR"] = 2340;
    mNamesToLimDown["LegUpperL"] = 1724;
    mNamesToLimDown["LegLowerR"] = 3342;
    mNamesToLimDown["LegLowerL"] =  722;
    mNamesToLimDown["AnkleR"]    = 1239;
    mNamesToLimDown["AnkleL"]    = 2845;
    mNamesToLimDown["FootR"]     = 1603;
    mNamesToLimDown["FootL"]     = 2435;
    mNamesToLimDown["Neck"]      = 3150;
    mNamesToLimDown["Head"]      = 1815;
    
    mNamesToInitPos["ShoulderR"] = 1500;
    mNamesToInitPos["ShoulderL"] = 2517;
    mNamesToInitPos["ArmUpperR"] = 1834;
    mNamesToInitPos["ArmUpperL"] = 2283;
    mNamesToInitPos["ArmLowerR"] = 2380;
    mNamesToInitPos["ArmLowerL"] = 1710;
    mNamesToInitPos["PelvYR"]    = 2043;
    mNamesToInitPos["PelvYL"]    = 2033;
    mNamesToInitPos["PelvR"]     = 2057;
    mNamesToInitPos["PelvL"]     = 2043;
    mNamesToInitPos["LegUpperR"] = 1277;
    mNamesToInitPos["LegUpperL"] = 2797;
    mNamesToInitPos["LegLowerR"] = 3513;
    mNamesToInitPos["LegLowerL"] =  571;
    mNamesToInitPos["AnkleR"]    = 2843;
    mNamesToInitPos["AnkleL"]    = 1240;
    mNamesToInitPos["FootR"]     = 2077;
    mNamesToInitPos["FootL"]     = 2037;
    mNamesToInitPos["Neck"]      = 2050;
    mNamesToInitPos["Head"]      = 2173;
  }
}

void Servo::setAcceleration(double force){
  mAcceleration = force;
  if(force == -1)				// No Aceleration limitation -> restore previous Velocity limit
    setVelocity(mMaxVelocity);
}

void Servo::setVelocity(double vel){
  CM730 *cm730 = getRobot()->getCM730();
  int value = fabs((vel*30/M_PI)/0.114);  // Need to be verified
  if(value > 1023)
    value = 1023;
  else if(value == 0) // Because 0 means max Velocity for the dynamixel
    value = 1;
  cm730->WriteWord(mNamesToIDs[getName()], MX28::P_MOVING_SPEED_L, value, 0);
  if(mAcceleration == -1)
    mMaxVelocity = vel;
}

void Servo::enablePosition(int ms){  //EMPTY
}

void Servo::disablePosition(){  //EMPTY
}

void Servo::setForce(double force){
  CM730 *cm730 = getRobot()->getCM730();
  if(force == 0)
    cm730->WriteWord(mNamesToIDs[getName()], MX28::P_TORQUE_ENABLE, 0, 0);
  else{
    cm730->WriteWord(mNamesToIDs[getName()], MX28::P_TORQUE_ENABLE, 1, 0);
    this->setMotorForce(fabs(force));
    int firm_ver = 0;
    if(cm730->ReadByte(JointData::ID_HEAD_PAN, MX28::P_VERSION, &firm_ver, 0) != CM730::SUCCESS)
      printf("Can't read firmware version from Dynamixel ID %d!\n", JointData::ID_HEAD_PAN);
    else if(27 <= firm_ver){
      if(force > 0)
        cm730->WriteWord(mNamesToIDs[getName()], MX28::P_GOAL_POSITION_L, mNamesToLimUp[getName()], 0);
      else
        cm730->WriteWord(mNamesToIDs[getName()], MX28::P_GOAL_POSITION_L, mNamesToLimDown[getName()], 0);
     }
     else
       printf("Servo::setForce not available for this version of Dynamixel firmware, please update it.\n");
  }
}

void Servo::setMotorForce(double motor_force){
  CM730 *cm730 = getRobot()->getCM730();
  if(motor_force > 2.5)
  {
    cm730->WriteWord(mNamesToIDs[getName()], MX28::P_TORQUE_ENABLE, 1, 0);
    cm730->WriteWord(mNamesToIDs[getName()], MX28::P_TORQUE_LIMIT_L, 1023, 0);
  }
  else if (motor_force  >  0)
  {
    double value = (motor_force/2.5) * 1023;
    cm730->WriteWord(mNamesToIDs[getName()], MX28::P_TORQUE_ENABLE, 1, 0);
    cm730->WriteWord(mNamesToIDs[getName()], MX28::P_TORQUE_LIMIT_L, value, 0);
  }
  else
    {cm730->WriteWord(mNamesToIDs[getName()], MX28::P_TORQUE_ENABLE, 0, 0);}
}

void Servo::setControlP(double p){
  CM730 *cm730 = getRobot()->getCM730();
  if(p >= 0)
  {
    int value = p * 8; // Seems to be good, but has to be verified
    cm730->WriteWord(mNamesToIDs[getName()], MX28::P_P_GAIN, value, 0);
  }
}

void Servo::enableMotorForceFeedback(int ms){  //EMPTY
}

void Servo::disableMotorForceFeedback(){  //EMPTY
}

double Servo::getMotorForceFeedback() const{
  CM730 *cm730 = getRobot()->getCM730();
  int value = 0;
  double force = 0;

  cm730->ReadWord(mNamesToIDs[getName()], MX28::P_PRESENT_LOAD_L, &value, 0);
  if(value<1024)
    {force = ((double)value/1023) * 2.5;}
  else
    {force = -(((double)value-1023)/1023) * 2.5;}

  return force;
}

double Servo::getPosition() const{
  CM730 *cm730 = getRobot()->getCM730();
  int value = 0;
  double position = 0;

  cm730->ReadWord(mNamesToIDs[getName()], MX28::P_PRESENT_POSITION_L, &value, 0);
  position = (MX28::Value2Angle(value)*M_PI) / 180;
  return position;
}

void Servo::setPosition(double position) {
  CM730 *cm730 = getRobot()->getCM730();
  int value = MX28::Angle2Value(position*180.0/M_PI);

  if(value >= 0 && value <= MX28::MAX_VALUE) {
    int error;
    //       Self-Collision Avoidance      //
    // Work only with a resolution of 4096 //
    if(value > std::max(mNamesToLimDown[getName()], mNamesToLimUp[getName()]))
      value = std::max(mNamesToLimDown[getName()], mNamesToLimUp[getName()])
    else if(value < std::min(mNamesToLimDown[getName()], mNamesToLimUp[getName()]))
      value = std::min(mNamesToLimDown[getName()], mNamesToLimUp[getName()])
      
    cm730->WriteWord(mNamesToIDs[getName()], MX28::P_GOAL_POSITION_L, value, &error);
  }
}

void Servo::updateSpeed(int ms) {
	
  if(mAcceleration != -1) {
    double speed = getSpeed();
    int     sens = sgn(speed);
    
    // Direction of rotation changed //
    //     -> reset of velocity      //
    if(sgn(mActualVelocity) != sens) {
      mActualVelocity = 0;
      setVelocity(0);
    }
	
    if(speed == 0)
      mActualVelocity = 0;
	  
    mActualVelocity = sens * (fabs(mActualVelocity) + ms * mAcceleration / 1000);
    setVelocity(mActualVelocity);
  }
}
    

double Servo::getSpeed() const {
  CM730 *cm730 = getRobot()->getCM730();
  int value = 0;
  double speed = 0;
  
  cm730->ReadWord(mNamesToIDs[getName()], MX28::P_PRESENT_SPEED_L, &value, 0);
  if(value > 1023)
    value = - (value - 1023);
  speed = ((value * M_PI) / 30 ) * 0.114;
  
  return speed;
}

int Servo::getGoalPosition() {
  return mGoalPosition;
}

int Servo::getTorqueEnable() {
  return mTorqueEnable;
}

int Servo::getPGain() {
  return mPGain;
}

int Servo::getMovingSpeed() {
  return mMovingSpeed;
}

int Servo::getTorqueLimit() {
  return mTorqueLimit;
}
