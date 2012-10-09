#include <webots/Camera.hpp>
#include <webots/Robot.hpp>

#include <LinuxDARwIn.h>


::webots::Camera::Camera(const std::string &name, const Robot *robot) :
  Device(name, robot),
  mImage(NULL)
{
}

::webots::Camera::~Camera() {
  disable();
}

void ::webots::Camera::enable(int ms) {
  disable();
  ::Robot::LinuxCamera::GetInstance()->Initialize(0);
  ::Robot::LinuxCamera::GetInstance()->SetCameraSettings(::Robot::CameraSettings());
  mImage = (unsigned char *) malloc (3*getWidth()*getHeight());
}

void ::webots::Camera::disable() {
  if (mImage) {
    free (mImage);
    mImage = NULL;
  }
}

const unsigned char *::webots::Camera::getImage() const {
  ::Robot::LinuxCamera::GetInstance()->CaptureFrame();
  memcpy(mImage, ::Robot::LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageData, ::Robot::LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageSize);
  return mImage;
}

int ::webots::Camera::getWidth() const {
  return 320;
}

int ::webots::Camera::getHeight() const {
  return 240;
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

