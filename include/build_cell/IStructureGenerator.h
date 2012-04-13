/*
 * IStructureGenerator.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_STRUCTURE_GENERATOR_H
#define I_STRUCTURE_GENERATOR_H

// FORWARD DECLARES //////////////////////////
namespace sstbx { namespace common {
	class Structure;
}}

namespace sstbx { namespace build_cell {

class IStructureGenerator
{
public:

	/**
	 * Generate a new structure.
	 *
	 */
	virtual ::sstbx::common::Structure * const generateStructure() const = 0;
};

}}

#endif /* I_STRUCTURE_GENERATOR_H */
