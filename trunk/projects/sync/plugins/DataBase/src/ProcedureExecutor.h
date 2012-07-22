#ifndef ProcedureExecutor_h__
#define ProcedureExecutor_h__

#include "ProtoPacketPtr.h"
#include "IKernel.h"
#include "Procedure.h"

#include <memory>
#include <map>

#include <boost/noncopyable.hpp>

//! Class for procedure execution
class CProcedureExecutor : boost::noncopyable
{
	friend class std::auto_ptr<CProcedureExecutor>;

	//! Ctor
	CProcedureExecutor(IKernel& kernel, ILog& log);

	//! Dtor
	~CProcedureExecutor(void);

public:

	//! Execute procedure
	void						Execute(const CProcedure::Id::Enum_t id, const CProcedure::ParamsMap& params, data::Table& result);
	
	//! Init
	static void					Create(IKernel& kernel, ILog& log);

	//! Instance reference
	static CProcedureExecutor&	Instance();

private:

	//! Implementation
	class Impl;
	std::auto_ptr<Impl>							m_pImpl;

	//! Single instance
	static std::auto_ptr<CProcedureExecutor>	s_pInstance;
};
#endif // ProcedureExecutor_h__
