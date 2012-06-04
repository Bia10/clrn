#pragma once

#include "IEntity.h"

#include <list>

#include <boost/scoped_ptr.hpp>

class BaseEntity :
	public IEntity
{
	//! List of pointers type
	typedef std::list<Pointer>				PointerList;

	//! Pointer to xml data type
	typedef boost::scoped_ptr<xml::wide_t>	XmlPtr;
	
public:
	//! Set entity data
	void SetData(const xml::wide_t& xmlData);

	//! Add reference
	void AddReference(const Pointer& entity);

	//! Add dependency
	void AddDependency(const Pointer& entity);

	//! Process entity
	void Process();

	//! Handle recieved data
	void HandleData(const void* const data, std::size_t size);

protected:

	//! Xml data
	XmlPtr			m_XmlData;

	//! List of dependencies
	PointerList		m_Dependencies;

	//! List of references
	PointerList		m_References;
};
