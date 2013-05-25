#ifndef ProtoPacketPtr_h__
#define ProtoPacketPtr_h__

#include <boost/shared_ptr.hpp>

//! Forward declarations
namespace packets
{
	class Packet;
}

//! Protobufer table pointer type
typedef boost::shared_ptr<packets::Packet> ProtoPacketPtr;


#endif // ProtoPacketPtr_h__
