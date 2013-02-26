#ifndef TableContext_h__
#define TableContext_h__

namespace pcmn
{
struct TableContext
{
	TableContext()
		: m_MaxBet(0)
		, m_Pot(0)
	{

	}
	unsigned m_MaxBet;
	unsigned m_Pot;
};
}


#endif // TableContext_h__
