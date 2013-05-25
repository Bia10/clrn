#ifndef ProtoJobPtr_h__
#define ProtoJobPtr_h__

#include <boost/shared_ptr.hpp>

//! Forward declarations
namespace jobs
{
	class Job;
	enum Job_JobId;
}

//! Protobufer table pointer type
typedef boost::shared_ptr<jobs::Job> ProtoJobPtr;

#endif // ProtoJobPtr_h__
