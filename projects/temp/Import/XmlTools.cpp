//////////////////////////////////////////////////////////////////////////////// 
//
// VERSION INFO :   $HeadURL: https://clrnprjts.googlecode.com/svn/trunk/projects/temp/Import/XmlTools.cpp $
//                  $Revision: 103 $
//                  $Date: 2012-04-19 17:52:44 +0400 (Чт, 19 апр 2012) $
//                  $Author: clrnmail@gmail.com $
//
// BRIEF        :   Data dispatcher implementation.
//
//////////////////////////////////////////////////////////////////////////////// 
#include "stdafx.h"
#include "XmlTools.h"

#include <boost/property_tree/xml_parser.hpp>

namespace xml
{

namespace detail
{

template<typename TXml>
void Read(const typename TXml::data_type::value_type* inputData, TXml& xmlResult)
{
	boost::property_tree::detail::rapidxml::xml_document<typename TXml::data_type::value_type> doc;
	doc.parse<boost::property_tree::detail::rapidxml::parse_default>(const_cast<typename TXml::data_type::value_type*>(inputData));

	for (boost::property_tree::detail::rapidxml::xml_node<typename TXml::data_type::value_type> *child = doc.first_node(); child; child = child->next_sibling())
		boost::property_tree::xml_parser::read_xml_node(child, xmlResult, 0);	
}

void Read(const wchar_t* inputData, narrow_t& xmlResult)
{
	Read(PT::tostr(inputData).c_str(), xmlResult);
}

void Read(const char* inputData, wide_t& xmlResult)
{
	Read(PT::towstr(inputData).c_str(), xmlResult);
}

template<typename TStream, typename TXml>
void Read(std::basic_istream<TStream>& inputStream, TXml& xmlResult)
{
	// Load data into vector
	inputStream.unsetf(std::ios::skipws);
	std::vector<TStream> vecData(std::istreambuf_iterator<TStream>(inputStream.rdbuf()), std::istreambuf_iterator<TStream>());

	if (!inputStream.good())
		throw std::runtime_error("Read from stream failed.");

	vecData.push_back(0); // zero-terminate

	Read(&vecData.front(), xmlResult);
}


template<typename TXml>
void Write(const TXml& xmlData, std::basic_ostream<typename TXml::data_type::value_type>& outputStream, const std::locale& locale, const bool prettyPrint)
{
	outputStream.imbue(locale);

	boost::property_tree::xml_parser::write_xml<TXml>(
		outputStream, 
		xmlData,
		prettyPrint ? 
			boost::property_tree::xml_parser::xml_writer_make_settings<typename TXml::data_type::value_type>(L' ', 1) : 
			boost::property_tree::xml_parser::xml_writer_make_settings<typename TXml::data_type::value_type>()
	);
}

template<typename TData, typename TXml>
void Write(const TXml& xmlData, std::basic_string<TData>& resultData, const std::locale& locale, const bool prettyPrint)
{
	std::basic_ostringstream<TData> os;
	Write(xmlData, os, locale, prettyPrint);
	resultData.assign(os.str());
}

template<>
void Write(const narrow_t& xmlData, std::wstring& resultData, const std::locale& locale, const bool prettyPrint)
{
	std::ostringstream oss;
	Write(xmlData, oss, locale, prettyPrint);
	resultData.assign(PT::towstr(oss.str()));
}

template<>
void Write(const wide_t& xmlData, std::string& resultData, const std::locale& locale, const bool prettyPrint)
{
	std::wostringstream oss;
	Write(xmlData, oss, locale, prettyPrint);
	resultData.assign(PT::tostr(oss.str()));
}

template<typename TTarget, typename TSource>
TTarget Convert(const TSource& source)
{
	return source;
}

template<>
std::string Convert(const std::wstring& source)
{
	return PT::tostr(source);
}

template<>
std::wstring Convert(const std::string& source)
{
	return PT::towstr(source);
}

template <typename TTarget, typename TSource>
struct Translator
{
	boost::optional<TTarget> get_value(const TSource& v) 
	{ 
		return Convert<TTarget, TSource>(v);
	}
	
	boost::optional<TTarget> put_value(const TSource& v) 
	{ 
		return Convert<TTarget, TSource>(v);
	}
};

//! Make property_tree node from MSXML node
template<typename TNode>
void MakeNode(IXMLDOMNode* node, TNode& xmlResult)
{
	typedef typename TNode::data_type data_t;
	typedef Translator<data_t, std::wstring> trnslt_t;

	PT_CHECK(node);

	CComPtr<IXMLDOMNamedNodeMap> attrsList;
	PT_CHECK(SUCCEEDED(node->get_attributes(&attrsList)));

	long count = 0;
	if (attrsList)
	{
		PT_CHECK(SUCCEEDED(attrsList->get_length(&count)));

		if (count)
		{
			// atributes
			TNode& rootAttr = xmlResult.push_back(std::make_pair(Convert<data_t>(std::wstring(boost::property_tree::xml_parser::xmlattr<WCHAR>())), TNode()))->second;
			for (long index = 0; index < count; ++index)
			{ 
				CComPtr<IXMLDOMNode> attr;
				PT_CHECK(SUCCEEDED(attrsList->get_item(index, &attr)));

				CComBSTR name;
				PT_CHECK(SUCCEEDED(attr->get_nodeName(&name)));

				CComBSTR text;
				PT_CHECK(SUCCEEDED(attr->get_text(&text)));

				TNode &xmlAttr = rootAttr.push_back(std::make_pair(Convert<data_t>(std::wstring(name)), TNode()))->second;
				xmlAttr.put_value(text.operator BSTR(), trnslt_t());
			}
		}
	}

	// childs
	CComPtr<IXMLDOMNodeList> nodesList;
	PT_CHECK(SUCCEEDED(node->get_childNodes(&nodesList)));

	if (!nodesList)
		return;

	PT_CHECK(SUCCEEDED(nodesList->reset()));
	PT_CHECK(SUCCEEDED(nodesList->get_length(&count)));

	if (!count)
	{
		CComBSTR text;
		PT_CHECK(SUCCEEDED(node->get_text(&text)));

		if (text.Length())
			xmlResult.put_value(text.operator BSTR(), trnslt_t());
		return;
	}

	for (long index = 0; index < count; ++index)
	{ 
		CComPtr<IXMLDOMNode> node;
		PT_CHECK(SUCCEEDED(nodesList->get_item(index, &node)));

		CComBSTR name;
		PT_CHECK(SUCCEEDED(node->get_nodeName(&name)));

		if (name == L"#text" || name == L"#cdata-section")
		{
			CComBSTR text;
			PT_CHECK(SUCCEEDED(node->get_text(&text)));

			xmlResult.put_value(text.operator BSTR(), trnslt_t());
			continue;
		}

		TNode& xmlChild = xmlResult.push_back(std::make_pair(Convert<data_t>(std::wstring(name)), TNode()))->second;

		// recursively down
		MakeNode(node, xmlChild);
	}
}

// Build property tree from MSXML document
template<typename TNode>
void MakeTree(IXMLDOMDocument* doc, TNode& xmlResult)
{
	PT_CHECK(doc);

	CComPtr<IXMLDOMElement> root;
	PT_CHECK_DSC(SUCCEEDED(doc->get_documentElement(&root)), L"Can't get root element for xml-document");	

	CComBSTR name;
	PT_CHECK(SUCCEEDED(root->get_nodeName(&name)));

	TNode &xmlRoot = xmlResult.push_back(std::make_pair(Convert<typename TNode::data_type>(std::wstring(name)), TNode()))->second;

	MakeNode(root, xmlRoot);
}

} // namespace detail

void Read(const char* const data, wide_t& xmlResult)
{
	detail::Read<wide_t>(PT::towstr(data).c_str(), xmlResult);
}

void Read(const wchar_t* const data, wide_t& xmlResult)
{
	detail::Read<wide_t>(data, xmlResult);
}

void Read(std::istream& inputStream, wide_t& xmlResult)
{
	detail::Read(inputStream, xmlResult);
}

void Read(std::wistream& inputStream, wide_t& xmlResult)
{
	detail::Read(inputStream, xmlResult);
}

void Read(const char* const data, narrow_t& xmlResult)
{
	detail::Read<narrow_t>(data, xmlResult);
}

void Read(const wchar_t* const data, narrow_t& xmlResult)
{
	detail::Read<narrow_t>(PT::tostr(data).c_str(), xmlResult);
}

void Read(std::istream& inputStream, narrow_t& xmlResult)
{
	detail::Read(inputStream, xmlResult);
}

void Read(std::wistream& inputStream, narrow_t& xmlResult)
{
	detail::Read(inputStream, xmlResult);
}

void Write(const wide_t& xmlData, std::string& resultData, bool prettyPrint /*= false*/, const std::locale& locale /*= std::locale(std::locale(), new utf8_conversion())*/)
{
	detail::Write(xmlData, resultData, locale, prettyPrint);
}

void Write(const wide_t& xmlData, std::wstring& resultData, bool prettyPrint /*= false*/, const std::locale& locale /*= std::locale(std::locale(), new utf8_conversion())*/)
{
	detail::Write(xmlData, resultData, locale, prettyPrint);
}

void Write(const wide_t& xmlData, std::wostream& outputStream, bool prettyPrint /*= false*/, const std::locale& locale /*= std::locale(std::locale(), new utf8_conversion())*/)
{
	detail::Write(xmlData, outputStream, locale, prettyPrint);
}

void Write(const narrow_t& xmlData, std::string& resultData, bool prettyPrint /*= false*/, const std::locale& locale /*= std::locale(std::locale(), new utf8_conversion())*/)
{
	detail::Write(xmlData, resultData, locale, prettyPrint);
}

void Write(const narrow_t& xmlData, std::wstring& resultData, bool prettyPrint /*= false*/, const std::locale& locale /*= std::locale(std::locale(), new utf8_conversion())*/)
{
	detail::Write(xmlData, resultData, locale, prettyPrint);
}

void Write(const narrow_t& xmlData, std::ostream& outputStream, bool prettyPrint /*= false*/, const std::locale& locale /*= std::locale(std::locale(), new utf8_conversion())*/)
{
	detail::Write(xmlData, outputStream, locale, prettyPrint);
}

void Read(IXMLDOMDocument* doc, wide_t& xmlResult)
{
	detail::MakeTree(doc, xmlResult);
}

void Read(IXMLDOMDocument* doc, narrow_t& xmlResult)
{
	detail::MakeTree(doc, xmlResult);
}

} // namespace xml
