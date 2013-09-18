/*
 * File:         DarwinInputPacket.hpp
 * Date:         January 2013
 * Description:  Defines a packet sending from the real DARwIn-OP to the remote control library
 * Author:       david.mansolino@epfl.ch
 * Modifications:
 */

#ifndef DARWIN_INPUT_PACKET_HPP
#define DARWIN_INPUT_PACKET_HPP

#include "Packet.hpp"

class DarwinOutputPacket;

class DarwinInputPacket : public Packet {
  public:
             DarwinInputPacket(int maxSize);
    virtual ~DarwinInputPacket();

    void     decode(int simulationTime, const DarwinOutputPacket &outputPacket);
    
  private:
    int      mCameraWidth;
    int      mCameraHeight;
};

#endif
