#ifndef Connect_h__
#define Connect_h__

#include "BaseJob.h"

#include <boost/scoped_ptr.hpp>

//! Outgoing connection establishment job
//!
//! \class COutgoingPing
//!
class CConnect :
	public CBaseJob
{
public:
	CConnect(IKernel& kernel, ILog& logger);

	//! Invoke job
	void						Invoke(const TableList& params, const std::string& host);

	//! Execute job
	void						Execute(const ProtoPacketPtr packet);

};

#endif // Connect_h__
