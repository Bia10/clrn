#ifndef VR_STR_CONVERTER_HPP
#define VR_STR_CONVERTER_HPP

#include "stdafx.h"
#include <string>
#include <memory> // either from boost::tr1 or VS8 distribution.
#include <locale>

#include <boost/shared_ptr.hpp>

namespace vr { namespace str {

    std::string  narrow(const std::wstring& wide, const std::locale& locale);
    std::wstring widen(const std::string& narrow, const std::locale& locale);

    class any_string
    {
    public:
        explicit any_string(const char *,        const std::locale& locale = std::locale("") );
        explicit any_string(const wchar_t *,     const std::locale& locale = std::locale("") );
        explicit any_string(const std::string&,  const std::locale& locale = std::locale("") );
        explicit any_string(const std::wstring&, const std::locale& locale = std::locale("") );

        const std::string&  str()  const;
        const std::wstring& wstr() const;

        operator const std::string&() const  { return str(); }
        operator const std::wstring&() const { return wstr(); }

    private:
        const   std::locale                        m_locale;
        mutable boost::shared_ptr<std::string>  m_ansi;
        mutable boost::shared_ptr<std::wstring> m_wide;
    };

    class str_converter
    {
    public:
        str_converter(const std::locale& locale = std::locale("") );

        any_string operator()(const char * str)        { return any_string(str, m_locale); }
        any_string operator()(const wchar_t * str)     { return any_string(str, m_locale); }
        any_string operator()(const std::string& str)  { return any_string(str, m_locale); }
        any_string operator()(const std::wstring& str) { return any_string(str, m_locale); }

    private:
        const std::locale m_locale;
    };

}} // namespace vr::str
#endif // VR_STR_CONVERTER_HPP