#include "stdafx.h"
#include "FileSystem.h"
#include "Conversion.h"

namespace bfs = boost::filesystem3;

namespace fs
{

std::wstring fs::GetInitialPath()
{
	return bfs::initial_path().wstring();
}

std::wstring fs::FullPath(const std::wstring& path)
{
	TRY 
	{
		return bfs::system_complete(path).wstring();
	}
	CATCH_PASS_EXCEPTIONS(conv::cast<std::string>(path))
}

std::string fs::FullPath(const std::string& path)
{
	TRY 
	{
		return bfs::system_complete(path).string();
	}
	CATCH_PASS_EXCEPTIONS(path)
}

bool Exists(const std::string& path)
{
	TRY 
	{
		return bfs::exists(path);
	}
	CATCH_PASS_EXCEPTIONS(path)
}

bool Exists(const std::wstring& path)
{
	TRY 
	{
		return bfs::exists(path);
	}
	CATCH_PASS_EXCEPTIONS(conv::cast<std::string>(path))
}

void Copy(const std::string& src, const std::string& dst)
{
	TRY 
	{
		bfs::copy(src, dst);
	}
	CATCH_PASS_EXCEPTIONS(src, dst)
}

void Copy(const std::wstring& src, const std::wstring& dst)
{
	TRY 
	{
		bfs::copy(src, dst);
	}
	CATCH_PASS_EXCEPTIONS(conv::cast<std::string>(src), conv::cast<std::string>(dst))
}

void Move(const std::string& src, const std::string& dst)
{
	TRY 
	{
		bfs::rename(src, dst);
	}
	CATCH_PASS_EXCEPTIONS(src, dst)
}

void Move(const std::wstring& src, const std::wstring& dst)
{
	TRY 
	{
		bfs::rename(src, dst);
	}
	CATCH_PASS_EXCEPTIONS(conv::cast<std::string>(src), conv::cast<std::string>(dst))
}

void CreateDirectories(const std::string& src)
{
	TRY 
	{
		if (bfs::is_directory(src))
		{
			bfs::create_directories(src);
			return;
		}

		bfs::create_directories(bfs::path(src).parent_path());	
	}
	CATCH_PASS_EXCEPTIONS(src)
}

void CreateDirectories(const std::wstring& src)
{
	TRY 
	{
		if (bfs::is_directory(src))
		{
			bfs::create_directories(src);
			return;
		}
		
		bfs::create_directories(bfs::path(src).parent_path());	
	}
	CATCH_PASS_EXCEPTIONS(conv::cast<std::string>(src))
}

} // namespace fs
