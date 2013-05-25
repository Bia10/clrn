/*
 * Types.h
 *
 *  Created on: Sep 23, 2011
 *      Author: clrn
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <string>
#include <sstream>

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#endif

#ifdef UNICODE

typedef wchar_t TCHAR;
#define _T(x) L##x

#else

typedef char TCHAR;
#define _T(x) x

#endif

typedef std::basic_string< TCHAR > _tstring;
typedef std::basic_ostringstream< TCHAR > _tostringstream;
typedef std::basic_istringstream< TCHAR > _tistringstream;
typedef std::basic_ostream< TCHAR > _tostream;
typedef std::basic_istream< TCHAR > _tistream;
typedef std::basic_ifstream< TCHAR > _tifstream;

#endif /* TYPES_H_ */
