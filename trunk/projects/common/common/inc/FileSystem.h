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

	//! Get file name
	std::string		GetFileName(const std::string& path);
	std::wstring	GetFileName(const std::wstring& path);

	//! Get directory
	std::string		GetDirectory(const std::string& path);
	std::wstring	GetDirectory(const std::wstring& path);

	//! Get extension
	std::string		GetExtension(const std::string& path);
	std::wstring	GetExtension(const std::wstring& path);

	//! Remove file
	void			Remove(const std::string& path);
	void			Remove(const std::wstring& path);


} // namespace fs


#endif // FileSystem_h__