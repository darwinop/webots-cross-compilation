#include "DarwinInputPacket.hpp"

#include "Camera.hpp"
#include "Device.hpp"
#include "DeviceManager.hpp"
#include "DarwinOutputPacket.hpp"
#include "Sensor.hpp"
#include "Servo.hpp"
#include "TripleValuesSensor.hpp"
#include "SingleValueSensor.hpp"

#include <webots/remote_control.h>

#include <iostream>

#include <cstdlib>

#include <QtCore/QtCore>


using namespace std;

DarwinInputPacket::DarwinInputPacket(int maxSize) :
  Packet(maxSize)
{
  CameraR *camera = DeviceManager::instance()->camera();
  mCameraWidth = camera->width();
  mCameraHeight = camera->height();
}

DarwinInputPacket::~DarwinInputPacket() {
}

void DarwinInputPacket::decode(int simulationTime, const DarwinOutputPacket &outputPacket) {
  // ---
  // Sensors
  // ---
  
  // the order of the sensors should match with DarwinOutputPacket::apply()
 
  int currentPos = 5;

  // Accelerometer
  if(outputPacket.isAccelerometerRequested()) {
    double values[3];
    for(int i=0; i<3; i++) {
      values[i] = (double)readIntAt(currentPos);
      currentPos += 4;
    }
    TripleValuesSensor *accelerometer = DeviceManager::instance()->accelerometer();
    wbr_accelerometer_set_values(accelerometer->tag(), values);
    accelerometer->resetSensorRequested();
  }
  
  // Gyro
  if(outputPacket.isGyroRequested()) {
    double values[3];
    for(int i=0; i<3; i++) {
      values[i] = (double)readIntAt(currentPos);
      currentPos += 4;
    }
    TripleValuesSensor *gyro = DeviceManager::instance()->gyro();
    wbr_gyro_set_values(gyro->tag(), values);
    gyro->resetSensorRequested();
  }

  // Camera
  if(outputPacket.isCameraRequested()) {
	  
	int image_length = readIntAt(currentPos);
	currentPos += 4;
	
    CameraR *camera = DeviceManager::instance()->camera();

    static JSAMPARRAY buffer;
    
    static struct jpeg_decompress_struct cinfo;
    static struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_memory_src(&cinfo, (const JOCTET *)getBufferFromPos(currentPos), image_length); // set buffer source
    jpeg_read_header(&cinfo, true);
    jpeg_start_decompress(&cinfo);
    int row_stride = cinfo.output_width * cinfo.output_components;
    
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
    buffer[0] = (JSAMPROW)malloc(sizeof(JSAMPLE) * row_stride);
    
    int counter = 0;
    unsigned char imageRGB[mCameraWidth * mCameraHeight * 3];
    
    // Start decompression
    while (cinfo.output_scanline < cinfo.output_height) {
      jpeg_read_scanlines(&cinfo, buffer, 1);
      memcpy(imageRGB+counter, buffer[0], row_stride);
      counter += row_stride;
    }
    jpeg_finish_decompress(&cinfo);
    
    // Convert RGB buffer to BGRA buffer
    unsigned char imageBGRA[mCameraWidth * mCameraHeight * 4];
    for(int i = 0; i < mCameraHeight; i++) {
      for(int j = 0; j < mCameraWidth; j++) {
        imageBGRA[i * 4 * mCameraWidth + j * 4 + 0] = imageRGB[i * 3 * mCameraWidth + j * 3 + 2];
        imageBGRA[i * 4 * mCameraWidth + j * 4 + 1] = imageRGB[i * 3 * mCameraWidth + j * 3 + 1];
        imageBGRA[i * 4 * mCameraWidth + j * 4 + 2] = imageRGB[i * 3 * mCameraWidth + j * 3 + 0];
        imageBGRA[i * 4 * mCameraWidth + j * 4 + 3] = 255;
      }
    }
    wbr_camera_set_image(camera->tag(), (const unsigned char *)imageBGRA);
    camera->resetSensorRequested();
    currentPos += image_length;
  }
  
  // Servo position feedback
  for(int i=0; i<20; i++) {
    if(outputPacket.isServoPositionFeedback(i)) {
      double value = (double)readIntAt(currentPos)  / 10000;
      currentPos += 4;
      ServoR *servo = DeviceManager::instance()->servo(i);
      wbr_servo_set_position_feedback(servo->tag(), value);
      servo->resetSensorRequested();
    }
  }
  
  // Servo force feedback
  for(int i=0; i<20; i++) {
    if(outputPacket.isServoForceFeedback(i)) {
      double value = (double)readIntAt(currentPos) / 10000;
      currentPos += 4;
      SingleValueSensor *servoForceFeedback = DeviceManager::instance()->servoForceFeedback(i);
      wbr_servo_set_motor_force_feedback(servoForceFeedback->tag(), value);
      servoForceFeedback->resetSensorRequested();
    }
  }
  
}

// *** Jpeg part *** //

void DarwinInputPacket::init_source(j_decompress_ptr cinfo) {
  (void)cinfo;
}

boolean DarwinInputPacket::fill_input_buffer(j_decompress_ptr cinfo) {
  my_src_ptr src = (my_src_ptr) cinfo->src;

  WARNMS(cinfo, JWRN_JPEG_EOF);

  // Create a fake EOI marker 
  src->eoi_buffer[0] = (JOCTET) 0xFF;
  src->eoi_buffer[1] = (JOCTET) JPEG_EOI;
  src->pub.next_input_byte = src->eoi_buffer;
  src->pub.bytes_in_buffer = 2;

  return TRUE;
}

void DarwinInputPacket::skip_input_data(j_decompress_ptr cinfo, long num_bytes) {
  my_src_ptr src = (my_src_ptr) cinfo->src;

  if (num_bytes > 0) {
    while (num_bytes > (long) src->pub.bytes_in_buffer) {
      num_bytes -= (long) src->pub.bytes_in_buffer;
      (void) fill_input_buffer(cinfo);

    }
    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
  }
}

void DarwinInputPacket::term_source(j_decompress_ptr cinfo) {
  // no work necessary here 
  (void)cinfo;
}

void DarwinInputPacket::jpeg_memory_src(j_decompress_ptr cinfo, const JOCTET * buffer, size_t bufsize) {
  my_src_ptr src;

  if (cinfo->src == NULL) { // first time for this JPEG object? 
    cinfo->src = (struct jpeg_source_mgr *) (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(my_source_mgr));
  }

  src = (my_src_ptr) cinfo->src;
  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; // use default method
  src->pub.term_source = term_source;

  src->pub.next_input_byte = buffer;
  src->pub.bytes_in_buffer = bufsize;
}
