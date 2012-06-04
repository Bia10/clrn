#pragma once

#include <boost/property_tree/ptree_fwd.hpp>
#include "Common/Common/codecvt.h"

#include <list>

//! Forward declarations
struct IXMLDOMDocument;

//! Macro for attributes mapping
#define ATTR(name) L"<xmlattr>." ## name

namespace xml
{
	static const std::wstring xml_attr = L"<xmlattr>";

	typedef boost::property_tree::wiptree			wide_t;
	typedef boost::property_tree::iptree			narrow_t;

	typedef std::pair<const std::wstring, wide_t>	wide_value_t;
	typedef std::pair<const std::string, narrow_t>	narrow_value_t;

	void Read(const char* const data, wide_t& xmlResult);
	void Read(const wchar_t* const data, wide_t& xmlResult);
	void Read(std::istream& inputStream, wide_t& xmlResult);
	void Read(std::wistream& inputStream, wide_t& xmlResult);

	void Read(const char* const data, narrow_t& xmlResult);
	void Read(const wchar_t* const data, narrow_t& xmlResult);
	void Read(std::istream& inputStream, narrow_t& xmlResult);
	void Read(std::wistream& inputStream, narrow_t& xmlResult);

	void Read(IXMLDOMDocument* doc, wide_t& xmlResult);
	void Read(IXMLDOMDocument* doc, narrow_t& xmlResult);

	void Write(const wide_t& xmlData, std::string& resultData, bool prettyPrint = false, const std::locale& locale = std::locale(std::locale(), new utf8_conversion()));
	void Write(const wide_t& xmlData, std::wstring& resultData, bool prettyPrint = false, const std::locale& locale = std::locale(std::locale(), new utf8_conversion()));
	void Write(const wide_t& xmlData, std::wostream& outputStream, bool prettyPrint = false, const std::locale& locale = std::locale(std::locale(), new utf8_conversion()));

	void Write(const narrow_t& xmlData, std::string& resultData, bool prettyPrint = false, const std::locale& locale = std::locale(std::locale(), new utf8_conversion()));
	void Write(const narrow_t& xmlData, std::wstring& resultData, bool prettyPrint = false, const std::locale& locale = std::locale(std::locale(), new utf8_conversion()));
	void Write(const narrow_t& xmlData, std::ostream& outputStream, bool prettyPrint = false, const std::locale& locale = std::locale(std::locale(), new utf8_conversion()));
	
	//! Simple convert narrow to wide
	template<typename TTarget, typename TSource>
	TTarget cvrt(const TSource& src)
	{
		TTarget result;
		result.reserve(src.size());

		TSource::const_iterator it = src.begin();
		const TSource::const_iterator itEnd = src.end();
		for (; it != itEnd; ++it)
			result.push_back(TTarget::value_type(*it));

		return result;
	}
	
	//! Find algorithm
	template
	<
		typename TNode, 
		typename TPredicate
	>
	void Find
	(
		TNode& xmlSource, 
		const TNode& xmlMatch, 
		std::list<typename TNode::second_type::iterator>& result, 
		const TPredicate& predicate, 
		const std::size_t level = -1
	)
	{
		if (!level)
			return;

		TNode::second_type::iterator it = xmlSource.second.begin();
		const TNode::second_type::const_iterator itEnd = xmlSource.second.end();
		for (; it != itEnd; ++it)
		{
			if (predicate(*it, xmlMatch))
				result.push_back(it);

			Find(*it, xmlMatch, result, predicate, level - 1);
		}
	}

	//! Const find algorithm
	template
	<
		typename TNode, 
		typename TPredicate
	>
	void Find
	(
		const TNode& xmlSource, 
		const TNode& xmlMatch, 
		std::list<typename TNode::second_type::const_iterator>& result, 
		const TPredicate& predicate, 
		const std::size_t level = -1
	)
	{
		if (!level)
			return;

		TNode::second_type::const_iterator it = xmlSource.second.begin();
		const TNode::second_type::const_iterator itEnd = xmlSource.second.end();
		for (; it != itEnd; ++it)
		{
			if (predicate(*it, xmlMatch))
				result.push_back(it);

			Find(*it, xmlMatch, result, predicate, level - 1);
		}
	}

	//! Xml find predicate
	template
	<
		typename TNode,
		template <class> class TNameMatcher
	>
	struct NodeCompare
	{
		bool operator() (const TNode& xmlSource, const TNode& xmlMatch) const
		{
			// checking names
			if (!m_NameMatcher(xmlSource.first, xmlMatch.first))
				return false;

			// checking data if exists
			if (!xmlMatch.second.data().empty() && xmlMatch.second.data() != xmlSource.second.data())
				return false;

			// checking attributes
			const TNode::second_type::data_type attrName = cvrt<typename TNode::second_type::data_type>(xml_attr);
			const boost::optional<typename const TNode::second_type&> matchAttrs = xmlMatch.second.get_child_optional(attrName);
			const boost::optional<typename const TNode::second_type&> sourceAttrs = xmlSource.second.get_child_optional(attrName);

			if (!matchAttrs && !sourceAttrs)
				return true;

			if (matchAttrs && !sourceAttrs)
				return false;

			// checking size
			if (matchAttrs.get().size() > sourceAttrs.get().size())
				return false;

			// checking values
			TNode::second_type::const_iterator it = matchAttrs.get().begin();
			const TNode::second_type::const_iterator itEnd = matchAttrs.get().end();
			for (; it != itEnd; ++it)
			{
				// finding attribute
				const TNode::second_type::const_assoc_iterator itAttr = sourceAttrs.get().find(it->first);
				if (itAttr == sourceAttrs.get().not_found())
				{
					// attribute not found
					return false;
				}

				// checking attribute value
				const TNode::second_type::data_type& rawMatchData = it->second.data();
				const TNode::second_type::data_type& rawSourceData = itAttr->second.data();
				if (!rawMatchData.empty() && rawMatchData != rawSourceData)
				{
					// value not matched
					return false;
				}
			}

			return true;
		}
	private:

		TNameMatcher<typename TNode::second_type::data_type> m_NameMatcher;
	};

	//! Name matcher
	template<typename T>
	struct NameMatcher
	{
		bool operator() (const T& source, const T& matchName) const
		{
			if (!matchName.empty() && source.find(matchName) == source.npos)
				return false;
			return true;
		}
	};

	//! Exact name matcher
	template<typename T>
	struct ExactNameMatcher
	{
		bool operator() (const T& source, const T& matchName) const
		{
			if (!matchName.empty() && source != matchName)
				return false;
			return true;
		}
	};


} // namespace xml

