/*
 * NamedProperty.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////
#include "utility/NamedProperty.h"

namespace sstbx {
namespace utility {


BadLexicalCast::BadLexicalCast(const char * msg):
myMsg(msg)
{}

const char * BadLexicalCast::what() const throw()
{
  return myMsg.c_str();
}


}
}
