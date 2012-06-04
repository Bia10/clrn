#ifndef Table_h__
#define Table_h__

#include <map>
#include <list>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/range/iterator.hpp>

//! Forward declarations
namespace data
{
	class Table;
	class Table_Row;
	enum Table_Id;
}

//! Table columns mapping
//!
//! \class CColumnMapping
//!
class CTableMapping : boost::noncopyable
{
	//! Ctor
	CTableMapping();

public:

	//! Column mapping
	class CColumnMapping
	{
	public:
		//! Column mapping 
		typedef std::map<std::string, std::size_t>	ColumnMap;

		//! Ctor
		CColumnMapping(const char* columns[]);

		//! Get index by field name
		std::size_t operator [] (const std::string& name) const;

		//! Compare
		bool operator == (const ColumnMap& that) const;

		//! Compare
		bool operator != (const ColumnMap& that) const;

		//! Get mapping
		const ColumnMap& Mapping() const;

	private:
		//! Mapping
		ColumnMap		m_Map;
	};

	//! Table column mapping 
	typedef std::map<data::Table_Id, CColumnMapping>	TableColumnMap;

	//! Table mapping 
	typedef std::map<data::Table_Id, std::string>		TableNameMap;

	//! Get column mapping by id
	const CColumnMapping& operator[] (const data::Table_Id id) const;

	//! Get name mapping by id
	const std::string& Name(const data::Table_Id id) const;

	//! Get id by mapping
	const data::Table_Id operator[] (const CColumnMapping::ColumnMap& mapping) const;

	//! Single instance
	static CTableMapping&	Instance();

private:

	//! Table columns mapping
	TableColumnMap	m_ColumnMap;

	//! Table name mapping
	TableNameMap	m_NameMap;

	//! Single instance ptr
	static boost::scoped_ptr<CTableMapping> s_Mapping;
};

//! Google protobuffer table wrapper
//!
//! \class CTable
//!
class CTable : boost::noncopyable
{
public:

	//! Ctor
	CTable(data::Table& table);

	//! Dtor
	~CTable();

	//! Scope table ptr
	typedef boost::scoped_ptr<CTable> Ptr;

	//! Table row
	class Row
	{
	public:

		//! Pointer type
		typedef boost::shared_ptr<Row>	Ptr;

		//! Ctor
		Row(const CTableMapping::CColumnMapping& mapping, data::Table_Row& row);

		//! Dtor
		~Row();

		//! Const getter by index
		const std::string& operator [](const std::size_t index) const;

		//! Getter by index
		std::string& operator [](const std::size_t index);

		//! Const getter by name
		const std::string& operator [](const std::string& name) const;

		//! Getter by name
		std::string& operator [](const std::string& name);

		//! Compare with table
		bool operator == (const data::Table_Row& row) const;

		//! Compare with table
		bool operator != (const data::Table_Row& row) const;

	private:
		//! Implementation
		class Impl;
		Impl* m_pImpl;
	};

	//! Rows vector
	typedef std::vector<Row::Ptr> RowsVector;

	//! Class iterator
	class iterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef Row value_type;
		typedef std::size_t difference_type;
		typedef Row* pointer;
		typedef Row& reference;

		iterator(const RowsVector::iterator& it);
		iterator& operator ++ ();
		pointer operator -> ();
		reference operator * ();
		bool operator == (const iterator& it) const;
	private:

		RowsVector::iterator m_It;;
	};

	//! Const iterator type
	typedef iterator const_iterator;

	//! List of uints
	typedef			std::list<std::size_t> IntList;

	//! Row getter by index
	Row&			operator [] (const std::size_t index);

	//! Const row getter by index
	const Row&		operator [] (const std::size_t index) const;

	//! Row getter by predicate
	Row&			operator [] (const std::string& predicate);

	//! Const row getter by predicate
	const Row&		operator [] (const std::string& predicate) const;

	//! Stream operator
	std::ostream&	operator << (std::ostream& stream) const;

	//! Save as sql
	void			Sql(std::ostream& stream);

	//! Table size
	std::size_t		Size() const;

	//! Add row
	Row&			AddRow();

	//! Add rows from another table
	void			AddRows(const data::Table& table);

	//! Delete rows
	void			EraseRows(const data::Table& table);
	 
	//! Create index
	void			CreateIndex(const std::string& fieldName);

	//! Find rows
	void			FindRows(IntList& indexes, const std::string& predicate) const;

	//! First row
	iterator		begin();

	//! Last row
	iterator		end();

	//! First row
	const_iterator	begin() const;

	//! Last row
	const_iterator	end() const;

private:

	//! Forward declarations
	class Impl;

	//! Implementation
	Impl* m_pImpl;
};

#endif // Table_h__
