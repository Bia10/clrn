#ifndef stdafx_h__
#define stdafx_h__

#include "Exception.h"
#include "ILog.h"
#include "Constants.h"
#include "IKernel.h"
#include "packet.pb.h"
#include "Table.h"
#include "job.pb.h"
#include "DataBase/inc/Procedure.h"
#include "Events/inc/Event.h"
#include "Conversion.h"
#include "StreamHelpers.h"
#include "GUID.h"

#include <list>
#include <map>
#include <limits>

#include <boost/function.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#pragma warning(push)
#pragma warning(disable : 4996)
#include <boost/algorithm/string.hpp>
#pragma warning(pop) 

static const unsigned int CURRENT_MODULE_ID =  NETWORK_MODULE_ID;

#endif // stdafx_h__