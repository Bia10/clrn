#include "stdafx.h"
#include "Conversion.h"

#include <stdexcept>

namespace vr { namespace str {

std::string narrow(const std::wstring& wide, const std::locale& locale)
{
    if( wide.empty() )
        return std::string();

    typedef std::wstring::traits_type::state_type state_type;
    typedef std::codecvt<wchar_t, char, state_type> CVT;

    const CVT& cvt = std::use_facet<CVT>(locale);
    std::string narrow(cvt.max_length()*wide.size(), '\0');
    state_type state = state_type();

    const wchar_t* from_beg = &wide[0];
    const wchar_t* from_end = from_beg + wide.size();
    const wchar_t* from_nxt;
    char* to_beg = &narrow[0];
    char* to_end = to_beg + narrow.size();
    char* to_nxt;

    std::string::size_type sz = 0;
    std::codecvt_base::result r;
    do
    {
        r = cvt.out(state, from_beg, from_end, from_nxt,
                           to_beg,   to_end,   to_nxt);
        switch (r)
        {
        case std::codecvt_base::error:
            throw std::runtime_error("Cannot convert wstring to string. Locale: "+locale.name());

        case std::codecvt_base::partial:
            sz += to_nxt - to_beg;
            narrow.resize(2*narrow.size());
            to_beg = &narrow[sz];
            to_end = &narrow[0] + narrow.size();
            break;

        case std::codecvt_base::noconv:
            narrow.resize(sz + (from_end-from_beg)*sizeof(wchar_t));
            std::memcpy(&narrow[sz], from_beg,(from_end-from_beg)*sizeof(wchar_t));
            r = std::codecvt_base::ok;
            break;

        case std::codecvt_base::ok:
            sz += to_nxt - to_beg;
            narrow.resize(sz);
            break;
        }
    } while( r != std::codecvt_base::ok );

    return narrow;
} 

std::wstring widen(const std::string& narrow, const std::locale& locale)
{
    if( narrow.empty() )
        return std::wstring();

    typedef std::string::traits_type::state_type state_type;
    typedef std::codecvt<wchar_t, char, state_type> CVT;

    const CVT& cvt = std::use_facet<CVT>(locale);
    std::wstring wide(narrow.size(), '\0');
    state_type state = state_type();
    const char* from_beg = &narrow[0];
    const char* from_end = from_beg + narrow.size();
    const char* from_nxt;
    wchar_t* to_beg = &wide[0];
    wchar_t* to_end = to_beg + wide.size();
    wchar_t* to_nxt;

    std::wstring::size_type sz = 0;
    std::codecvt_base::result r;
    do
    {
        r = cvt.in(state, from_beg, from_end, from_nxt,
                          to_beg,   to_end,   to_nxt);
        switch (r)
        {
        case std::codecvt_base::error:
            throw std::runtime_error("Cannot convert string to wstring. Locale: "+locale.name());

        case std::codecvt_base::partial:
            sz += to_nxt - to_beg;
            wide.resize(2*wide.size());
            to_beg = &wide[sz];
            to_end = &wide[0] + wide.size();
            break;

        case std::codecvt_base::noconv:
            wide.resize(sz + (from_end-from_beg));
            std::memcpy(&wide[sz], from_beg, (std::size_t)(from_end-from_beg));
            r = std::codecvt_base::ok;
            break;

        case std::codecvt_base::ok:
            sz += to_nxt - to_beg;
            wide.resize(sz);
            break;
        }
    } while( r != std::codecvt_base::ok );

   return wide;
}
// --------------------------------------------------------------- any_string
any_string::any_string(const char * str,const std::locale& locale) : 
  m_locale(locale), m_ansi( new std::string(str) )
{}
any_string::any_string(const wchar_t * str,const std::locale& locale) : 
  m_locale(locale), m_wide( new std::wstring(str) )
{}
any_string::any_string(const std::string& str,const std::locale& locale) : 
  m_locale(locale), m_ansi( new std::string(str) )
{}
any_string::any_string(const std::wstring& str,const std::locale& locale) : 
  m_locale(locale), m_wide( new std::wstring(str) )
{}

const std::string& any_string::str() const
{
    if( !m_ansi )
    {
        assert(m_wide);

        m_ansi.reset( new std::string( narrow(*m_wide,m_locale) ) );
    }

    return *m_ansi;
}
const std::wstring& any_string::wstr() const
{
    if( !m_wide )
    {
        assert(m_ansi);

        m_wide.reset( new std::wstring( widen(*m_ansi,m_locale) ) );
    }

    return *m_wide;
}
// ------------------------------------------------------------ str_converter
str_converter::str_converter(const std::locale& locale /*= std::locale("")*/ ) : 
  m_locale(locale)
{}

}} // namespace vr::str