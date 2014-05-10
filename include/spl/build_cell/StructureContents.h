/*
 * StructureContents.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_CONTENTS_H
#define STRUCTURE_CONTENTS_H

// INCLUDES ////////////
#include "spl/build_cell/Sphere.h"
#include "spl/common/Constants.h"

namespace spl {
namespace build_cell {

class StructureContents
{
public:
  StructureContents();
  StructureContents(const size_t numAtoms, const double volume);

  StructureContents & operator +=(const StructureContents & rhs);
  StructureContents & operator *=(const int multiplyBy);

  void addAtoms(const size_t numAtoms);
  void addAtoms(const size_t numAtoms, const double radius);
  void addVolume(const double volume);

  void removeAtoms(const size_t numAtoms);
  void removeAtoms(const size_t numAtoms, const double radius);
  void removeVolume(const double volume);

  size_t getNumAtoms() const;
  double getVolume() const;

private:
  double volume(const double radius) const;

  size_t myNumAtoms;
  double myVolume;
};

inline StructureContents::StructureContents():
myNumAtoms(0),
myVolume(0.0)
{}

inline StructureContents::StructureContents(const size_t numAtoms, const double volume):
myNumAtoms(numAtoms),
myVolume(volume)
{}

inline StructureContents & StructureContents::operator +=(const StructureContents & rhs)
{
  myNumAtoms += rhs.myNumAtoms;
  myVolume += rhs.myVolume;
  return *this;
}

inline StructureContents & StructureContents::operator *=(const int multiplyBy)
{
  myNumAtoms *= multiplyBy;
  myVolume *= static_cast<double>(multiplyBy);
  return *this;
}

inline void StructureContents::addAtoms(const size_t numAtoms)
{
  myNumAtoms += numAtoms;
}

inline void StructureContents::addAtoms(const size_t numAtoms, const double radius)
{
  addAtoms(numAtoms);
  addVolume(static_cast<double>(numAtoms) * Sphere::volume(radius));
}

inline void StructureContents::addVolume(const double volume)
{
  myVolume += volume;
}

inline void StructureContents::removeAtoms(const size_t numAtoms)
{
  myNumAtoms -= numAtoms;
}

inline void StructureContents::removeAtoms(const size_t numAtoms, const double radius)
{
  removeAtoms(numAtoms);
  removeVolume(static_cast<double>(numAtoms) * Sphere::volume(radius));
}

inline void StructureContents::removeVolume(const double volume)
{
  myVolume -= volume;
}

inline size_t StructureContents::getNumAtoms() const
{
  return myNumAtoms;
}

inline double StructureContents::getVolume() const
{
  return myVolume;
}

}
}

#endif /* STRUCTURE_CONTENT_H */
