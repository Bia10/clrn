/*
 * Factory.h
 *
 *  Created on: Nov 13, 2011
 *      Author: root
 */

#ifndef FACTORY_H_
#define FACTORY_H_

#include "IHttpData.h"
#include "IExecutor.h"

#include <vector>
#include <algorithm>
#include <stdexcept>

namespace cmn
{

//! Factory base
class IFactory
{
public:

	virtual ~IFactory() {}
};

//! Interface of the creator
template < class TBase >
class ICreator
{
public:
	virtual ~ICreator() {}

	virtual TBase * Create() = 0;
};

//! Creator
template < class TObject, class TBase >
class CCReator : public ICreator< TBase >
{
public:

	virtual ~CCReator() {}

	virtual TObject * Create()
	{
		return new TObject();
	}
};

template
<
	class TBase,
	class TRequestInfo = http::IHttpData,
	class TPriority = int
>
class CFactory :
	public IFactory
{
private:

	struct CCallInfo
	{
		TPriority 			Priority;
		ICreator< TBase > * pCreator;
		TBase * 			pExecutor;
	};

public:

	typedef std::vector< CCallInfo > TCreators;

	template < class TObject >
	void Add(const TPriority& Priority)
	{
		CCallInfo Call;
		Call.Priority = Priority;
		Call.pCreator = new CCReator< TObject, TBase >();
		Call.pExecutor = Call.pCreator->Create();

		m_vecCreators.push_back(Call);

		// Sort all creators by priority
		std::sort(m_vecCreators.begin(), m_vecCreators.end(), [](const CCallInfo& lhs, const CCallInfo& rhs)
				{
					return lhs.Priority > rhs.Priority;
				}
		);
	}

	TBase * Create(const TRequestInfo& Info)
	{
		for (const auto& Call : m_vecCreators)
		{
			if (Call.pExecutor->Match(Info))
				return Call.pCreator->Create();
		}
		return NULL;
	}

	virtual ~CFactory()
	{
		for (const auto& Call : m_vecCreators)
		{
			delete Call.pCreator;
			delete Call.pExecutor;
		}
	}

private:

	TCreators m_vecCreators;
};

//! Type of the request executors factory
typedef CFactory< srv::IExecutor, http::IHttpData, int > TExecutorsFactory;

}/* namespace cmn */

#endif /* FACTORY_H_ */
