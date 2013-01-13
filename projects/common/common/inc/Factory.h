////////////////////////////////////////////////////////////////////////////////
//
// COPYRIGHT    :   Positive Technologies, 2006-2012
// PROGRAMMED BY:   Karnaukhov Sergey
// BRIEF        :   Object factory implementation
// DESCRIPTION  :   Implements simple registering/creating objects.
//
//////////////////////////////////////////////////////////////////////////////// 
#ifndef Factory_h__
#define Factory_h__

#include "PTConversions.h"

#include <stdexcept>
#include <sstream>

#include <boost/ptr_container/ptr_map.hpp>

#if _MSC_VER >= 1500
#pragma warning (push)
#pragma warning(disable: 4481) // nonstandard extension used: override specifier 'override'
#endif

namespace PT
{
namespace Functional
{
namespace Details
{

//! Interface of the creator
template<class Base>
class ICreator
{
public:
	virtual ~ICreator() {}

	//! Create object
	virtual Base* Create() const = 0;
};

//! Creator with default ctor
template<class Base, class Object>
class DefaultCreator : public ICreator<Base>
{
	//! Create object
	virtual Base* Create() const override { return new Object(); }
};

//! Creator with functor
template<class Base, class Functor>
class FunctorCreator : public ICreator<Base>
{
public:
	template<typename T>
	FunctorCreator(const T& arg) : m_Functor(arg) {}

private:
	//! Create object
	virtual Base* Create() const override { return m_Functor(); }

private:
	Functor m_Functor;
};

//! Empty class
struct Dummy 
{
};

//! Empty locker
struct DummyLocker
{	
	template<typename T>
	DummyLocker(const T&) {} 
};

} // namespace Details

//! Exception throwing policy
class ExceptionPolicy
{
public:
	template<typename Map, typename Id>
	static void CheckInsert(const Map& map, const Id& id)
	{
		if (map.count(id))
		{
			std::ostringstream oss;
			oss << "Item with id: [" << PT::tostr(id) << "] already exists in factory.";
			throw std::logic_error(oss.str().c_str());
		}	
	}

	template<typename Map, typename Id>
	static void CheckCreate(const Map& map, const Id& id)
	{
		if (!map.count(id))
		{
			std::ostringstream oss;
			oss << "Item with id: [" << PT::tostr(id) << "] not exists in factory.";
			throw std::logic_error(oss.str().c_str());
		}
	}
};

//! Ignore error policy
class IgnoreErrorPolicy
{
public:
	template<typename Map, typename Id>
	static void CheckInsert(const Map& /*map*/, const Id& /*id*/)
	{
	}

	template<typename Map, typename Id>
	static void CheckCreate(const Map& /*map*/, const Id& /*id*/)
	{
	}
};

//! Exception throwing policy with replace if exists
class ReplaceIfExistsPolicy
{
public:
	template<typename Map, typename Id>
	static void CheckInsert(Map& map, const Id& id)
	{
		if (map.count(id))
			map.erase(id);
	}

	template<typename Map, typename Id>
	static void CheckCreate(const Map& map, const Id& id)
	{
		if (!map.count(id))
		{
			std::ostringstream oss;
			oss << "Item with id: [" << PT::tostr(id) << "] not exists in factory.";
			throw std::logic_error(oss.str().c_str());
		}
	}
};

//! No locking policy
class NoLockPolicy
{
public:
	typedef Details::Dummy Object;
	typedef Details::DummyLocker ScopedLock;

	Object& GetObject() const {return m_Dummy;}
private:
	mutable Object m_Dummy;
};

//! Scoped lockable policy
template<typename T>
class MutexLockPolicy
{
public:
	typedef typename T::scoped_lock ScopedLock;
	typedef T Object;
	Object& GetObject() const {return m_Mutex;}
private:
	mutable Object m_Mutex;
};

//! Object factory
template
<
	typename Base,
	typename Id,
	typename CheckPolicy = ExceptionPolicy,
	typename LockPolicy = NoLockPolicy
>
class Factory : protected LockPolicy
{
	typedef Details::ICreator<Base> ICreator;
	typedef boost::ptr_map<Id, ICreator> Map;
public:

	//! Register default creator
	template<class Object>
	void Register(const Id& id)
	{
		const LockPolicy::ScopedLock lock(LockPolicy::GetObject());

		CheckPolicy::CheckInsert(m_Creators, id);
		std::auto_ptr<ICreator> creator(new Details::DefaultCreator<Base, Object>());
		m_Creators.insert(id, creator);
	}

	//! Register functor creator
	template<class Functor>
	void Register(const Id& id, const Functor& functor)
	{
		const LockPolicy::ScopedLock lock(LockPolicy::GetObject());

		CheckPolicy::CheckInsert(m_Creators, id);
		std::auto_ptr<ICreator> creator(new Details::FunctorCreator<Base, Functor>(functor));
		m_Creators.insert(id, creator);
	}

	//! Register custom creator
	template<class Functor>
	void Register(const Id& id, std::auto_ptr<ICreator>& creator)
	{
		const LockPolicy::ScopedLock lock(LockPolicy::GetObject());

		CheckPolicy::CheckInsert(m_Creators, id);
		m_Creators.insert(id, creator);
	}

	//! Create object
	Base* Create(const Id& id) const
	{
		const LockPolicy::ScopedLock lock(LockPolicy::GetObject());
		CheckPolicy::CheckCreate(m_Creators, id);
		const Map::const_iterator it = m_Creators.find(id);
		return m_Creators.end() == it ? NULL : it->second->Create();
	}

	//! Is factory empty
	bool IsEmpty() const
	{
		const LockPolicy::ScopedLock lock(LockPolicy::GetObject());
		return m_Creators.empty();
	}	

	//! Destructor
	virtual ~Factory()
	{
		const LockPolicy::ScopedLock lock(LockPolicy::GetObject());
		m_Creators.clear();
	}

private:

	//! Creators
	Map m_Creators;
};


} // namespace Functional
} // namespace PT

#if _MSC_VER >= 1500
#pragma warning (pop)
#endif

#endif // Factory_h__
