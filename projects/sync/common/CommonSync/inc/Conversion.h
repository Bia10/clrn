#ifndef Conversion_h__
#define Conversion_h__

#include "Exception.h"

#include <string>

#include <boost/lexical_cast.hpp>

#pragma warning(push)
#pragma warning(disable:4244) // 'argument' : conversion from 'boost::locale::utf::code_point' to 'const wchar_t', possible loss of data
#include <boost/locale/encoding.hpp>
#pragma warning(pop)

namespace conv
{
	namespace detail
	{
		//! Customized stream type for boolean
		struct Boolean 
		{
			bool m_Data;
			Boolean() {}
			Boolean(bool data) : m_Data(data) {}
			operator bool() const { return m_Data; }

			template<typename T>
			friend std::basic_ostream<T>& operator << (std::basic_ostream<T> &out, const Boolean& b) 
			{
				out << std::boolalpha << b.m_Data;
				return out;
			}

			template<typename T>
			friend std::basic_istream<T> & operator >> (std::basic_istream<T> &in, Boolean &b) 
			{
				in >> std::boolalpha >> b.m_Data;
				if (in.fail())
				{
					in.unsetf(std::ios_base::boolalpha);
					in.clear();
					in >> b.m_Data;
				}
				return in;
			}
		};

		//! Help template struct
		template<typename Target, typename Source>
		struct Caster
		{
			Target operator () (const Source& src)
			{
				TRY 
				{
					return boost::lexical_cast<Target, Source>(src);
				}
				CATCH_PASS_EXCEPTIONS(src, typeid(Source).name(), typeid(Target).name())
			}
		};

		//! Specialized help struct
		template<typename Source>
		struct Caster<bool, Source>
		{
			bool operator () (const Source& src)
			{
				TRY 
				{
					return boost::lexical_cast<Boolean, Source>(src);
				}
				CATCH_PASS_EXCEPTIONS(src, typeid(Source).name())
			}
		};

		//! Specialized help struct
		template<>
		struct Caster<std::string, std::wstring>
		{
			std::string operator () (const std::wstring& src)
			{
				return boost::locale::conv::from_utf<wchar_t>(src, "utf8");
			}
		};

		//! Specialized help struct
		template<>
		struct Caster<std::wstring, std::string>
		{
			std::wstring operator () (const std::string& src)
			{
				return boost::locale::conv::utf_to_utf<wchar_t, char>(src);
			}
		};
	} // namespace detail

	//! Cast function
	template<typename Target, typename Source>
	inline Target cast(const Source& value)
	{
		return detail::Caster<Target, Source>()(value);
	}

}


#endif // Conversion_h__