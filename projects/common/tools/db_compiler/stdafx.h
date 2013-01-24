#ifndef stdafx_h__
#define stdafx_h__

#include "SQLiteDB.h"
#include "Log.h"
#include "Exception.h"
#include "Modules.h"

#include <vector>
#include <string>
#include <iostream>
 
#pragma warning(push)
#pragma warning(disable : 4512)
#include <boost/program_options.hpp>
#pragma warning(pop) 

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/exception_ptr.hpp>

static const unsigned int CURRENT_MODULE_ID = Modules::Tools;

#endif // stdafx_h__
