#include "stdafx.h"
#include "Settings.h"

//! Settings implementation
//!
//! \class CSettings
//!
class CSettings::Impl : boost::noncopyable
{
public:
	Impl(ILog& logger, DataBase& dataBase, data::Table& data)
		: m_Log(logger)
		, m_DataBase(dataBase)
		, m_pData(&data)
	{
		m_pTableWrapper.reset(new CTable(data));
	}

	~Impl()
	{
	}

	template<typename T>
	inline void Get(const int module, T& value, const std::string& path)
	{
		CHECK(m_pData, conv::cast<std::string>(value), path);
		CHECK(m_pTableWrapper, conv::cast<std::string>(value), path, *m_pData);

		BOOST_FOREACH(const CTable::Row& row, *m_pTableWrapper)
		{
			const int currentModule = conv::cast<int>(row["module"]);
			if (currentModule != module)
				continue;

			const std::string& valueName = row["name"];
			if (valueName != path)
				continue;

			value = conv::cast<T>(row["value"]);
			return;
		}
		bool finded = false;
		CHECK(finded, "Row not found", module, conv::cast<std::string>(value), path);
	}

	template<typename T>
	inline void Get(const int module, std::vector<T>& value, const std::string& path)
	{
		CHECK(m_pData, path);
		CHECK(m_pTableWrapper, path, *m_pData);

		BOOST_FOREACH(const CTable::Row& row, *m_pTableWrapper)
		{
			const int currentModule = conv::cast<int>(row["module"]);
			if (currentModule != module)
				continue;

			const std::string& valueName = row["name"];
			if (valueName != path)
				continue;

			value.push_back(conv::cast<T>(row["value"]));
		}
	}

	template<typename T>
	inline void Set(const int module, const T& value, const std::string& path)
	{
		CHECK(m_pData, conv::cast<std::string>(value), path);

		BOOST_FOREACH(CTable::Row& row, *m_pTableWrapper)
		{
			const int currentModule = conv::cast<int>(row["module"]);
			if (currentModule != module)
				continue;

			const std::string& valueName = row["name"];
			if (valueName != path)
				continue;

			row["value"] = conv::cast<std::string>(value);
			return;
		}
		bool finded = false;
		CHECK(finded, "Row not found", module, conv::cast<std::string>(value), path);
	}

	void Save()
	{
		SCOPED_LOG(m_Log);

		TRY 
		{
			CHECK(m_pData);

			CTable table(*m_pData);

			std::ostringstream oss;
			table.Sql(oss);

			LOG_TRACE("SQL: [%s]") % oss.str();

			m_DataBase.Execute(oss.str().c_str());
		}
		CATCH_PASS_EXCEPTIONS("Load failed.")
	}

	void Load()
	{
		SCOPED_LOG(m_Log);

		TRY 
		{	
			CHECK(m_pData);

			m_DataBase.Execute((std::string("select * from ") + SETTINGS_TABLE_NAME).c_str(), *m_pData);
			m_pTableWrapper.reset(new CTable(*m_pData));
		}
		CATCH_PASS_EXCEPTIONS("Load failed.")
	}

	void SetLocalGuid(const std::string& guid)
	{
		m_LocalGuid = guid;
	}

	void SetDBpath(const std::string& path)
	{
		m_DBPath = path;
	}

	const std::string& DbPath() const
	{
		return m_DBPath;
	}

	const std::string& LocalGuid() const
	{
		return m_LocalGuid;
	}

private:

	//! Logger reference 
	ILog&						m_Log;

	//! SQLite dataBase
	DataBase&					m_DataBase;

	//! Settings data
	data::Table*				m_pData;

	//! Table wrapper
	boost::scoped_ptr<CTable>	m_pTableWrapper;

	//! DB path
	std::string					m_DBPath;

	//! Local guid
	std::string					m_LocalGuid;
};

CSettings::CSettings(ILog& logger, DataBase& dataBase, data::Table& data)
	: m_pImpl(new Impl(logger, dataBase, data))
{

}

CSettings::~CSettings(void)
{
}

void CSettings::Set(const int module, const unsigned int value, const std::string& path)
{
	m_pImpl->Set(module, value, path);
}

void CSettings::Set(const int module, const int value, const std::string& path)
{
	m_pImpl->Set(module, value, path);
}

void CSettings::Set(const int module, const double value, const std::string& path)
{
	m_pImpl->Set(module, value, path);
}

void CSettings::Set(const int module, const std::string& value, const std::string& path)
{
	m_pImpl->Set(module, value, path);
}

void CSettings::Set(const int module, const std::wstring& value, const std::string& path)
{
	m_pImpl->Set(module, value, path);
}

void CSettings::Get(const int module, unsigned int& value, const std::string& path)
{
	m_pImpl->Get(module, value, path);
}

void CSettings::Get(const int module, int& value, const std::string& path)
{
	m_pImpl->Get(module, value, path);
}

void CSettings::Get(const int module, double& value, const std::string& path)
{
	m_pImpl->Get(module, value, path);
}

void CSettings::Get(const int module, std::string& value, const std::string& path)
{
	m_pImpl->Get(module, value, path);
}

void CSettings::Get(const int module, std::wstring& value, const std::string& path)
{
	m_pImpl->Get(module, value, path);
}

void CSettings::Get(const int module, std::vector<std::wstring>& value, const std::string& path)
{
	m_pImpl->Get(module, value, path);
}

void CSettings::Get(const int module, std::vector<unsigned int>& value, const std::string& path)
{
	m_pImpl->Get(module, value, path);
}

void CSettings::Save()
{
	m_pImpl->Save();
}

void CSettings::Load()
{
	m_pImpl->Load();
}

void CSettings::SetLocalGuid(const std::string& guid)
{
	m_pImpl->SetLocalGuid(guid);
}

void CSettings::SetDBpath(const std::string& path)
{
	m_pImpl->SetDBpath(path);
}

const std::string& CSettings::DbPath() const
{
	return m_pImpl->DbPath();
}

const std::string& CSettings::LocalGuid() const
{
	return m_pImpl->LocalGuid();
}

const std::size_t CSettings::PingInterval() const
{
	std::size_t pingInterval = 0;
	m_pImpl->Get(KERNEL_MODULE_ID, pingInterval, "ping_interval");
	return pingInterval;
}

const std::size_t CSettings::ThreadsCount() const
{
	std::size_t cnt = 0;
	m_pImpl->Get(KERNEL_MODULE_ID, cnt, "kernel_threads");
	return cnt;
}

const std::size_t CSettings::UDPPort() const
{
	std::size_t port = 0;
	m_pImpl->Get(KERNEL_MODULE_ID, port, "udp_port");
	return port;
}

const std::size_t CSettings::BufferSize() const
{
	std::size_t size = 0;
	m_pImpl->Get(KERNEL_MODULE_ID, size, "udp_buffer_size");
	return size;
}

const std::size_t CSettings::JobTimeout() const
{
	return 1000 * 1000;
}
