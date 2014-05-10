/*
 * AtomFormatParser.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_FORMAT_PARSER_H
#define ATOM_FORMAT_PARSER_H

// INCLUDES /////////////////////////////////////////////
#include <map>
#include <string>
#include <vector>

#include <boost/any.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace spl {

// FORWARD DECLARATIONS ////////////////////////////////////

namespace io {

template< typename AtomDataType>
  class AtomFormatParser
  {
  public:
    typedef std::vector< std::string> CompactAtomInfo;
    typedef boost::variant< CompactAtomInfo, AtomDataType> AtomInfo;
    typedef std::vector< std::string> FormatDescription;

    AtomFormatParser();
    explicit
    AtomFormatParser(const FormatDescription & formatDescription);

    AtomFormatParser &
    operator =(const AtomFormatParser & toCopy);

    bool
    setFormat(const FormatDescription & format);

    template< typename T>
      void
      addEntry(const std::string & name, T AtomDataType::* const memberPtr);

    template< typename T>
      bool
      setDefault(T AtomDataType::* const memberPtr,
          const boost::optional< T> & value);

    template< typename T>
      bool
      setDefault(boost::optional< T> AtomDataType::* const memberPtr,
          const boost::optional< T> & value);

    template< typename T>
      boost::optional< T>
      getValue(T AtomDataType::* const memberPtr,
          const AtomInfo & atomData) const;

    template< typename T>
      boost::optional< T>
      getValue(boost::optional< T> AtomDataType::* const memberPtr,
          const AtomInfo & atomData) const;

  private:
    typedef int AtomDataType::* CanonicalMemberPtrType;
    typedef std::vector< CanonicalMemberPtrType> FormatOrder;
    typedef std::map< std::string, CanonicalMemberPtrType> FormatEntries;
    typedef std::vector< CanonicalMemberPtrType> FormatEntriesList;
    typedef std::vector< boost::any> DefaultsList;

    template< typename T>
      CanonicalMemberPtrType
      canonicalise(T AtomDataType::* const memberPtr) const;

    template< typename T>
      size_t
      entryIdx(T AtomDataType::* memberPtr) const;

    template< typename T>
      size_t
      formatIdx(T AtomDataType::* memberPtr) const;

    template< typename T /*= AtomDataType*/>
      void
      init();

    DefaultsList myDefaults;
    FormatOrder myFormatOrder;
    FormatEntries myFormatEntries;
    FormatEntriesList myFormatEntriesList;
  };

}
}

#include "spl/io/detail/AtomFormatParser.h"

#endif /* ATOM_FORMAT_PARSER_H */
