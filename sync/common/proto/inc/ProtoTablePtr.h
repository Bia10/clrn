#ifndef ProtoTablePtr_h__
#define ProtoTablePtr_h__

#include <boost/shared_ptr.hpp>

//! Forward declarations
namespace data
{
	class Table;
}

//! Protobufer table pointer type
typedef boost::shared_ptr<data::Table> ProtoTablePtr;

#endif // ProtoTablePtr_h__
