/*
 * SortedDistanceComparator.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////

#include "SortedDistanceComparator.h"

#include "common/AbstractFmidCell.h"
#include "common/Structure.h"

#include <armadillo>

namespace sstbx { namespace utility {

const size_t SortedDistanceComparator::MAX_CELL_MULTIPLES = 10;
const double SortedDistanceComparator::DEFAULT_TOLERANCE = 1e-3;

SortedDistanceComparator::SortedDistanceComparator(const double tolerance):
myTolerance(tolerance)
{}

double SortedDistanceComparator::compareStructures(
	const SortedDistanceComparisonData & dist1,
	const SortedDistanceComparisonData & dist2) const
{
	using std::vector;

	const size_t maxI = std::min(
		dist1.sortedDistances.size(),
		dist2.sortedDistances.size());
	double rms = 0.0;
	for(size_t i = 0; i < maxI; ++i)
	{
		rms += pow((dist1.sortedDistances[i] - dist2.sortedDistances[i]), 2);
	}
	rms = sqrt(rms);

	return rms;
}

bool SortedDistanceComparator::areSimilar(
	const SortedDistanceComparisonData & dist1,
	const SortedDistanceComparisonData & dist2) const
{
	return compareStructures(dist1, dist2) < myTolerance;
}

const SortedDistanceComparisonData *
SortedDistanceComparator::generateComparisonData(const sstbx::common::Structure & str) const
{
	using std::vector;

	typedef arma::vec3	Vec3;
	typedef arma::mat33 Mat33;

	// Copy over the unit cell so we can do distance calculations
	SortedDistanceComparisonData * data = new SortedDistanceComparisonData(*(str.getUnitCell()));

	const double maxDist = 1.75 * data->cell.getLongestVector();

	// Calculate the distances ...
	vector<double> * const distances = new vector<double>();
	populateDistanceVector(str, data->sortedDistances, maxDist);
	// ... and sort them
	std::sort(data->sortedDistances.begin(), data->sortedDistances.end());

	return data;
}

void SortedDistanceComparator::populateDistanceVector(
	const ::sstbx::common::Structure & str,
	std::vector<double> & distVec,
	const double cutoff) const
{
	typedef arma::Mat<double> Mat;

	Mat pos;
	str.getAtomPositionsDescendent(pos);
	const ::sstbx::common::AbstractFmidCell<> & cell = *str.getUnitCell();

	std::vector<double> distances;
	const size_t numParticles = pos.n_cols;
	for(size_t i = 0; i < numParticles - 1; ++i)
	{
		for(size_t j = i + 1; j < numParticles; ++j)
		{
			distances.clear();

			cell.getAllDistancesWithinCutoff(pos.col(i), pos.col(j), cutoff, distances, MAX_CELL_MULTIPLES);

			distVec.insert(distVec.end(), distances.begin(), distances.end());
		}
	}
}

}}