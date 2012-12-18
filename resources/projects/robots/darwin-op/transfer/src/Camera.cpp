#include <webots/Camera.hpp>
#include <webots/Robot.hpp>

#include <LinuxDARwIn.h>
#include "Camera.h"

unsigned char *::webots::Camera::mImage = NULL;
const int ::webots::Camera::mResolution[NBRESOLUTION][2] = { {320, 240}, {640, 360}, {640, 400}, {640, 480}, {768, 480}, {800, 600} };

::webots::Camera::Camera(const std::string &name, const Robot *robot) :
  Device(name, robot)
{
  mIsActive = false;
}

::webots::Camera::~Camera() {
  disable();
}

void ::webots::Camera::enable(int ms) {
  disable();
  ::Robot::LinuxCamera::GetInstance()->Initialize(0);
  ::Robot::LinuxCamera::GetInstance()->SetCameraSettings(::Robot::CameraSettings());
  mImage = (unsigned char *) malloc (4*getWidth()*getHeight());

  int error = 0;

  // create and start the thread
  if((error = pthread_create(&this->mCameraThread, NULL, this->CameraTimerProc, this))!= 0) {
    printf("Camera thread error = %d\n",error);
    exit(-1);
  }

  mIsActive = true;
}

void ::webots::Camera::disable() {
  if(mIsActive){
    int error=0;
    // End the thread
    if((error = pthread_cancel(this->mCameraThread))!= 0)
      exit(-1);
    mIsActive = false;
  }
  if(mImage) {
    free(mImage);
    mImage = NULL;
  }
}

const unsigned char *::webots::Camera::getImage() const {
  return mImage;
}

void *::webots::Camera::CameraTimerProc(void *param) {
  while(1) {
    ::Robot::LinuxCamera::GetInstance()->CaptureFrameWb();
    memcpy(mImage, ::Robot::LinuxCamera::GetInstance()->fbuffer->m_BGRAFrame->m_ImageData, ::Robot::LinuxCamera::GetInstance()->fbuffer->m_BGRAFrame->m_ImageSize);
  }
  return NULL;
}

int ::webots::Camera::getWidth() const {
  return ::Robot::Camera::WIDTH;
}

int ::webots::Camera::getHeight() const {
  return ::Robot::Camera::HEIGHT;
}

double ::webots::Camera::getFov() const {
  return 1.0123;
}

double ::webots::Camera::getNear() const {
  return 0.0;
}

unsigned char ::webots::Camera::imageGetRed(const unsigned char *image, int width, int x, int y) {
  return image[3*(y*width + x)];
}

unsigned char ::webots::Camera::imageGetGreen(const unsigned char *image, int width, int x, int y) {
  return image[3*(y*width + x) + 1];
}

unsigned char ::webots::Camera::imageGetBlue(const unsigned char *image, int width, int x, int y) {
  return image[3*(y*width + x) + 2];
}

unsigned char ::webots::Camera::imageGetGrey(const unsigned char *image, int width, int x, int y) {
  return image[3*(y*width + x)] / 3 + image[3*(y*width + x) + 1] / 3  + image[3*(y*width + x) + 2] / 3;
}

bool ::webots::Camera::checkResolution(int width, int height) {
  for(int i=0; i < NBRESOLUTION; i++) {
    if( (mResolution[i][0] == width) && (mResolution[i][1] == height) )
      return true;
  }
  return false;
}

int ::webots::Camera::getSamplingPeriod() {
  if(getRobot()->getBasicTimeStep() < 30)
    return 30;
  else
    return getRobot()->getBasicTimeStep();
}

int ::webots::Camera::getType() const {
  return WB_CAMERA_COLOR;
}


