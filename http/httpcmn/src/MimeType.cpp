/*
 * MimeType.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#include "MimeType.h"
#include "Exception.h"

#include <map>

#include <boost/algorithm/string/case_conv.hpp>

namespace http
{

CMimeType::CMimeType(const Type& eType) :
		m_eType(eType)
{

}

CMimeType::CMimeType(const _tstring & sExtension)
{
	SetFromFileExtension(sExtension);
}

CMimeType::~CMimeType()
{

}

void CMimeType::Set(const Type & eType)
{
	m_eType = eType;
}

void CMimeType::SetFromFileExtension(const _tstring & sExtension)
{
	static std::map< _tstring, Type > s_mapTypes;

	TRY_EXCEPTIONS
	{
		if (s_mapTypes.empty())
		{
			s_mapTypes[_T("html")] 		= Type::eHtml;
			s_mapTypes[_T("htm")] 		= Type::eHtml;
			s_mapTypes[_T("txt")] 		= Type::eText;
			s_mapTypes[_T("gif")] 		= Type::eGif;
			s_mapTypes[_T("jpeg")]		= Type::eJpeg;
			s_mapTypes[_T("jpg")]		= Type::eJpeg;
			s_mapTypes[_T("png")] 		= Type::ePng;
			s_mapTypes[_T("pdf")] 		= Type::ePdf;
			s_mapTypes[_T("ico")] 		= Type::eIco;
			s_mapTypes[_T("js")] 		= Type::eJs;
			s_mapTypes[_T("css")] 		= Type::eCss;
			s_mapTypes[_T("php")] 		= Type::ePhp;
			s_mapTypes[_T("swf")] 		= Type::eSwf;
			s_mapTypes[_T("xml")] 		= Type::eXml;
			s_mapTypes[_T("json")] 		= Type::eJson;
		}

		_tstring sExt(sExtension);
		boost::algorithm::to_lower(sExt);

		const std::map< _tstring, Type >::const_iterator it = s_mapTypes.find(sExt);
		if(it == s_mapTypes.end())
		{
			_tostringstream ossError;
			ossError << "Can't convert extension " << sExt << " to mime type." << std::endl;
			THROW_EXCEPTION_CODE(cmn::CStatus::StatusCode::eNotImplemented, ossError.str());
		}

		m_eType = it->second;
	}
	CATCH_PASS_EXCEPTIONS
}

const _tstring& CMimeType::ToString() const
{
	static std::map< Type, _tstring > s_mapTypes;

	TRY_EXCEPTIONS
	{
		if (s_mapTypes.empty())
		{
			s_mapTypes[Type::eHtml] 	= _T("text/html");
			s_mapTypes[Type::eText] 	= _T("text/html");
			s_mapTypes[Type::eGif] 		= _T("image/gif");
			s_mapTypes[Type::eJpeg]		= _T("image/jpeg");
			s_mapTypes[Type::ePng] 		= _T("image/png");
			s_mapTypes[Type::ePdf] 		= _T("text/pdf");
			s_mapTypes[Type::eIco] 		= _T("image/ico");
			s_mapTypes[Type::eJs] 		= _T("text/javascript");
			s_mapTypes[Type::eCss] 		= _T("text/css");
			s_mapTypes[Type::ePhp] 		= _T("text/php");
			s_mapTypes[Type::eSwf] 		= _T("text/swf");
			s_mapTypes[Type::eXml] 		= _T("text/xml");
			s_mapTypes[Type::eJson] 	= _T("text/json");
		}

		const std::map< Type, _tstring >::const_iterator it = s_mapTypes.find(m_eType);
		if(it == s_mapTypes.end())
		{
			_tostringstream ossError;
			ossError << "Can't convert mime type " << static_cast< int > (m_eType) << " to string." << std::endl;
			THROW_EXCEPTION_CODE(cmn::CStatus::StatusCode::eNotImplemented, ossError.str());
		}

		return it->second;
	}
	CATCH_PASS_EXCEPTIONS
}


} /* namespace http */
