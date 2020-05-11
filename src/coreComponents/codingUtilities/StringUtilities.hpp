/*
 * ------------------------------------------------------------------------------------------------------------
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * Copyright (c) 2018-2019 Lawrence Livermore National Security LLC
 * Copyright (c) 2018-2019 The Board of Trustees of the Leland Stanford Junior University
 * Copyright (c) 2018-2019 Total, S.A
 * Copyright (c) 2019-     GEOSX Contributors
 * All right reserved
 *
 * See top level LICENSE, COPYRIGHT, CONTRIBUTORS, NOTICE, and ACKNOWLEDGEMENTS files for details.
 * ------------------------------------------------------------------------------------------------------------
 */

/**
 * @file StringUtilities.hpp
 */

#ifndef GEOSX_CODINGUTILITIES_STRINGUTILITIES_HPP_
#define GEOSX_CODINGUTILITIES_STRINGUTILITIES_HPP_

#include <cxxabi.h>
#include <cstring>
#include <memory>
#include <sstream>
 #include <iomanip>
#include <algorithm>
#include <map>


#include "common/DataTypes.hpp"
#include "LvArray/src/IntegerConversion.hpp"

namespace geosx
{
namespace stringutilities
{

/// Overloaded function to check equality between strings and char arrays
/// Mainly used to avoid char*==char* mistakes
inline bool streq( std::string const & strA, std::string const & strB )
{ return strA == strB; }

inline bool streq( std::string const & strA, char const * const strB )
{ return strA == strB; }

inline bool streq( char const * const strA, std::string const & strB )
{ return strA == strB; }

inline bool streq( char const * const strA, char const * const strB )
{ return !strcmp( strA, strB ); }

/// Subdivide string by delimiters
string_array Tokenize( std::string const & str, std::string const & delimiters );

/// Trim whitespace from string
void TrimLeft(std::string& str, const std::string& d=" \t\n\r");
void TrimRight(std::string& str, const std::string& d=" \t\n\r");
void Trim(std::string& str, const std::string& d=" \t\n\r");

inline void Trim(string_array& strVect, const std::string& d=" \t\n\r"){
  for(int i =0 ; i < int(strVect.size()) ; ++i)
    Trim(strVect[i],d);
}

/// Split string at first token
string_array Split(const std::string& str, const std::string& delimiters);

/// Remove all spaces ' ' from a string
inline void RemoveSpaces(std::string& aString){
  aString.erase(std::remove(aString.begin(), aString.end(), ' '), aString.end());
}

/**
 * @brief Retuns a string containing a padded value
 * @param[in] value to be padded
 * @param[in] size size of the padding
 */
template< typename T >
string PadValue( T value, int size )
{
  std::stringstream paddedStringStream;
  paddedStringStream << std::setfill( '0' ) << std::setw( size ) << value;
  return paddedStringStream.str();
}
}
}

#endif /* GEOSX_CODINGUTILITIES_STRINGUTILITIES_HPP_ */
