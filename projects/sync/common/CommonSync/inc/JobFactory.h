#ifndef JobFactory_h__
#define JobFactory_h__

#include "IJob.h"

#include <loki/Factory.h>

//! Forward declarations
namespace jobs
{
	enum Job_JobId;
	class Job;
}
class IKernel;
class ILog;

//! Job creator
//!
//! \class Creator
//!
template
<
	typename Base, 
	typename Object
>
struct Creator
{
	Base* operator () (IKernel& kernel, ILog& logger)
	{
		return new Object(kernel, logger);
	}
	~Creator()
	{

	}
};

//! Job factory declaration
//!
//! \class CJobFactory
//!
class CJobFactory
{
	//! Factory type
	typedef Loki::Factory
	<
		IJob,
		jobs::Job_JobId, 
		LOKI_TYPELIST_2(IKernel&, ILog&)
	> 
	FatoryType;

public:

	//! Register job executor
	template<typename T>
	void		Register(const jobs::Job_JobId id)
	{
		m_Factory.Register(id, Creator<IJob, T>());
	}

	//! Create
	IJob::Ptr	Create
	(
		const jobs::Job_JobId id, 
		IKernel& kernel, 
		ILog& logger
	)
	{
		return IJob::Ptr
		(
			m_Factory.CreateObject
			(
				id, 
				kernel, 
				logger
			)
		);
	}

	//! Unregister
	void		Unregister(const jobs::Job_JobId id)
	{
		m_Factory.Unregister(id);
	}

private:

	//! Factory
	FatoryType m_Factory;
};


#endif // JobFactory_h__