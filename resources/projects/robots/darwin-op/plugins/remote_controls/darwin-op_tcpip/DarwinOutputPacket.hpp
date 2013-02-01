/*
 * File:         DarwinOutputPacket.hpp
 * Date:         January 2013
 * Description:  Defines a packet sending from the remote control library to the DARwIn-OP
 * Author:       david.mansolino@epfl.ch
 * Modifications:
 */

#ifndef DARWIN_OUTPUT_PACKET_HPP
#define DARWIN_OUTPUT_PACKET_HPP

#include "Packet.hpp"

class Device;

class DarwinOutputPacket : public Packet {
public:
  DarwinOutputPacket();
  virtual ~DarwinOutputPacket();

  virtual void clear();

  int answerSize() const { return mAnswerSize; }

  void apply(int simulationTime);

  bool isAccelerometerRequested() const { return mAccelerometerRequested; }
  bool isGyroRequested() const { return mGyroRequested; }
  bool isCameraRequested() const { return mCameraRequested; }
  bool isServoPositionFeedback(int at) const { return mServoPositionFeedback[at]; }
  bool isServoForceFeedback(int at) const { return mServoForceFeedback[at]; }

private:
  int mAnswerSize;

  bool mAccelerometerRequested;
  bool mGyroRequested;
  bool mCameraRequested;
  bool mServoPositionFeedback[20];
  bool mServoForceFeedback[20];
};

#endif
