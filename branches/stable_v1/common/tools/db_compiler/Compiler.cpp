#include "StdAfx.h"
#include "Compiler.h"
#include "SQLiteDB.h"

CDBCompiler::CDBCompiler(ILog& logger)
	: m_Logger(logger)
	, m_Database(new sql::SQLiteDataBase(logger))
{
}

CDBCompiler::~CDBCompiler(void)
{

}

void CDBCompiler::Create(const std::string& fileName)
{
	boost::filesystem::remove(fileName);

	m_Database->Open(fileName);
}

void CDBCompiler::Open(const std::string& fileName)
{
	m_Database->Open(fileName);
}

void CDBCompiler::ExecuteFile(const std::string& fileName)
{	
	std::ifstream ifs(fileName.c_str());

	const std::size_t size = static_cast<std::size_t>(boost::filesystem::file_size(fileName));

	std::vector<char> data(size + 1);
	ifs.read(&data.front(), data.size());
	m_Database->Execute(&data.front());
}