#include "StdAfx.h"
#include "File.h"
#include "Exception.h"

#include <boost/filesystem.hpp>

namespace clrn{

CFile::CFile(const _tstring& sPath, Mode::Enum_t eMode) :
	m_sPath(sPath),
	m_eMode(eMode)
{
	Open();
}

void CFile::Clear()
{
	_tofstream ofs(m_sPath.c_str(), std::ios_base::out);
	ofs.close();
}


void CFile::Open()
{
	m_sPath = GetCompletePath(m_sPath);

	switch (m_eMode) 
	{
	case Mode::eAppend:
		m_Stream.open(m_sPath.c_str(), std::ios_base::app);
		break;
	case Mode::eWrite:
		m_Stream.open(m_sPath.c_str(), std::ios_base::out);
		break;
	case Mode::eRead:
		m_Stream.open(m_sPath.c_str(), std::ios_base::in);
		break;
	case Mode::eReadWrite:
		m_Stream.open(m_sPath.c_str(), std::ios_base::out | std::ios_base::in);
		break;
	}

	if (!m_Stream.is_open())
	{
		std::ostringstream oss;
		oss << "Can't open file! Path: " << boost::filesystem::path(m_sPath).string();
		THROW_MESSAGE(oss.str().c_str());
		throw std::runtime_error(oss.str().c_str());
	}
}


void CFile::Close()
{
	m_Stream.close();
}


void CFile::Write(const std::map< float, float >& Data)
{
	std::map< float, float >::const_iterator it = Data.begin();
	const std::map< float, float >::const_iterator itEnd = Data.end();
	for (; it != itEnd; ++it)
		m_Stream << it->first << "=" << it->second << std::endl;
}


void CFile::Read(std::map< float, float >& Data)
{
	std::vector< _tstring > TxtSource;
	TxtSource.assign(std::istream_iterator< _tstring, TCHAR > (m_Stream), std::istream_iterator< _tstring, TCHAR > ());

	std::vector< _tstring >::const_iterator it = TxtSource.begin();
	const std::vector< _tstring >::const_iterator itEnd = TxtSource.end();
	for (; it != itEnd; ++it)
	{
		const _tstring sCurrent = *it;
		const _tstring::size_type nEqual = sCurrent.find('=');
		if (nEqual == _tstring::npos)
			continue;
		const _tstring sName(sCurrent.c_str(), &sCurrent.c_str()[nEqual]);
		const _tstring sValue(&sCurrent.c_str()[nEqual + 1]);
		Data[boost::lexical_cast< float > (sName)] = boost::lexical_cast< float > (sValue);
	}
}

void CFile::Read(std::vector< _tstring >& Data)
{
	Data.assign(std::istream_iterator< _tstring, TCHAR > (m_Stream), std::istream_iterator< _tstring, TCHAR > ());
}

void CFile::Read(_tstring& sData)
{
	sData.assign(std::istream_iterator< TCHAR, TCHAR > (m_Stream), std::istream_iterator< TCHAR, TCHAR > ());
}

void CFile::Write(const std::map< int, std::pair< float, float > >& Data)
{
	std::map< int, std::pair< float, float > >::const_iterator it = Data.begin();
	const std::map< int, std::pair< float, float > >::const_iterator itEnd = Data.end();	
	for (; it != itEnd; ++it)
		m_Stream << "Frequency=" << it->first << " Amplitude=" << it->second.first << " Phase=" << it->second.second << std::endl;
}

void CFile::Read(std::map< int, std::pair< float, float > >& Data)
{
	std::vector< _tstring > TxtSource;
	TxtSource.assign(std::istream_iterator< _tstring, TCHAR > (m_Stream), std::istream_iterator< _tstring, TCHAR > ());

	int nFrequency = 0;
	float nAmplitude = 0;
	float nPhase = 0;

	std::vector< _tstring >::const_iterator it = TxtSource.begin();
	const std::vector< _tstring >::const_iterator itEnd = TxtSource.end();
	for (; it != itEnd; ++it)
	{
		const _tstring sCurrent = *it;
		{
			const _tstring::size_type nFrequencyBegin = sCurrent.find(_T("Frequency="));
			if (nFrequencyBegin != _tstring::npos)
			{
				const _tstring sValue(&sCurrent.c_str()[nFrequencyBegin + 10]);
				nFrequency = boost::lexical_cast< int > (sValue);
			}
		}

		{
			const _tstring::size_type nFrequencyBegin = sCurrent.find(_T("Amplitude="));
			if (nFrequencyBegin != _tstring::npos)
			{

				const _tstring sValue(&sCurrent.c_str()[nFrequencyBegin + 10]);
				nAmplitude = boost::lexical_cast< float > (sValue);
			}
		}

		{
			const _tstring::size_type nFrequencyBegin = sCurrent.find(_T("Phase="));
			if (nFrequencyBegin != _tstring::npos)		
			{
				const _tstring sValue(&sCurrent.c_str()[nFrequencyBegin + 6]);
				nPhase = boost::lexical_cast< float > (sValue);

				Data[nFrequency] = std::pair< float, float > (nAmplitude, nPhase);
			}
		}
	}
}

const _tstring CFile::GetCompletePath( const _tstring& sPath )
{
#ifdef _UNICODE
	return boost::filesystem3::complete(sPath, boost::filesystem::initial_path()).wstring();
#elif
	return boost::filesystem3::complete(sPath, boost::filesystem::initial_path()).string();
#endif // _UNICODE
}
};