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

//! ������ ������ � �������
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
	
	//! �����������
	CFile(const _tstring& sPath, Mode::Enum_t eMode);

	//! �������� 
	void Clear();

	//! �������� � ����
	void Write(const std::map< float, float >& Data);

	//! �������� � ����
	void Write(const std::map< int, std::pair< float, float > >& Data);

	//! ��������� �� �����
	void Read(std::map< float, float >& Data);

	//! ��������� �� �����
	void Read(std::map< int, std::pair< float, float > >& Data);

	//! ��������� �� �����
	void Read(_tstring& sData);

	//! ��������� �� �����
	void Read(std::vector< _tstring >& Data);

	//! �������
	void Open();

	//! �������
	void Close();

	//! Gets complete path
	static const _tstring GetCompletePath(const _tstring& sPath);

private:

	//! ���� � �����
	std::wstring m_sPath;

	//! �������� �����
	_tfstream m_Stream;

	//! ����� ������ � ������
	Mode::Enum_t m_eMode;
};

};