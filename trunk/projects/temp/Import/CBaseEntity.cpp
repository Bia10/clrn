#include "CBaseEntity.h"

#include <algorithm>

#include <boost/property_tree/ptree.hpp>

void BaseEntity::SetData(const xml::wide_t& xmlData)
{
	m_XmlData.reset(new xml::wide_t(xmlData));
}

void BaseEntity::AddReference(const Pointer& entity)
{
	if (std::find(m_References.begin(), m_References.end(), entity) == m_References.end())
		m_References.push_back(entity);
}

void BaseEntity::AddDependency(const Pointer& entity)
{
	if (std::find(m_Dependencies.begin(), m_Dependencies.end(), entity) == m_Dependencies.end())
		m_Dependencies.push_back(entity);
}

void BaseEntity::Process()
{

}