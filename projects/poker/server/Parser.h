#ifndef Parser_h__
#define Parser_h__

#include "ILog.h"
#include "packet.pb.h"
#include "TableContext.h"
#include "Modules.h"
#include "Logic.h"

#include <vector>

namespace srv
{

//! Stats packet parser
class Parser
{
public:

	Parser(ILog& logger, const net::Packet& packet, pcmn::IDecisionCallback& callback);
	~Parser();
	bool Parse(); // returns true if next player bot and he need to do something
	pcmn::TableContext::Data& GetResult() const;

private:
	class Impl;
	Impl* m_Impl;

};

} // namespace srv

#endif // Parser_h__
