#ifndef stdafx_h__
#define stdafx_h__

#include "Exception.h"
#include "Log.h"
#include "Constants.h"
#include "IKernel.h"
#include "SQLiteDB.h"
#include "data.pb.h"
#include "job.pb.h"
#include "packet.pb.h"
#include "Settings.h"
#include "GUID.h"
#include "Conversion.h"
#include "Table.h"
#include "StreamHelpers.h"

#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include <deque>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

static const unsigned int CURRENT_MODULE_ID = SETTINGS_MODULE_ID;

#endif // stdafx_h__
