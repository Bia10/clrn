#include "stdafx.h"
#include "FileSystem.h"

namespace bfs = boost::filesystem3;

namespace fs
{

std::wstring fs::GetInitialPath()
{
	return bfs::initial_path().wstring();
}

std::wstring fs::FullPath(const std::wstring& path)
{
	return bfs::system_complete(path).wstring();
}

std::string fs::FullPath(const std::string& path)
{
	return bfs::system_complete(path).string();
}

bool Exists(const std::string& path)
{
	return bfs::exists(path);
}

bool Exists(const std::wstring& path)
{
	return bfs::exists(path);
}

void Copy(const std::string& src, const std::string& dst)
{
	bfs::copy(src, dst);
}

void Copy(const std::wstring& src, const std::wstring& dst)
{
	bfs::copy(src, dst);
}

void Move(const std::string& src, const std::string& dst)
{
	bfs::rename(src, dst);
}

void Move(const std::wstring& src, const std::wstring& dst)
{
	bfs::rename(src, dst);
}

} // namespace fs
