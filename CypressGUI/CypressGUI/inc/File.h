#pragma once

// Include Project headers
#include "stdafx.h"
#include "Conversion.h"

// Include STL headers
#include <string>
#include <vector>
#include <map>
#include <fstream>

// Include Boost headers
#include <boost/lexical_cast.hpp>

namespace clrn{

//! Объект работы с файлами
class CFile
{
public:

	struct Mode
	{
		enum Enum_t
		{
			eRead,
			eWrite,
			eAppend,
			eReadWrite
		};
	};
	
	//! Конструктор
	CFile(const _tstring& sPath, Mode::Enum_t eMode);

	//! Очистить 
	void Clear();

	//! Записать в файл
	void Write(const std::map< float, float >& Data);

	//! Записать в файл
	void Write(const std::map< int, std::pair< float, float > >& Data);

	//! Прочитать из файла
	void Read(std::map< float, float >& Data);

	//! Прочитать из файла
	void Read(std::map< int, std::pair< float, float > >& Data);

	//! Прочитать из файла
	void Read(_tstring& sData);

	//! Прочитать из файла
	void Read(std::vector< _tstring >& Data);

	//! Открыть
	void Open();

	//! Закрыть
	void Close();

	//! Gets complete path
	static const _tstring GetCompletePath(const _tstring& sPath);

private:

	//! Путь к файлу
	std::wstring m_sPath;

	//! Файловый поток
	_tfstream m_Stream;

	//! Режим работы с файлом
	Mode::Enum_t m_eMode;
};

};