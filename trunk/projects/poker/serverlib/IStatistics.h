#ifndef IStatistics_h__
#define IStatistics_h__

#include "ILog.h"
#include "TableContext.h"

namespace srv
{

//! Stats writer/reader
class IStatistics
{
public:

	virtual ~IStatistics() {}

	//! Write statistics data
	virtual void Write(pcmn::TableContext::Data& data) = 0;

private:
	class Impl;
	Impl* m_Impl;

};

} // namespace srv

#endif // IStatistics_h__
