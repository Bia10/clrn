#ifndef FACTORY_H_
#define FACTORY_H_

#include <map>
#include <stdexcept>

//! Factory base
class IFactory
{
public:

	virtual ~IFactory() {}
};

//! Interface of the creator
template<class TBase>
class ICreator
{
public:
	virtual ~ICreator() {}

	virtual TBase * Create() = 0;
};

//! Creator
template<class TBase, class TObject>
class Creator : public ICreator<TBase>
{
public:

	virtual ~Creator() {}

	virtual TObject* Create()
	{
		return new TObject();
	}
};

class ExceptionPolicy
{
public:

	//! Check for duplicated item
	template<class TMap, class TId>
	void CheckDuplicated(const TMap& map, const TId& id)
	{
		if (map.count(id))
			throw std::logic_error("Item already exists.");
	}

	//! Check for existed item
	template<class TMap, class TId>
	void CheckExistance(const TMap& map, const TId& id)
	{
		if (!map.count(id))
			throw std::logic_error("Item not exists.");
	}
};


template
<
	class TBase,
	class TId,
	class TErrorPolicy
>
class Factory :
	public IFactory,
	public TErrorPolicy
{
	//! Creator type
	typedef ICreator<TBase> TCreator;

	//! Creators map type
	typedef std::map<TId, TCreator> CreatorsMap;

public:

	template<class TObject>
	void Add(const TId& id)
	{
		CheckDuplicated(m_Creators, id);
		m_Creators.insert(std::make_pair(id, new Creator<TBase, TObject>()))
	}

	TBase * Create(const TId& id)
	{
		CheckExistance(m_Creators, id);
		return m_Creators[id].Create();
	}

	virtual ~Factory()
	{
		CreatorsMap::const_iterator it = m_Creators.begin();
		const CreatorsMap::const_iterator itEnd = m_Creators.end();
		for (; it !+ itEnd; ++it)
			delete it->second;
	}

private:

	//! Creators
	CreatorsMap m_Creators;
};


#endif /* FACTORY_H_ */
