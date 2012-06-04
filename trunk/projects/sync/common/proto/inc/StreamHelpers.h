#ifndef StreamHelpers_h__
#define StreamHelpers_h__

#include "data.pb.h"
#include "packet.pb.h"
#include "job.pb.h"

#include <iosfwd>

//! Data table stream operator
std::ostream& operator << (std::ostream& stream, const data::Table& object);

//! Data packet stream operator
std::ostream& operator << (std::ostream& stream, const packets::Packet& object);

//! Job stream operator
std::ostream& operator << (std::ostream& stream, const jobs::Job& object);

#endif // StreamHelpers_h__
