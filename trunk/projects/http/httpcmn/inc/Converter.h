/*
 * Converter.h
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#ifndef CONVERTER_H_
#define CONVERTER_H_

#include "Types.h"

//! Forward declarations
namespace boost
{
	namespace posix_time
	{
		class ptime;
	}
}

namespace cmn
{

//! Class for misc conversions
class CConverter
{
public:
	CConverter();
	virtual ~CConverter();

	//! Url decoding
	static void 				DecodeUrl(const _tstring& sIn, _tostream& osResult);

	//! Posix time to cookie expire time formatter
	static const _tstring 		ToCookieExpireTime(const boost::posix_time::ptime& ptTime);

	//! From base 64
	static const _tstring		ToBase64(const _tstring& sInput);

	//! To base 64
	static const _tstring		FromBase64(const _tstring& sInput);


};

} /* namespace cmn */
#endif /* CONVERTER_H_ */
