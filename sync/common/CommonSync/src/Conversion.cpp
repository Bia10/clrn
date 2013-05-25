#include "stdafx.h"
#include "Conversion.h"

namespace conv
{

unsigned __int64 ToPosix64(const boost::posix_time::ptime& pt)
{
	using namespace boost::posix_time;
	static ptime epoch(boost::gregorian::date(2012, 1, 1));
	time_duration diff(pt - epoch);
	return diff.total_milliseconds();
}

boost::posix_time::ptime FromPosix64(const unsigned __int64 time)
{
	using namespace boost::posix_time;
	static ptime epoch(boost::gregorian::date(2012, 1, 1));
	epoch + boost::posix_time::milliseconds(time);
	return epoch + boost::posix_time::milliseconds(time);
}

} // namespace conv