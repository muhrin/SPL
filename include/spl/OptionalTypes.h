/*
 * OptionalTypes.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef OPTIONAL_TYPES_H
#define OPTIONAL_TYPES_H

// INCLUDES /////////////////////////////////////////////
#include <string>

#include <boost/optional.hpp>

#include <armadillo>


// FORWARD DECLARATIONS ////////////////////////////////////


namespace spl {

typedef ::boost::optional<bool> OptionalBool;
typedef ::boost::optional<double> OptionalDouble;
typedef ::boost::optional< ::std::string> OptionalString;
typedef ::boost::optional<unsigned int> OptionalUInt;
typedef ::boost::optional< ::arma::mat33> OptionalArmaMat33;
typedef ::boost::optional< ::arma::mat> OptionalArmaMat;
typedef ::boost::optional< ::arma::vec3> OptionalArmaVec3;
typedef ::boost::optional< ::arma::vec4> OptionalArmaVec4;
typedef ::boost::optional< ::arma::vec> OptionalArmaVec;

template <typename T>
struct OptionalVector
{
  typedef ::boost::optional< ::std::vector<T> > Type;
private:
  OptionalVector() {}
};

}

#endif /* OPTIONAL_TYPES_H */
