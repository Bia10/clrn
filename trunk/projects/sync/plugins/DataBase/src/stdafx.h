#ifndef stdafx_h__
#define stdafx_h__

#include "Exception.h"
#include "Constants.h"
#include "Table.h"
#include "SQLiteDB.h"
#include "data.pb.h"
#include "packet.pb.h"
#include "job.pb.h"
#include "IKernel.h"
#include "StreamHelpers.h"
#include "DataBasePlugin.h"
#include "JobFactory.h"
#include "ILog.h"
#include "Conversion.h"
#include "FileSystem.h"
#include "ProcedureExecutor.h"
#include "Events/inc/Event.h"

#include <boost/function.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#pragma warning(push)
#pragma warning(disable : 4996)
#include <boost/algorithm/string.hpp>
#pragma warning(pop) 

static const unsigned int CURRENT_MODULE_ID =  DATABASE_MODULE_ID;

#endif // stdafx_h__
