#ifndef stdafx_h__
#define stdafx_h__

#include "Exception.h"
#include "ILog.h"
#include "Constants.h"
#include "job.pb.h"
#include "packet.pb.h"
#include "data.pb.h"
#include "Conversion.h"
#include "StreamHelpers.h"
#include "Table.h"
#include "IKernel.h"

#include <list>
#include <map>
#include <limits>

#include <boost/function.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#pragma warning(push)
#pragma warning(disable:4512) // 'boost::signals::detail::bound_objects_visitor' : assignment operator could not be generated
#include <boost/signals.hpp>
#pragma warning(pop)
#include <boost/format.hpp>

static const unsigned int CURRENT_MODULE_ID = EVENTS_MODULE_ID;

#endif // stdafx_h__