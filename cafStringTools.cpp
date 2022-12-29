#include "cafStringTools.h"

//--------------------------------------------------------------------------------------------------
///
//--------------------------------------------------------------------------------------------------
std::string caffa::StringTools::trim( std::string s )
{
    s.erase( s.begin(), std::find_if( s.begin(), s.end(), []( unsigned char ch ) { return !std::isspace( ch ); } ) );
    s.erase( std::find_if( s.rbegin(), s.rend(), []( unsigned char ch ) { return !std::isspace( ch ); } ).base(), s.end() );
    return s;
}

//--------------------------------------------------------------------------------------------------
///
//--------------------------------------------------------------------------------------------------
std::string caffa::StringTools::tolower( std::string data )
{
    std::transform( data.begin(), data.end(), data.begin(), []( unsigned char c ) { return (char)std::tolower( c ); } );
    return data;
}

//--------------------------------------------------------------------------------------------------
///
//--------------------------------------------------------------------------------------------------
std::string caffa::StringTools::replace( const std::string& data, const std::string& what, const std::string& with )
{
    std::string out   = data;
    size_t      count = 0;
    for ( size_t pos = 0; out.npos != ( pos = out.find( what.data(), pos, what.length() ) ); pos += with.length(), ++count )
    {
        out.replace( pos, what.length(), with.data(), with.length() );
    }
    return out;
}
