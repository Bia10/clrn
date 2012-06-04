/*
 * UserInfo.h
 *
 *  Created on: Dec 17, 2011
 *      Author: root
 */

#ifndef USERINFO_H_
#define USERINFO_H_

#include "Types.h"

#include <vector>

#include <boost/scoped_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

//! Class for user info storing
class CUserInfo : private boost::noncopyable
{
public:

	//! Struct for user description
	struct CUser
	{
	    _tstring sLogin;
	    _tstring sPasswordHash;
	    _tstring sSessionHash;
	    boost::posix_time::ptime ptLogin;
	};

	//! Type of the user storage
	typedef std::vector< CUser > 				TUsers;

	CUserInfo(const _tstring& sUserInfoStorageFileName);
	virtual ~CUserInfo();

	//! Set user info
	void 										Set(const _tstring& sSessionId, const CUser& UserData);

	//! Gets user info
	const CUser&								Get(const _tstring& sSessionId) const;

	//! Is user credentials valid
	const bool									IsUserCredentialsValid(const _tstring & sLogin, const _tstring & sPassword, _tstring& sPasswordHash) const;

private:

	//! Load user info
	void 										Load();

	//! Store user info
	void 										Store() const;

	//! User info storage path
	const _tstring								m_sStoragePath;

	//! Users
	TUsers										m_vecUsers;

	//! Mutex for user operations
	mutable boost::mutex						m_mxData;
};

//! Singletone for users
class Users
{
public:
	//! Users storage initialization
	static void 		Init(const _tstring& sUserInfoStorageFileName);

	//! Reference to single object
	static CUserInfo&	Instance();

private:

	//! Pointer to the single user info storage
	static boost::scoped_ptr< CUserInfo > ms_pUsers;
};

#endif /* USERINFO_H_ */
