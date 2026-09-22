///////////////////////////////////////////////////////////////////////////////
// @file RandomNumber.cpp
// @brief Implementation of the RandomNumber class, providing random number generation functionality for the DFRSimArch framework.
// @author Thomas Kelleher
//////////////////////////////////////////////////////////////////////////////

#include <DFRCore/RandomNumber.hpp>

namespace DFR {

//! @brief Constructs a DFR::RandomNumber instance with an optional seed for reproducibility.
//! @param seed An optional seed for the random number generator. If not provided, a random seed will be used.
RandomNumber::RandomNumber(unsigned int seed) : mRng(seed)
{
    // Constructor implementation (if needed)
}

//! @brief Generates a random integer in the specified range [min, max].
//! @param min The minimum value of the range (inclusive).
//! @param max The maximum value of the range (inclusive).
//! @return A random integer in the specified range.
int RandomNumber::getRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mRng);
}

//! @brief Generates a random double in the specified range [min, max).
//! @param min The minimum value of the range (inclusive).
//! @param max The maximum value of the range (exclusive).
//! @return A random double in the specified range.
double RandomNumber::getRandomDouble(double min, double max)
{
    std::uniform_real_distribution<double> dist(min, max);
    return dist(mRng);
}

//! @brief Generates a random number following a normal distribution with the specified mean and standard deviation.
//! @param mean The mean of the normal distribution.
//! @param stddev The standard deviation of the normal distribution.
//! @return A random double from the specified normal distribution.
double RandomNumber::getRandomNormal(double mean, double stddev)
{
    std::normal_distribution<double> dist(mean, stddev);
    return dist(mRng);
}

} // namespace DFR
