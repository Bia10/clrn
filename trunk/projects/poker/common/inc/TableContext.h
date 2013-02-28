#ifndef TableContext_h__
#define TableContext_h__

#include "Actions.h"

namespace pcmn
{
struct TableContext
{
	TableContext()
		: m_MaxBet(0)
		, m_Pot(0)
		, m_LastAction()
		, m_LastAmount(0)
	{

	}
	unsigned m_MaxBet;
	unsigned m_Pot;
	Action::Value m_LastAction;
	unsigned m_LastAmount;
};
}


#endif // TableContext_h__
