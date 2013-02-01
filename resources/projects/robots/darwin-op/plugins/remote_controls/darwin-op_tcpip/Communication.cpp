#include "Communication.hpp"
#include "Packet.hpp"

#include <QtNetwork/QtNetwork>
#include <QtCore/QtCore>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <cstdlib>
#include <cstring>

using namespace std;

Communication::Communication() :
  mInitialized(false)
{
  socket = new QTcpSocket();
}

Communication::~Communication() {
  cleanup();
}

bool Communication::initialize(QString IP, int port) {
  socket->abort(); // Close previous connection (if any)
  socket->connectToHost((char*)IP.toStdString().c_str(), port);
  mInitialized = true;
  if(!(socket->isOpen()))
    return false;
  else
    return true;
}

void Communication::cleanup() {
  socket->abort();
}

bool Communication::sendPacket(const Packet *packet) {
  
  if(!(socket->isOpen())) {
    cerr << "SOCKET NOT OPEN" << endl;
    return false;
  }
  
  if(!(socket->isWritable())) {
    cerr << "SOCKET NOT WRITABLE" << endl;
    return false;
  }
  
  if(socket->write(packet->data()->constData(), packet->size()) == -1) {
    cerr << "WRITING ERROR" << endl;
    return false;
  }
  
  if(socket->waitForBytesWritten(-1) == -1) {
    cerr << "BYTES NOT WRITTEN" << endl;
    return false;
  }
  
  return true;
}

bool Communication::receivePacket(Packet *packet) {

  packet->clear();
  
  if(socket->waitForReadyRead(-1) == -1) {
    cerr << "RECEIVING PACKET NOT READABLE" << endl;
    return false;
  }

  socket->setReadBufferSize(0);

  while(socket->bytesAvailable() < 5) // Wait until at least W + packet size has been transmitted
    {usleep(100);}
  
  QByteArray startPacket;
  startPacket = socket->readAll();
  if(startPacket[0] != 'W')  // if packet do not start by 'W' -> skip this packet
    return receivePacket(packet);
  packet->append(startPacket);  // Read beginning of the packet
    
  int packet_size = packet->readIntAt(1); // extract packet size from the beginning of the packet
  
  while(packet->size() < packet_size) {
    socket->waitForReadyRead(-1);
    packet->append(socket->readAll()); 
  }

  return true;
}

