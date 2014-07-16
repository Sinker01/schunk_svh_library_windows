// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

// -- BEGIN LICENSE BLOCK ----------------------------------------------
// -- END LICENSE BLOCK ------------------------------------------------

//----------------------------------------------------------------------
/*!\file
 *
 * \author  Georg Heppner
 * \date    2014-02-10
 * \date    2014-07-16
 *
 * This file contains the S5FHSerialPacket data structure that is used to
 * send and receive everything from an to the actual hardware.
 * The serial packet is used to wrap up the payload data for convenient handling.
 * By wrapping everything in the same packet structure it can be handled quite neatly
 */
//----------------------------------------------------------------------
#include <driver_s5fh/S5FHSerialPacket.h>

namespace driver_s5fh {

icl_comm::ArrayBuilder& operator << (icl_comm::ArrayBuilder& ab, const S5FHSerialPacket& data)
{
  ab << data.index << data.address << static_cast<u_int16_t>(data.data.size()) << data.data;
  return ab;
}

icl_comm::ArrayBuilder& operator >> (icl_comm::ArrayBuilder& ab, S5FHSerialPacket& data)
{
  // Disregard the size when deserializing as we get that anyway
  u_int16_t size ;
   ab >> data.index >> data.address >> size >> data.data;
  return ab;
}

std::ostream& operator << (std::ostream& o, const S5FHSerialPacket& sp)
{
  o << "index: " << static_cast<int>(sp.index) << " address: " << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(sp.address) << " Data: ";
  for (size_t i = 0; i < sp.data.size(); i++)
  {
    o << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(sp.data[i]) << " ";
  }
  // Reset Output stream to decimal output .. otherwise it may confuse people and the stream operators have the tendency to hang on to these hints
  std::cout << std::dec ;
  return o;
}

}

