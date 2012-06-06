#ifndef FileSystem_h__
#define FileSystem_h__

#include <string>

namespace fs
{
	
	//! Get initial path
	std::wstring	GetInitialPath();

	//! Build full path
	std::wstring	FullPath(const std::wstring& path);
	std::string		FullPath(const std::string& path);

	//! Object already exists
	bool			Exists(const std::string& path);
	bool			Exists(const std::wstring& path);

	//! Copy
	void			Copy(const std::string& src, const std::string& dst);
	void			Copy(const std::wstring& src, const std::wstring& dst);

	//! Move
	void			Move(const std::string& src, const std::string& dst);
	void			Move(const std::wstring& src, const std::wstring& dst);

	//! Create directories
	void			CreateDirectories(const std::string& src);
	void			CreateDirectories(const std::wstring& src);

} // namespace fs


#endif // FileSystem_h__