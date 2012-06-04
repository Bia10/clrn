/*
 * Converter.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#include "Converter.h"
#include "Exception.h"
#include "Status.h"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/archive/iterators/istream_iterator.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>

typedef boost::archive::iterators::istream_iterator< char > boost_istream_iterator;
typedef boost::archive::iterators::ostream_iterator< char > boost_ostream_iterator;

typedef boost::archive::iterators::insert_linebreaks<
    boost::archive::iterators::base64_from_binary<
        boost::archive::iterators::transform_width<boost_istream_iterator, 6, 8>
    >, 76
> bin_to_base64;

typedef boost::archive::iterators::transform_width<
    boost::archive::iterators::binary_from_base64<
        boost::archive::iterators::remove_whitespace<boost_istream_iterator>
    >, 8, 6
> base64_to_bin;

namespace cmn
{

CConverter::CConverter()
{
	// TODO Auto-generated constructor stub

}

CConverter::~CConverter()
{
	// TODO Auto-generated destructor stub
}

void CConverter::DecodeUrl(const _tstring& sIn, _tostream& osResult)
{
	TRY_EXCEPTIONS
	{
		for (std::size_t i = 0; i < sIn.size(); ++i)
		{
			if (sIn[i] == '%')
			{
				if (i + 3 <= sIn.size())
				{
					int value = 0;
					std::istringstream is(sIn.substr(i + 1, 2));
					if (is >> std::hex >> value)
					{
						osResult << static_cast<char>(value);
						i += 2;
					}
					else
					{
						THROW_EXCEPTION_CODE(CStatus::StatusCode::eBadRequest, "Failed to decode url string.");
					}
				}
				else
				{
					THROW_EXCEPTION_CODE(CStatus::StatusCode::eBadRequest, "Failed to decode url string.");
				}
			}
			else if (sIn[i] == '+')
			{
				osResult << ' ';
			}
			else
			{
				osResult << sIn[i];
			}
		}
	}
	CATCH_PASS_EXCEPTIONS
}

const _tstring CConverter::ToCookieExpireTime(const boost::posix_time::ptime & ptTime)
{
	TRY_EXCEPTIONS
	{
		_tostringstream ossDate;
		ossDate
			<< ptTime.date().day_of_week().as_short_string() << _T(", ")
			<< ptTime.date().day().as_number() << _T(" ")
			<< ptTime.date().month().as_short_string() << _T(" ")
			<< ptTime.date().year().operator unsigned short int() << _T(" ")
			<< ptTime.time_of_day().hours() << _T(":")
			<< ptTime.time_of_day().minutes() << _T(":")
			<< ptTime.time_of_day().seconds() << _T(" GMT");

		return ossDate.str();
	}
	CATCH_PASS_EXCEPTIONS
}

const _tstring CConverter::ToBase64(const _tstring & sInput)
{
	TRY_EXCEPTIONS
	{
		_tostringstream ossResult;

		_tistringstream iss(sInput);
		std::copy(
				bin_to_base64(boost_istream_iterator(iss >> std::noskipws)),
				bin_to_base64(boost_istream_iterator()),
				boost_ostream_iterator(ossResult)
		);

		return ossResult.str();
	}
	CATCH_PASS_EXCEPTIONS
}



const _tstring CConverter::FromBase64(const _tstring & sInput)
{
	TRY_EXCEPTIONS
	{
		_tostringstream ossResult;

		_tistringstream iss(sInput);
		std::copy(
				base64_to_bin(boost_istream_iterator(iss >> std::noskipws)),
				base64_to_bin(boost_istream_iterator()),
				boost_ostream_iterator(ossResult)
		);

		return ossResult.str();
	}
	CATCH_PASS_EXCEPTIONS
}




}  /* namespace cmn */
