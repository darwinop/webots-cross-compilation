#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

#include "remote.hpp"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "/darwin/Linux/build/streamer/jpeg_utils.h"
#include "Image.h"

#define PORT 5023

using namespace webots;

void writeINT2Buffer(char * buffer, int value);
int readINTFromBuffer(char * buffer);

int main(int argc, char *argv[]) {
  int erreur = 0;
  int cameraWidthZoomFactor = 1;
  int cameraHeightZoomFactor = 1;
  sscanf(argv[1], "%d", &cameraWidthZoomFactor);
  sscanf(argv[2], "%d", &cameraHeightZoomFactor);
  
  // Server socket
  SOCKADDR_IN sin;
  SOCKET sock;
  socklen_t recsize = sizeof(sin);
    
  // Client socket
  SOCKADDR_IN csin;
  SOCKET csock;
  socklen_t crecsize = sizeof(csin);
    
  int sock_err;

  if(!erreur) {
    // Creation of the socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // If  socket is valid
    if(sock != INVALID_SOCKET) {
      printf("The socket %d is now open in mode TCP/IP\n", sock);
      
      // Configuration
      sin.sin_addr.s_addr = htonl(INADDR_ANY);  // Adresse IP automatic
      sin.sin_family = AF_INET;                 // Protocole familial (IP)
      sin.sin_port = htons(PORT);               // Listening of the port
      sock_err = bind(sock, (SOCKADDR*)&sin, recsize);
      
      // If socket works
      if(sock_err != SOCKET_ERROR) {
        // Starting port Listening (server mode)
        sock_err = listen(sock, 1); // only one connexion 
        printf("Listening of port %d...\n", PORT);

        // If socket works
        if(sock_err != SOCKET_ERROR) {
          // Wait until a client connect
          printf("Please wait until client connect to port  %d...\n", PORT);

          csock = accept(sock, (SOCKADDR*)&csin, &crecsize);
          printf("Client is connecting with socket %d of %s:%d\n", csock, inet_ntoa(csin.sin_addr), htons(csin.sin_port));
          }
          else
             perror("listen");
      }
      else
        perror("bind");
      
      Remote * remote = new Remote();
      remote->remoteStep();
      const double *acc;
      const double *gyro;
      const unsigned char *image;

      char receiveBuffer[1024] = "";
      char sendBuffer[350000] = "";

      Image rgbImage((320/cameraWidthZoomFactor), (240/cameraHeightZoomFactor), Image::RGB_PIXEL_SIZE);
      unsigned char jpeg_buffer[rgbImage.m_ImageSize];
      
      int c = 0;

      while(1) {
        // Wait for message
        do {  // If message do not start by 'W' this is a wrong message, skip it
          recv(csock, receiveBuffer, sizeof(receiveBuffer), 0);
        } while (receiveBuffer[0] != 'W');
        
        int receivePos = 1, sendPos = 5;

        // *** SENSORS *** //

        // Accelerometer
        if(receiveBuffer[receivePos] == 'A') {
          acc = remote->getRemoteAccelerometer();
          for(c = 0; c<3; c++) 
            writeINT2Buffer(sendBuffer + 4 * c + sendPos, (int)acc[c]);
          sendPos += 12;
          receivePos++;
        }

        // Gyro
        if(receiveBuffer[receivePos] == 'G') {
          gyro = remote->getRemoteGyro();
          for(c = 0; c<3; c++)
            writeINT2Buffer(sendBuffer + 4 * c + sendPos, (int)gyro[c]);
          sendPos += 12;
          receivePos++;
        }

        // Camera
        if(receiveBuffer[receivePos] == 'C') {
          image = remote->getRemoteImage();
          int image_buffer_position = 0;

          for(int height = 120 - (120 / cameraHeightZoomFactor) ; height < 120 + (120 / cameraHeightZoomFactor); height++) {
            for(int width = 160 - (160 / cameraWidthZoomFactor) ; width < 160 + (160 / cameraWidthZoomFactor); width++) {
              rgbImage.m_ImageData[image_buffer_position  + 2] = image[height * 320 * 4 + width * 4 + 0];
              rgbImage.m_ImageData[image_buffer_position  + 1] = image[height * 320 * 4 + width * 4 + 1];
              rgbImage.m_ImageData[image_buffer_position  + 0] = image[height * 320 * 4 + width * 4 + 2];
              image_buffer_position += 3;
            }
          }
          
          // Compress image to jpeg
          int buffer_length = 0;
          if(cameraHeightZoomFactor * cameraWidthZoomFactor < 2) // -> resolution 320x240 -> put quality at 65%
            buffer_length = jpeg_utils::compress_rgb_to_jpeg(&rgbImage, jpeg_buffer, rgbImage.m_ImageSize, 65);
          else // image smaller, put quality at 80%
            buffer_length = jpeg_utils::compress_rgb_to_jpeg(&rgbImage, jpeg_buffer, rgbImage.m_ImageSize, 80);

          writeINT2Buffer(sendBuffer + sendPos, buffer_length); // write image_buffer length 
          sendPos += 4;

          memcpy(sendBuffer + sendPos, jpeg_buffer, buffer_length); // write image
          sendPos += buffer_length;

          receivePos++;
        }

        // *** ACTUATORS *** //

        // LEDs
        for(c = 0; c<5; c++) {
          if(receiveBuffer[receivePos] == 'L') {
            unsigned char c1 = static_cast <unsigned char> (receiveBuffer[receivePos+4]);
            unsigned char c2 = static_cast <unsigned char> (receiveBuffer[receivePos+3]);
            unsigned char c3 = static_cast <unsigned char> (receiveBuffer[receivePos+2]);
            int value = c1 + (c2 << 8) + (c3 << 16);
            remote->setRemoteLED(receiveBuffer[receivePos+1], value);
            receivePos += 5;
          }
        }

        // Servos Actuator
        for(c = 0; c<20; c++) {
          if(receiveBuffer[receivePos] == 'S') {
            int servoNumber = (int)receiveBuffer[receivePos+1];
            receivePos+= 2;
            if(receiveBuffer[receivePos] == 'p') { // Position
              int value = readINTFromBuffer(receiveBuffer + receivePos + 1);
              remote->setRemoteServoPosition(servoNumber, value);
              receivePos += 5;
            }
            if(receiveBuffer[receivePos] == 'v') { // Velocity
              int value = readINTFromBuffer(receiveBuffer + receivePos + 1);
              remote->setRemoteServoVelocity(servoNumber, value);
              receivePos += 5;
            }
            if(receiveBuffer[receivePos] == 'a') { // Acceleration
              int value = readINTFromBuffer(receiveBuffer + receivePos + 1);
              remote->setRemoteServoAcceleration(servoNumber, value);
              receivePos += 5;
            }
            if(receiveBuffer[receivePos] == 'm') { // MotorForce
              int value = readINTFromBuffer(receiveBuffer + receivePos + 1);
              remote->setRemoteServoMotorForce(servoNumber, value);
              receivePos += 5;
            }
            if(receiveBuffer[receivePos] == 'c') { // ControlP
              int value = readINTFromBuffer(receiveBuffer + receivePos + 1);
              remote->setRemoteServoControlP(servoNumber, value);
              receivePos += 5;
            }
            if(receiveBuffer[receivePos] == 'f') { // Force
              int value = readINTFromBuffer(receiveBuffer + receivePos + 1);
              remote->setRemoteServoForce(servoNumber, value);
              receivePos += 5;
            }
          }
        }
        
        // Servos sensors Position
        for(c = 0; c<20; c++) {
          if(receiveBuffer[receivePos] == 'P') {
            if((int)receiveBuffer[receivePos+1] < 20) {
              double servoPosition = remote->getRemoteServoPosition((int)receiveBuffer[receivePos+1]);
              writeINT2Buffer(sendBuffer + sendPos, (int)servoPosition);
            }
            else
              writeINT2Buffer(sendBuffer + sendPos, 0);
            sendPos += 4;
            receivePos += 2;
          }
        }

        // Servos sensors Force
        for(c = 0; c<20; c++) {
          if(receiveBuffer[receivePos] == 'F') {
            if((int)receiveBuffer[receivePos+1] < 20) {
              double servoTorque = remote->getRemoteServoForce((int)receiveBuffer[receivePos+1]);
              writeINT2Buffer(sendBuffer + sendPos, (int)servoTorque);
            }
            else
              writeINT2Buffer(sendBuffer + sendPos, 0);
            sendPos += 4;
            receivePos += 2;
          }
        }

        // Terminate the buffer and send it
        sendBuffer[0] = 'W';
        writeINT2Buffer(sendBuffer+1, sendPos); // Write size of buffer at the beginning
        sendBuffer[sendPos] = '\0';
        send(csock, sendBuffer, sendPos+1, 0);
        remote->remoteStep();
      }

      // Close client socket and server socket
      printf("Closing client socket\n");
      closesocket(csock);
      printf("Closing server socket\n");
      closesocket(sock);
    }
    else
      perror("socket");
  }
  return EXIT_SUCCESS;
}

void writeINT2Buffer(char * buffer, int value) {
  buffer[0] = value >> 24;
  buffer[1] = (value >> 16) & 0xFF;
  buffer[2] = (value >> 8) & 0xFF;
  buffer[3] = value & 0xFF;
}

int readINTFromBuffer(char * buffer) {
  unsigned char c1 = static_cast <unsigned char> (buffer[3]);
  unsigned char c2 = static_cast <unsigned char> (buffer[2]);
  unsigned char c3 = static_cast <unsigned char> (buffer[1]);
  unsigned char c4 = static_cast <unsigned char> (buffer[0]);
  return (c1 + (c2 << 8) + (c3 << 16) + (c4 << 24));
}

