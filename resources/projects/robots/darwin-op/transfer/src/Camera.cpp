#include <webots/Camera.hpp>
#include <webots/Robot.hpp>

#include <LinuxDARwIn.h>
#include "Camera.h"

unsigned char *::webots::Camera::mImage = NULL;

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
  mImage = (unsigned char *) malloc (3*getWidth()*getHeight());

//Thread start
  int error;
  struct sched_param param;
  pthread_attr_t attr;
  
  pthread_attr_init(&attr);
  
  error = pthread_attr_setschedpolicy(&attr, SCHED_RR);
  if(error != 0)
    printf("error = %d\n",error);
  error = pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);
  if(error != 0)
    printf("error = %d\n",error);
  
  memset(&param, 0, sizeof(param));
  param.sched_priority = 31;// RT
  error = pthread_attr_setschedparam(&attr, &param);
  if(error != 0)
    printf("error = %d\n",error);

  // create and start the thread
  if((error = pthread_create(&this->mCameraThread, &attr, this->CameraTimerProc, this))!= 0)
    exit(-1);

  mIsActive = true;
}

void ::webots::Camera::disable() {
  if(mIsActive){
    int error=0;
    // wait for the thread to end
    if((error = pthread_join(this->mCameraThread, NULL))!= 0)
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
    ::Robot::LinuxCamera::GetInstance()->CaptureFrame();
    memcpy(mImage, ::Robot::LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageData, ::Robot::LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageSize);
  }
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

int ::webots::Camera::getType() const {
  return 'c';
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

