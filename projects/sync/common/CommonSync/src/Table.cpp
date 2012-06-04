#include "StdAfx.h"

boost::scoped_ptr<CTableMapping> CTableMapping::s_Mapping;
CTableMapping::CTableMapping()
{
	CHECK(m_ColumnMap.empty());

	// init
	{
		const char* temp[] = {NULL};
		m_ColumnMap.insert(std::make_pair(data::Table_Id_None, CColumnMapping(temp)));
		m_NameMap.insert(std::make_pair(data::Table_Id_None, MODULES_TABLE_NAME));
	}
	{
		const char* temp[] = {"id", "name", NULL};
		m_ColumnMap.insert(std::make_pair(data::Table_Id_Modules, CColumnMapping(temp)));
		m_NameMap.insert(std::make_pair(data::Table_Id_Modules, MODULES_TABLE_NAME));
	}
	{
		const char* temp[] = {"id", "module", "name", "value", NULL};
		m_ColumnMap.insert(std::make_pair(data::Table_Id_Settings, CColumnMapping(temp)));
		m_NameMap.insert(std::make_pair(data::Table_Id_Settings, SETTINGS_TABLE_NAME));
	}
	{
		const char* temp[] = {"id", "guid", "ip", "port", NULL};
		m_ColumnMap.insert(std::make_pair(data::Table_Id_Hosts, CColumnMapping(temp)));
		m_NameMap.insert(std::make_pair(data::Table_Id_Hosts, HOSTS_TABLE_NAME));
	}
	{
		const char* temp[] = {"src", "dst", "status", "ping", "ip", "port", NULL};
		m_ColumnMap.insert(std::make_pair(data::Table_Id_HostMap, CColumnMapping(temp)));
		m_NameMap.insert(std::make_pair(data::Table_Id_HostMap, HOSTMAP_TABLE_NAME));
	}
	{
		const char* temp[] = {"from", "to", "status", "ping", "ip", "port", NULL};
		m_ColumnMap.insert(std::make_pair(data::Table_Id_HostMapEvent, CColumnMapping(temp)));
		m_NameMap.insert(std::make_pair(data::Table_Id_HostMapEvent, HOSTMAP_EVENT_NAME));
	}
	{
		const char* temp[] = {"guid", NULL};
		m_ColumnMap.insert(std::make_pair(data::Table_Id_HostStatusEvent, CColumnMapping(temp)));
		m_NameMap.insert(std::make_pair(data::Table_Id_HostStatusEvent, HOST_STATUS_EVENT_NAME));
	}
}

//! Table row implementation
//!
//! \class CTable::Row::Impl
//!
class CTable::Row::Impl : boost::noncopyable
{
public:

	//! Ctor
	Impl(const CTableMapping::CColumnMapping& mapping, data::Table_Row& row)
		: m_Mapping(mapping)
		, m_Row(row)
	{
	
	}

	//! Getter
	std::string& Get(const std::size_t index)
	{
		return *m_Row.mutable_data(index);
	}

	//! Getter
	std::string& Get(const std::string& name)
	{
		return *m_Row.mutable_data(m_Mapping[name]);
	}

	bool operator == (const data::Table_Row& row) const
	{
		if (m_Row.data_size() != row.data_size())
			return false;

		for (int index = 0 ; index < m_Row.data_size(); ++index)
		{
			if (m_Row.data(index) != row.data(index))
				return false;
		}
		return true;
	}

private:

	//! Table mapping
	const CTableMapping::CColumnMapping&	m_Mapping;

	//! Row reference
	data::Table_Row&						m_Row;

};


//! Data table implementation
//!
//! \class CTable::Impl
//!
class CTable::Impl : boost::noncopyable
{
	//! Indexed values types
	typedef std::multimap<std::string, std::size_t>	IndexMap;

	//! Map of indexes type
	typedef std::map<std::string, IndexMap>		MapOfIndexes;

public:

	Impl(data::Table& table)
		: m_Table(table)
	{
		Init();
	}

	//! Init rows
	void Init()
	{
		const CTableMapping::CColumnMapping& mapping = CTableMapping::Instance()[m_Table.id()];

		for (int i = 0 ; i < m_Table.rows_size(); ++i)
		{
			m_Rows.push_back(Row::Ptr(new Row(mapping, *m_Table.mutable_rows(i))));
		}
	}

	//! Getter
	CTable::Row& Get(const std::size_t index)
	{
		CHECK(m_Rows.size() > index, index, m_Rows.size());
		return *m_Rows[index];
	}

	//! Getter
	const CTable::Row& Get(const std::size_t index) const
	{
		CHECK(m_Rows.size() > index, index, m_Rows.size());
		return *m_Rows[index];
	}

	//! Find rows by predicate
	void FindRows(IntList& results, const std::string& predicate)
	{	
		TRY 
		{
			static boost::regex regexp("(&|\\|)?([^\\|&=!]+)(!?=+)([^\\|&=!]+)");
			boost::sregex_iterator it(predicate.begin(), predicate.end(), regexp);
			const boost::sregex_iterator itBegin(it);
			const boost::sregex_iterator itEnd;

			for ( ; it != itEnd; ++it)
			{
				const std::string& pred = (*it)[1];
				const std::string& key = (*it)[2];
				const std::string& comp = (*it)[3];
				const std::string& value = (*it)[4];

				const bool negativePredicate = (comp == "!=");

				// during first iteration or if the '|' specified filling up results list
				if (itBegin == it || pred == "|")
				{	
					if (m_Indexes.count(key))
					{
						// have index on this column
						typedef std::pair<IndexMap::const_iterator, IndexMap::const_iterator> Range;

						// index data for this column
						const IndexMap& indexData = m_Indexes[key];

						// finding indexes for specified value
						const Range range = indexData.equal_range(value);
						for (IndexMap::const_iterator itR = range.first; itR != range.second; ++itR)
							results.push_back(itR->second);
					}
					else
					{
						std::size_t index = 0;
						BOOST_FOREACH(const RowsVector::value_type& row, m_Rows)
						{
							const bool equal = (*row)[key] == value;
							if (!negativePredicate && equal || negativePredicate && !equal)
								results.push_back(index);

							++index;
						}
					}
				}
				else
				{
					// if not first iteration checking up results list
					CHECK(!results.empty(), predicate, m_Table);

					IntList::const_iterator it = results.begin();
					const IntList::const_iterator itEnd = results.end();
					for (; it != itEnd; )
					{
						const bool notEqual = (*m_Rows[*it])[key] != value;
						if (!negativePredicate && notEqual || negativePredicate && !notEqual)
							it = results.erase(it);
						else
							++it;
					}
				}
			}		
		}
		CATCH_PASS_EXCEPTIONS(predicate)	
	}

	//! Getter
	CTable::Row& Get(const std::string& predicate)
	{
		IntList list;
		FindRows(list, predicate);
		CHECK(!list.empty(), predicate, m_Table);
		return *m_Rows[list.front()];
	}

	//! To debug string
	std::string ToString() const
	{
		return m_Table.DebugString();
	}

	//! To sql string
	void Sql(std::ostream& stream)
	{
		CHECK(m_Table.has_id(), m_Table);

		const CTableMapping::CColumnMapping::ColumnMap& map = CTableMapping::Instance()[m_Table.id()].Mapping();
		const std::string& tableName = CTableMapping::Instance().Name(m_Table.id());

		BOOST_FOREACH(const RowsVector::value_type& row, m_Rows)
		{
			stream << "insert into " << tableName << " values(";

			BOOST_FOREACH(const CTableMapping::CColumnMapping::ColumnMap::value_type& column, map)
			{
				if (0 != column.second)
					stream << ", ";

				stream << "'" << (*row)[column.first] << "'";
			}
			stream << ");" << std::endl;
		}
	}

	//! Rows size
	std::size_t Size() const
	{
		return m_Rows.size();
	}

	//! Begin
	CTable::iterator begin()
	{
		return m_Rows.begin();
	}

	//! End
	CTable::iterator end()
	{
		return m_Rows.end();
	}

	//! Add row
	CTable::Row& AddRow()
	{
		const CTableMapping::CColumnMapping& mapping = CTableMapping::Instance()[m_Table.id()];
		const CTableMapping::CColumnMapping::ColumnMap& map = mapping.Mapping();

		data::Table_Row* row = m_Table.add_rows();
		for (std::size_t i = 0; i < map.size(); ++i)
			row->add_data();

		m_Rows.push_back(Row::Ptr(new Row(mapping, *row)));
		return *m_Rows.back();
	}

	//! Add all rows from another table
	void AddRows(const data::Table& table)
	{
		EraseRows(table);

		const CTableMapping::CColumnMapping& mapping = CTableMapping::Instance()[m_Table.id()];

		for (int i = 0 ; i < table.rows_size(); ++i)
		{
			data::Table_Row* row = m_Table.add_rows();
			*row = table.rows(i);
			m_Rows.push_back(Row::Ptr(new Row(mapping, *row)));
		}
	}

	//! Erase rows
	void EraseRows(const data::Table& table)
	{
		std::vector<int> toErase;

		for (int i = 0 ; i < table.rows_size(); ++i)
		{
			const data::Table_Row& tableRow = table.rows(i);

			RowsVector::const_iterator it = m_Rows.begin();
			const RowsVector::const_iterator itEnd = m_Rows.end();
			for (; it != itEnd; ++it)
			{
				const CTable::Row& row = **it;
				
				if (row == tableRow)
					toErase.push_back(i);
			}
		}	

		RemoveRows(toErase);
	}

	//! Create index by field name
	void CreateIndex(const std::string& fieldName)
	{
		const CTableMapping::CColumnMapping& mapping = CTableMapping::Instance()[m_Table.id()];
		CreateIndex(mapping[fieldName], fieldName);		
	}

	//! Create index by field index
	void CreateIndex(const std::size_t fieldIndex, const std::string& fieldName)
	{
		IndexMap indexedValues;

		std::size_t index = 0;
		BOOST_FOREACH(const RowsVector::value_type& row, m_Rows)
		{
			indexedValues.insert(std::make_pair((*row)[fieldIndex], index++));
		}

		m_Indexes[fieldName] = indexedValues;
	}

	//! Remove row from table
	void RemoveRows(const std::vector<int>& rowIndexes)
	{
		BOOST_FOREACH(const int index, rowIndexes)
		{
			// removing row from table data
			google::protobuf::RepeatedPtrField<data::Table_Row>& rows = *m_Table.mutable_rows();

			rows.SwapElements(rows.size() - 1, index);
			rows.RemoveLast();

			// removing rows descriptors
			std::swap(m_Rows[m_Rows.size() - 1], m_Rows[index]);
			m_Rows.resize(m_Rows.size() - 1);
		}
	}

private:

	//! Table reference
	data::Table&			m_Table;

	//! Rows
	RowsVector				m_Rows; 

	//! Indexes
	MapOfIndexes			m_Indexes;
};

CTable::CTable(data::Table& table)
	: m_pImpl(new Impl(table))
{
}

CTable::~CTable()
{
	delete m_pImpl;
}

CTable::Row& CTable::operator[](const std::size_t index)
{
	return m_pImpl->Get(index);
}

const CTable::Row& CTable::operator[](const std::size_t index) const
{
	return m_pImpl->Get(index);
}

CTable::Row& CTable::operator[](const std::string& predicate)
{
	return m_pImpl->Get(predicate);
}

const CTable::Row& CTable::operator[](const std::string& predicate) const
{
	return m_pImpl->Get(predicate);
}

const std::string& CTable::Row::operator[](const std::size_t index) const
{
	return m_pImpl->Get(index);
}

std::string& CTable::Row::operator[](const std::size_t index)
{
	return m_pImpl->Get(index);
}

const std::string& CTable::Row::operator[](const std::string& name) const
{
	return m_pImpl->Get(name);
}

std::string& CTable::Row::operator[](const std::string& name)
{
	return m_pImpl->Get(name);
}

bool CTable::Row::operator == (const data::Table_Row& row) const
{
	return m_pImpl->operator == (row);
}

bool CTable::Row::operator != (const data::Table_Row& row) const
{
	return !(m_pImpl->operator == (row));
}

CTable::Row::Row(const CTableMapping::CColumnMapping& mapping, data::Table_Row& row)
	: m_pImpl(new Impl(mapping, row))
{

}

CTable::Row::~Row()
{
	delete m_pImpl;
}

const CTableMapping::CColumnMapping& CTableMapping::operator[](const data::Table_Id id) const
{
	const TableColumnMap::const_iterator itTable = m_ColumnMap.find(id);
	CHECK(m_ColumnMap.end() != itTable, id);
	return itTable->second;
}

const data::Table_Id CTableMapping::operator[](const CColumnMapping::ColumnMap& mapping) const
{
	BOOST_FOREACH(const TableColumnMap::value_type& pair, m_ColumnMap)
	{
 		if (pair.second == mapping)
 			return pair.first;
	}
	return data::Table_Id_None;
}

const std::string& CTableMapping::Name(const data::Table_Id id) const
{
	const TableNameMap::const_iterator itTable = m_NameMap.find(id);
	CHECK(m_NameMap.end() != itTable, id);
	return itTable->second;	
}

CTableMapping& CTableMapping::Instance()
{
	if (!s_Mapping)
		s_Mapping.reset(new CTableMapping());
	return *s_Mapping;
}

CTableMapping::CColumnMapping::CColumnMapping(const char* columns[])
{
	std::size_t index = 0;
	while (columns[index])
	{
		m_Map.insert(std::make_pair(columns[index], index));
		++index;
	}
}

std::size_t CTableMapping::CColumnMapping::operator[](const std::string& name) const
{
	const ColumnMap::const_iterator itColumn = m_Map.find(name);
	CHECK(m_Map.end() != itColumn, name);

	return itColumn->second;
}

bool CTableMapping::CColumnMapping::operator==(const ColumnMap& that) const
{
	return m_Map == that;
}

bool CTableMapping::CColumnMapping::operator!=(const ColumnMap& that) const
{
	return m_Map != that;
}	

const CTableMapping::CColumnMapping::ColumnMap& CTableMapping::CColumnMapping::Mapping() const
{
	return m_Map;
}

std::ostream& CTable::operator << (std::ostream& stream) const
{
	stream << m_pImpl->ToString();
	return stream;
}

void CTable::Sql(std::ostream& stream)
{
	m_pImpl->Sql(stream);
}

std::size_t CTable::Size() const
{
	return m_pImpl->Size();
}

void CTable::CreateIndex(const std::string& fieldName)
{
	m_pImpl->CreateIndex(fieldName);
}

CTable::iterator CTable::begin()
{
	return m_pImpl->begin();
}

CTable::const_iterator CTable::begin() const
{
	return m_pImpl->begin();
}

CTable::iterator CTable::end()
{
	return m_pImpl->end();
}

CTable::const_iterator CTable::end() const
{
	return m_pImpl->end();
}

CTable::Row& CTable::AddRow()
{
	return m_pImpl->AddRow();
}

void CTable::AddRows(const data::Table& table)
{
	m_pImpl->AddRows(table);
}

void CTable::EraseRows(const data::Table& table)
{
	m_pImpl->EraseRows(table);
}

void CTable::FindRows(IntList& indexes, const std::string& predicate) const
{
	m_pImpl->FindRows(indexes, predicate);
}

CTable::iterator::iterator(const RowsVector::iterator& it)
	: m_It(it)
{

}

CTable::iterator& CTable::iterator::operator++()
{
	++m_It;
	return *this;
}

CTable::iterator::pointer CTable::iterator::operator->()
{
	return m_It->get();
}

CTable::iterator::reference CTable::iterator::operator*()
{
	return **m_It;
}

bool CTable::iterator::operator == (const CTable::iterator& it) const
{
	return m_It == it.m_It;
}
