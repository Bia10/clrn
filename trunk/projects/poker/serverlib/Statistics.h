#ifndef Statistics_h__
#define Statistics_h__

#include "IStatistics.h"

namespace srv
{

//! Stats writer/reader
class Statistics : public IStatistics
{
public:

	Statistics(ILog& logger);
	~Statistics();

	//! Write statistics data
	void Write(pcmn::TableContext::Data& data);

private:
	class Impl;
	Impl* m_Impl;

};

} // namespace srv

#endif // Statistics_h__
