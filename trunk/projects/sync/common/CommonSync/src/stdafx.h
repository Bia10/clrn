#ifndef stdafx_h__
#define stdafx_h__

#include "Exception.h"
#include "Log.h"
#include "CppSQLite3.h"
#include "Constants.h"
#include "data.pb.h"
#include "StreamHelpers.h"
#include "Table.h"
#include "SQLiteDB.h"
#include "Conversion.h"

#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include <deque>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/bind.hpp>
#include <boost/regex.hpp>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <boost/algorithm/string.hpp>
#pragma warning(pop) 
 

static const unsigned int CURRENT_MODULE_ID = COMMON_MODULE_ID;

#endif // stdafx_h__