#pragma once

#include "XmlTools.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

//! Interface for import entity
//!
//! \class IEntity
//!
class IEntity : 
	public boost::enable_shared_from_this<IEntity>,
	public boost::noncopyable
{
public:

	//! Pointer type
	typedef boost::shared_ptr<IEntity> Pointer;

	//! Set entity data
	virtual void SetData(const xml::wide_t& xmlData) = 0;

	//! Add reference
	virtual void AddReference(const Pointer& entity) = 0;

	//! Add dependency
	virtual void AddDependency(const Pointer& entity) = 0;

	//! Process entity
	virtual void Process() = 0;

	//! Handle recieved data
	virtual void HandleData(const void* const data, std::size_t size) = 0;
};