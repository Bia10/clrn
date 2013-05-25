#include "StdAfx.h"
#include "Compiler.h"

CDBCompiler::CDBCompiler(ILog& logger)
	: m_Logger(logger)
{
}

CDBCompiler::~CDBCompiler(void)
{
	DataBase::Shutdown();
}

void CDBCompiler::Create(const std::string& fileName)
{
	boost::filesystem3::remove(fileName);

	DataBase::Create(m_Logger, fileName.c_str());
}

void CDBCompiler::Open(const std::string& fileName)
{
	DataBase::Create(m_Logger, fileName.c_str());
}

void CDBCompiler::ExecuteFile(const std::string& fileName)
{	
	std::ifstream ifs(fileName.c_str());

	const std::size_t size = static_cast<std::size_t>(boost::filesystem3::file_size(fileName));

	std::vector<char> data(size + 1);
	ifs.read(&data.front(), data.size());

	DataBase::Instance().Execute(&data.front());
}