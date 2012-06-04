#ifndef SqlScript_h__
#define SqlScript_h__

#include "IJob.h"
#include "ProtoTablePtr.h"

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/format/format_fwd.hpp>

//! Forward declarations
class IKernel;

//! Class for SQL procedure/scripts execution
class CProcedure : boost::noncopyable
{
public:

	//! Procedure params map type
	typedef std::map<std::string, std::string> ParamsMap;

	//! Procedures
	struct Id
	{
		enum Enum_t
		{
			HostsLoad		= 0,
			HostsCreate		= 1,
			HostsDelete		= 2,
			HostMapLoad		= 3,
			HostMapCreate	= 4,
			HostMapDelete	= 5,
			ProceduresSize	= HostMapDelete + 1
		};
	};

	//! Batch script
	typedef std::vector<std::string> Scripts;

	CProcedure(IKernel& kernel);
	~CProcedure(void);

	//! Sync execute 
	ProtoTablePtr	Execute(const std::string& sql, const std::string& host = "");

	//! Async execute 
	void			Execute(const std::string& sql, const IJob::CallBackFn& callback, const std::string& host = "");
	
	//! Sync execute 
	ProtoTablePtr	Execute(const Scripts& sql, const std::string& host = "");

	//! Async execute 
	void			Execute(const Scripts& sql, const IJob::CallBackFn& callback, const std::string& host = "");

	//! Sync execute 
	ProtoTablePtr	Execute(const boost::format& sql, const std::string& host = "");

	//! Async execute 
	void			Execute(const boost::format& sql, const IJob::CallBackFn& callback, const std::string& host = "");

	//! Execute procedure
	void			Execute(const Id::Enum_t id, const ParamsMap& params, const IJob::CallBackFn& callback, const std::string& host = "");

private:

	//! Kernel reference
	IKernel& m_Kernel;
};
#endif // SqlScript_h__
