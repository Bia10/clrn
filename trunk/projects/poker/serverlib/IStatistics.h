#ifndef IStatistics_h__
#define IStatistics_h__

#include "ILog.h"
#include "TableContext.h"
#include "Actions.h"

#include <vector>
#include <string>
#include <map>

namespace srv
{

//! Stats writer/reader
class IStatistics
{
public:

	//! Player ranges
	typedef std::map<pcmn::Action::Value, int> CardRanges;
	typedef std::map<std::string, CardRanges> PlayerRanges;

	//! Player equities
	struct PlayerEquities
	{
		typedef std::vector<pcmn::Action::Value> Actions;
		typedef std::vector<PlayerEquities> List;

		std::string m_Name;
		Actions m_Actions;
		float m_PotAmount;
		float m_WinRate;
	};

	virtual ~IStatistics() {}

	//! String list type
	typedef std::vector<std::string> Strings;

	//! Int list type
	typedef std::vector<int> Ints;

	//! Write statistics data
	virtual void Write(pcmn::TableContext::Data& data) = 0;

	//! Get card ranges
	virtual void GetRanges(PlayerRanges& players) = 0;

	//! Get last actions
	virtual void GetLastActions(const std::string& target, const std::string& opponent, int& checkFolds, int& calls, int& raises) = 0;

	//! Get equities
	virtual void GetEquities(PlayerEquities::List& players) = 0;


};

} // namespace srv

#endif // IStatistics_h__
