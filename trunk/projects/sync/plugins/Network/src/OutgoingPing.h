#ifndef PingHost_h__
#define PingHost_h__

#include "BaseJob.h"

#include <boost/scoped_ptr.hpp>

//! Outgoing ping job
//!
//! \class COutgoingPing
//!
class COutgoingPing :
	public CBaseJob
{
public:
	COutgoingPing(IKernel& kernel, ILog& logger);

	//! Invoke job
	void						Invoke(const TableList& params, const std::string& host);

	//! Execute job
	void						Execute(const ProtoPacketPtr packet);

};

#endif // PingHost_h__
