#ifndef stdafx_h__
#define stdafx_h__

#include "Exception.h"
#include "Log.h"
#include "Constants.h"
#include "packet.pb.h"
#include "Constants.h"
#include "SQLiteDB.h"
#include "Table.h"
#include "Conversion.h"
#include "FileSystem.h"
#include "StreamHelpers.h"

#include <stdio.h>
#include <iostream>

#include <loki/Factory.h>

#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

static const unsigned int CURRENT_MODULE_ID = KERNEL_MODULE_ID;

#endif // stdafx_h__
