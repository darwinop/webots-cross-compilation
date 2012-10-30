#include <webots/Robot.hpp>
#include <webots/Accelerometer.hpp>
#include <webots/Camera.hpp>
#include <webots/Gyro.hpp>
#include <webots/LED.hpp>
#include <webots/Servo.hpp>
#include <webots/Device.hpp>
#include <webots/Speaker.hpp>
#include "LinuxDARwIn.h"

#include <unistd.h>

#include <libgen.h>


webots::Robot::Robot() {
  initDarwinOP();
  initDevices();
  gettimeofday(&mStart, NULL);
  mPreviousStepTime = 0.0;
  // Load TimeStep from the file "config.ini"
  minIni ini("config.ini");
  LoadINISettings(&ini, "Robot Config");
  if(mTimeStep < 16)
    printf("The time step selected of %dms is very small and will probably not be respected.\n A time step of at least 16ms is recommended.\n", mTimeStep);
    
  getCM730()->MakeBulkReadPacket(); // Create the BulkReadPacket to read the actuators states in Robot::step
}

webots::Robot::~Robot() {
}

int webots::Robot::step(int ms) {
  double actualTime = getTime() * 1000;
  int stepDuration = actualTime - mPreviousStepTime;
  std::map<const std::string, int>::iterator servo_it;
  
  //      Update speed of each servos,      //
  // according to acceleration limit if set //
  for(servo_it = Servo::mNamesToIDs.begin() ; servo_it != Servo::mNamesToIDs.end(); servo_it++  )
    ((Servo *) mDevices[(*servo_it).first])->updateSpeed(stepDuration);
  
  // Read table from CM730 regarding body sensors //
  //   read table from:  P_GYRO_Z_L  38 (0x26)    //
  //         to: P_ACCEL_Z_H   49 (0x31)          //
  int values[3];
  unsigned char mControlTable[::Robot::CM730::MAXNUM_ADDRESS];

  if(getCM730()->ReadTable(::Robot::CM730::ID_CM, ::Robot::CM730::P_GYRO_Z_L, ::Robot::CM730::P_VOLTAGE, mControlTable ,0) == ::Robot::CM730::SUCCESS) {
    values[2] = ::Robot::CM730::MakeWord(mControlTable[::Robot::CM730::P_GYRO_Z_L], mControlTable[::Robot::CM730::P_GYRO_Z_H]);
    values[1] = ::Robot::CM730::MakeWord(mControlTable[::Robot::CM730::P_GYRO_Y_L], mControlTable[::Robot::CM730::P_GYRO_Y_H]);
    values[0] = ::Robot::CM730::MakeWord(mControlTable[::Robot::CM730::P_GYRO_X_L], mControlTable[::Robot::CM730::P_GYRO_X_H]);
    ((Gyro *)mDevices["Gyro"])->setValues(values);
    values[0] = ::Robot::CM730::MakeWord(mControlTable[::Robot::CM730::P_ACCEL_X_L], mControlTable[::Robot::CM730::P_ACCEL_X_H]);
    values[1] = ::Robot::CM730::MakeWord(mControlTable[::Robot::CM730::P_ACCEL_Y_L], mControlTable[::Robot::CM730::P_ACCEL_Y_H]);
    values[2] = ::Robot::CM730::MakeWord(mControlTable[::Robot::CM730::P_ACCEL_Z_L], mControlTable[::Robot::CM730::P_ACCEL_Z_H]);
    ((Accelerometer *)mDevices["Accelerometer"])->setValues(values);
  } 
  
  // -------- Sync Write to actuators --------
  const int msgLength = 9; // id + P + Empty + Goal Position (L + H) + Moving speed (L + H) + Torque Limit (L + H)

  int param[20*msgLength];
  int n=0;
  int changed_servos=0;
  int value;
  
  for(servo_it = Servo::mNamesToIDs.begin() ; servo_it != Servo::mNamesToIDs.end(); servo_it++ ) {
    if(((Servo *) mDevices[(*servo_it).first])->getTorqueEnable()) {
      param[n++] = (*servo_it).second;
      param[n++] = ((Servo *) mDevices[(*servo_it).first])->getPGain();
      param[n++] = 0; // Empty
      value = ((Servo *) mDevices[(*servo_it).first])->getGoalPosition();
      param[n++] = ::Robot::CM730::GetLowByte(value);
      param[n++] = ::Robot::CM730::GetHighByte(value);
      value = ((Servo *) mDevices[(*servo_it).first])->getMovingSpeed();
      param[n++] = ::Robot::CM730::GetLowByte(value);
      param[n++] = ::Robot::CM730::GetHighByte(value);
      value = ((Servo *) mDevices[(*servo_it).first])->getTorqueLimit();
      param[n++] = ::Robot::CM730::GetLowByte(value);
      param[n++] = ::Robot::CM730::GetHighByte(value);
      changed_servos++;
    }
  }
  getCM730()->SyncWrite(::Robot::MX28::P_P_GAIN, msgLength, changed_servos , param);
  
  // -------- Bulk Read to read the actuators states (position, speed and load) -------- //
  getCM730()->BulkRead();

  for(servo_it = Servo::mNamesToIDs.begin() ; servo_it != Servo::mNamesToIDs.end(); servo_it++) {
    ((Servo *) mDevices[(*servo_it).first])->setPresentPosition( getCM730()->m_BulkReadData[(*servo_it).second].ReadWord(::Robot::MX28::P_PRESENT_POSITION_L));
    ((Servo *) mDevices[(*servo_it).first])->setPresentSpeed( getCM730()->m_BulkReadData[(*servo_it).second].ReadWord(::Robot::MX28::P_PRESENT_SPEED_L));
    ((Servo *) mDevices[(*servo_it).first])->setPresentLoad( getCM730()->m_BulkReadData[(*servo_it).second].ReadWord(::Robot::MX28::P_PRESENT_LOAD_L));
  }
  
  // Timing management //
  if(stepDuration < getBasicTimeStep()) { // Step to short -> wait remaining time
    usleep((getBasicTimeStep() - stepDuration) * 1000);
    mPreviousStepTime = actualTime;
    return 0;
  }
  else { // Step to long -> return step duration
    mPreviousStepTime = actualTime;
    return stepDuration;
  }
}

std::string webots::Robot::getName() const {
  return "darwin-op";
}

double webots::Robot::getTime() const {
  struct timeval end;
  
  gettimeofday(&end, NULL);

  long seconds  = end.tv_sec  - mStart.tv_sec;
  long useconds = end.tv_usec - mStart.tv_usec;
  long mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

  return (double) mtime/1000.0;
}

int webots::Robot::getMode() const {
  return 1;
}

double webots::Robot::getBasicTimeStep() const {
  return mTimeStep;
}

webots::Device *webots::Robot::getDevice(const std::string &name) const {
  std::map<const std::string, webots::Device *>::const_iterator it = mDevices.find(name);
  if (it != mDevices.end())
    return (*it).second;
  return NULL;

}

webots::Accelerometer *webots::Robot::getAccelerometer(const std::string &name) const {
  webots::Device *device = getDevice(name);
  if (device) {
    webots::Accelerometer *accelerometer = dynamic_cast<webots::Accelerometer *> (device);
    if (accelerometer)
      return accelerometer;
  }
  return NULL;
}

webots::Camera *webots::Robot::getCamera(const std::string &name) const {
  webots::Device *device = getDevice(name);
  if (device) {
    webots::Camera *camera = dynamic_cast<webots::Camera *> (device);
    if (camera)
      return camera;
  }
  return NULL;
}

webots::Gyro *webots::Robot::getGyro(const std::string &name) const {
  webots::Device *device = getDevice(name);
  if (device) {
    webots::Gyro *gyro = dynamic_cast<webots::Gyro *> (device);
    if (gyro)
      return gyro;
  }
  return NULL;
}

webots::Servo *webots::Robot::getServo(const std::string &name) const {
  webots::Device *device = getDevice(name);
  if (device) {
    webots::Servo *servo = dynamic_cast<webots::Servo *> (device);
    if (servo)
      return servo;
  }
  return NULL;
}

webots::LED *webots::Robot::getLED(const std::string &name) const {
  webots::Device *device = getDevice(name);
  if (device) {
    webots::LED *led = dynamic_cast<webots::LED *> (device);
    if (led)
      return led;
  }
  return NULL;
}

webots::Speaker *webots::Robot::getSpeaker(const std::string &name) const {
  webots::Device *device = getDevice(name);
  if (device) {
    webots::Speaker *speaker = dynamic_cast<webots::Speaker *> (device);
    if (speaker)
      return speaker;
  }
  return NULL;
}

void webots::Robot::initDevices() {
  mDevices["Accelerometer"] = new webots::Accelerometer("Accelerometer", this);
  mDevices["Camera"]        = new webots::Camera       ("Camera",        this);
  mDevices["Gyro"]          = new webots::Gyro         ("Gyro",          this);
  mDevices["EyeLed"]        = new webots::LED          ("EyeLed",        this);
  mDevices["HeadLed"]       = new webots::LED          ("HeadLed",       this);
  mDevices["BackLedRed"]    = new webots::LED          ("BackLedRed",    this);
  mDevices["BackLedGreen"]  = new webots::LED          ("BackLedGreen",  this);
  mDevices["BackLedBlue"]   = new webots::LED          ("BackLedBlue",   this);
  mDevices["ShoulderR"]     = new webots::Servo        ("ShoulderR",     this);
  mDevices["ShoulderL"]     = new webots::Servo        ("ShoulderL",     this);
  mDevices["ArmUpperR"]     = new webots::Servo        ("ArmUpperR",     this);
  mDevices["ArmUpperL"]     = new webots::Servo        ("ArmUpperL",     this);
  mDevices["ArmLowerR"]     = new webots::Servo        ("ArmLowerR",     this);
  mDevices["ArmLowerL"]     = new webots::Servo        ("ArmLowerL",     this);
  mDevices["PelvYR"]        = new webots::Servo        ("PelvYR",        this);
  mDevices["PelvYL"]        = new webots::Servo        ("PelvYL",        this);
  mDevices["PelvR"]         = new webots::Servo        ("PelvR",         this);
  mDevices["PelvL"]         = new webots::Servo        ("PelvL",         this);
  mDevices["LegUpperR"]     = new webots::Servo        ("LegUpperR",     this);
  mDevices["LegUpperL"]     = new webots::Servo        ("LegUpperL",     this);
  mDevices["LegLowerR"]     = new webots::Servo        ("LegLowerR",     this);
  mDevices["LegLowerL"]     = new webots::Servo        ("LegLowerL",     this);
  mDevices["AnkleR"]        = new webots::Servo        ("AnkleR",        this);
  mDevices["AnkleL"]        = new webots::Servo        ("AnkleL",        this);
  mDevices["FootR"]         = new webots::Servo        ("FootR",         this);
  mDevices["FootL"]         = new webots::Servo        ("FootL",         this);
  mDevices["Neck"]          = new webots::Servo        ("Neck",          this);
  mDevices["Head"]          = new webots::Servo        ("Head",          this);
  mDevices["Speaker"]       = new webots::Speaker      ("Speaker",       this);
}

void webots::Robot::initDarwinOP() {
	char exepath[1024] = {0};
  if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)  {
      if(chdir(dirname(exepath)))
          fprintf(stderr, "chdir error!! \n");
  }
  
  mLinuxCM730 = new ::Robot::LinuxCM730("/dev/ttyUSB0");
  mCM730 = new ::Robot::CM730(mLinuxCM730);
  
  if(mCM730->Connect() == false) {
    printf("Fail to connect CM-730!\n");
    exit(EXIT_FAILURE);
  }
  
  ::Robot::MotionManager::GetInstance()->Initialize(mCM730);
}

void webots::LoadINISettings(minIni* ini, const std::string &section) {
  double value = INVALID_VALUE;
  if((value = ini->getd(section, "time_step", INVALID_VALUE)) != INVALID_VALUE)
    mTimeStep = value;
}
