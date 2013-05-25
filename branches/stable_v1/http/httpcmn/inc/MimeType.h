/*
 * MimeType.h
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#ifndef MIMETYPE_H_
#define MIMETYPE_H_

#include "Types.h"

namespace http
{

class CMimeType
{
public:

	enum class Type
	{
		eHtml,
		eText,
		eGif,
		eJpeg,
		ePng,
		ePdf,
		eIco,
		eJs,
		eCss,
		ePhp,
		eSwf,
		eXml,
		eJson
	};

	CMimeType() = delete;
	CMimeType(const Type& eType);
	CMimeType(const _tstring& sExtension);
	~CMimeType();

	//! Set type
	void 				Set(const Type& eType);

	//! From string
	void 				SetFromFileExtension(const _tstring& sExtension);

	//! To string
	const _tstring& 	ToString() const;

private:

	//! Type
	Type				m_eType;
};

} /* namespace http */
#endif /* MIMETYPE_H_ */
