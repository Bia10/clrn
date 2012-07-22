#include "StreamHelpers.h"

std::ostream& operator << (std::ostream& stream, const data::Table& object)
{
	stream << object.ShortDebugString();
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const jobs::Job& object)
{
	stream << object.ShortDebugString();
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const packets::Packet& object)
{
	stream << object.ShortDebugString();
	return stream;
}
