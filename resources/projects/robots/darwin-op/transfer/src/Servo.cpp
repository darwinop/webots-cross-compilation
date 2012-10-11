#include <webots/Servo.hpp>
#include <webots/Robot.hpp>

#include <JointData.h>
#include <MX28.h>
#include <CM730.h>
#include <cmath>

using namespace webots;
using namespace Robot;

std::map<const std::string, int> Servo::mNamesToIDs;

Servo::Servo(const std::string &name, const Robot *robot) :
  Device(name, robot)
{
  initStaticMap();
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

  }
}

void Servo::setAcceleration(double force){  // ToDo
}

void Servo::setVelocity(double vel){
  CM730 *cm730 = getRobot()->getCM730();
  int value = fabs(MX28::Angle2Value(vel*180.0/M_PI));  // Need to be verified
  cm730->WriteWord(mNamesToIDs[getName()], MX28::P_MOVING_SPEED_L, value, 0);
}

void Servo::enablePosition(int ms){  //EMPTY
}

void Servo::disablePosition(){  //EMPTY
}

void Servo::setForce(double force){  // NotFinished !!!
  CM730 *cm730 = getRobot()->getCM730();
  if(force == 0)
	{cm730->WriteWord(mNamesToIDs[getName()], MX28::P_TORQUE_ENABLE, 0, 0);}
  else
  {
	cm730->WriteWord(mNamesToIDs[getName()], MX28::P_TORQUE_ENABLE, 1, 0);
	//this->setMotorForce(force);
  }
}

void Servo::setMotorForce(double motor_force){
  CM730 *cm730 = getRobot()->getCM730();
  if(motor_force > 2.5)
	{cm730->WriteWord(mNamesToIDs[getName()], MX28::P_TORQUE_LIMIT_L, 1023, 0);}
  else if (motor_force  >=  0)
  {
	  double value = (motor_force/2.5) * 1023;
	  cm730->WriteWord(mNamesToIDs[getName()], MX28::P_TORQUE_LIMIT_L, value, 0);
  }
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
    cm730->WriteWord(mNamesToIDs[getName()], MX28::P_GOAL_POSITION_L, value, &error);
  }
}
