/*
 * CacheManager.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: root
 */

#include "CacheManager.h"
#include "HttpData.h"

namespace srv
{

CCacheManager::CCacheManager()
{
	// TODO Auto-generated constructor stub

}

CCacheManager::~CCacheManager()
{
	// TODO Auto-generated destructor stub
}

void CCacheManager::Add(const http::CHttpData& Request, const http::CHttpData & Reply, const bool bReplaceIfExists)
{
	if (bReplaceIfExists || !m_mapCache.count(Request))
	{
		m_mapCache[Request] = Reply;
	}
}

bool CCacheManager::Get(const http::CHttpData& Request, http::CHttpData& Reply)
{
	if (!m_mapCache.count(Request))
	{
		return false;
	}

	Reply = m_mapCache[Request];

	return true;
}

CCacheManager CacheManager::ms_manager;

CCacheManager& CacheManager::Instance()
{
	return ms_manager;
}

}/* namespace srv */
