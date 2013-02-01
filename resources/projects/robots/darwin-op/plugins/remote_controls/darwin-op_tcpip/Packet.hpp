/*
 * File:          Packet.hpp
 * Date:          June 2012
 * Description:   Defines a packet
 * Author:        fabien.rohrer@cyberbotics.com
 * Modifications: January 2013 adapted to DARwIn-OP robot by david.mansolino@epfl.ch
 */

#ifndef PACKET_HPP
#define PACKET_HPP

#include <QtCore/QtCore>

class Packet {
public:
  Packet(int maxSize);
  virtual ~Packet();

  QByteArray *data() const { return mData; }

  virtual void clear() { mData->clear(); }
  int size() const { return mData->size(); }
  int maxSize() const { return mMaxSize; }

  void append(QByteArray data);
  void appendINT(int value);


  int readIntAt(int pos);
  const unsigned char * getBufferFromPos(int pos);

private:
  int mMaxSize;
  QByteArray *mData;
};

#endif
