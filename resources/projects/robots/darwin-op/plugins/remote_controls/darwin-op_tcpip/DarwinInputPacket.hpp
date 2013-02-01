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

#include <jpeglib.h>
#include <jerror.h>

class DarwinOutputPacket;

class DarwinInputPacket : public Packet {
public:
  DarwinInputPacket(int maxSize);
  virtual ~DarwinInputPacket();

  void decode(int simulationTime, const DarwinOutputPacket &outputPacket);
  
private:

  int mCameraWidth;
  int mCameraHeight;

  // All this function are used for decompressing the JPEG image.
  // Particularly to change the source of the image buffer.
  // (The standard source with libjpeg is a file)
  static void init_source(j_decompress_ptr cinfo);
  static boolean fill_input_buffer(j_decompress_ptr cinfo);
  static void skip_input_data(j_decompress_ptr cinfo, long num_bytes);
  static void term_source(j_decompress_ptr cinfo);
  static void jpeg_memory_src(j_decompress_ptr cinfo, const JOCTET * buffer, size_t bufsize);
  
  typedef struct {
    struct jpeg_source_mgr pub; // public fields
    JOCTET eoi_buffer[2];       // a place to put a dummy EOI
  } my_source_mgr;
  
  typedef my_source_mgr * my_src_ptr;
  
};

#endif
