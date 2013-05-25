/*
 * CacheManager.h
 *
 *  Created on: Nov 10, 2011
 *      Author: root
 */

#ifndef CACHEMANAGER_H_
#define CACHEMANAGER_H_

#include "Types.h"
#include "HttpData.h"

#include <map>

namespace srv
{

class CCacheManager
{
public:

	//! Structure for ordering requests
	struct CCompare
	{
		const bool operator () (const http::IHttpData& lhs, const http::IHttpData& rhs) const
		{
			const _tstring& sPath = lhs.GetRequestPath();
			return sPath < rhs.GetRequestPath();
		}
	};

	//! Cache type
	typedef std::map< http::CHttpData, http::CHttpData, CCompare > TCache;

	CCacheManager();
	~CCacheManager();

	//! Saving reply to cache
	void			Add(const http::CHttpData& Request, const http::CHttpData& Reply, const bool bReplaceIfExists);

	bool			Get(const http::CHttpData& Request, http::CHttpData& Reply);
private:

	//! Cache
	TCache			m_mapCache;

};

class CacheManager
{
public:
	static CCacheManager& Instance();
private:
	static CCacheManager ms_manager;
};

} /* namespace srv */
#endif /* CACHEMANAGER_H_ */
