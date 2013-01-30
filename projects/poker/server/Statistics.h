#ifndef Statistics_h__
#define Statistics_h__

#include "ILog.h"
#include "Parser.h"

namespace srv
{

//! Stats writer/reader
class Statistics
{
public:

	Statistics(ILog& logger);
	~Statistics();

	//! Write statistics data
	void Write(Parser::Data& data);

private:
	class Impl;
	Impl* m_Impl;

};

} // namespace srv

#endif // Statistics_h__
