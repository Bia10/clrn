#include "FileSystem.h"
#include "Conversion.h"

#include <boost/filesystem.hpp>

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

std::string GetFileName(const std::string& path)
{
	TRY 
	{
		bfs::path filePath(path);
		return filePath.filename().string();
	}
	CATCH_PASS_EXCEPTIONS(path)
}

std::wstring GetFileName(const std::wstring& path)
{
	TRY 
	{
		bfs::path filePath(path);
		return filePath.filename().wstring();
	}
	CATCH_PASS_EXCEPTIONS(conv::cast<std::string>(path))
}

std::string GetDirectory(const std::string& path)
{
	TRY 
	{
		bfs::path filePath(path);
		return filePath.parent_path().string();
	}
	CATCH_PASS_EXCEPTIONS(conv::cast<std::string>(path))
}

std::wstring GetDirectory(const std::wstring& path)
{
	TRY 
	{
		bfs::path filePath(path);
		return filePath.parent_path().wstring();
	}
	CATCH_PASS_EXCEPTIONS(conv::cast<std::string>(path))
}

std::string GetExtension(const std::string& path)
{
	TRY 
	{
		bfs::path filePath(path);
		return filePath.extension().string();
	}
	CATCH_PASS_EXCEPTIONS(conv::cast<std::string>(path))
}

std::wstring GetExtension(const std::wstring& path)
{
	TRY 
	{
		bfs::path filePath(path);
		return filePath.extension().wstring();
	}
	CATCH_PASS_EXCEPTIONS(conv::cast<std::string>(path))
}

} // namespace fs
