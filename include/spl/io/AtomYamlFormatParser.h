/*
 * AtomYamlFormatParser.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_YAML_FORMAT_PARSER_H
#define ATOM_YAML_FORMAT_PARSER_H

// INCLUDES /////////////////////////////////////////////
#include <map>
#include <string>

#include <yaml-cpp/yaml.h>

namespace spl {

// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
class Atom;
}

namespace io {

class AtomYamlFormatParser
{
public:

  typedef ::std::pair< ::std::string, YAML::Node> FormatEntry;
  typedef ::std::vector<FormatEntry> AtomsFormat;
  typedef ::std::map< ::std::string, YAML::Node> AtomInfo;

  AtomYamlFormatParser();
  explicit AtomYamlFormatParser(const AtomsFormat & atomsFormat);
  explicit AtomYamlFormatParser(const YAML::Node & formatNode);
  
  const AtomsFormat & getFormat() const;
  void setFormat(const AtomsFormat & format);
  bool updateFormat(const YAML::Node & formatNode);
  
  bool parse(AtomInfo & atomInfo, const YAML::Node & atomNode) const;

  YAML::Node generateNode(const AtomInfo & atomInfo) const;

private:

  AtomsFormat myAtomsFormat;

};

}
}

#endif /* ATOM_YAML_FORMAT_PARSER_H */
