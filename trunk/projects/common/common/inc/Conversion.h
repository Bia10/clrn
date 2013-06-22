#ifndef Conversion_h__
#define Conversion_h__

#include "Exception.h"

#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#pragma warning(push)
#pragma warning(disable:4244) // 'argument' : conversion from 'boost::locale::utf::code_point' to 'const wchar_t', possible loss of data
#include <boost/locale/encoding.hpp>
#pragma warning(pop)
#undef max

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

        //! Specialized help struct - conversion bits from integer to vector
        template<>
        struct Caster<std::vector<unsigned>, unsigned>
        {
            std::vector<unsigned> operator () (const unsigned src)
            {
                std::vector<unsigned> result;

                if (!src)
                    return result;

                unsigned mask = 1;
                unsigned counter = 0;
                for (; mask; mask <<= 1, ++counter)
                {
                    if (src & mask)
                        result.push_back(counter);
                }

                return result;
            }
        };

        //! Specialized help struct - conversion vector of values to bits
        template<>
        struct Caster<unsigned, std::vector<unsigned>>
        {
            unsigned operator () (const std::vector<unsigned>& src)
            {
                unsigned result = 0;

                if (src.empty())
                    return result;

                for (const unsigned bit : src)
                    result |= 1 << bit;

                return result;
            }
        };
	} // namespace detail

	//! Cast function
	template<typename Target, typename Source>
	inline Target cast(const Source& value)
	{
		return detail::Caster<Target, Source>()(value);
	}

	//! Help function for time conversion
	unsigned __int64			ToPosix64(const boost::posix_time::ptime& pt);

	//! Help function for time conversion
	boost::posix_time::ptime	FromPosix64(const unsigned __int64 time);

}


#endif // Conversion_h__