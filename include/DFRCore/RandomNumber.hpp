//////////////////////////////////////////////////////////////////////////////////////
//! @file RandomNumber.hpp
//! @brief DFR::RandomNumber class definition, providing random number generation functionality for the DFRSimArch framework.
//! @author Thomas Kelleher
//////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <random>

namespace DFR {

//! @brief DFRRandomNumber is a utility class that provides random number generation functionality for the DFRSimArch framework. It encapsulates a random number generator and provides methods for generating random numbers of various types and distributions.
class RandomNumber
{
public:
    //! @brief Constructs a DFRRandomNumber instance with an optional seed for reproducibility.
    //! @param seed An optional seed for the random number generator. If not provided, a random seed will be used.
    RandomNumber(unsigned int seed = std::random_device{}());

    //! @brief Sets the seed for the random number generator to allow for reproducible random sequences.
    //! @param seed The seed value to set for the random number generator.
    void setSeed(unsigned int seed) { mRng.seed(seed); }
    
    //! @brief Generates a random integer in the specified range [min, max].
    //! @param min The minimum value of the range (inclusive).
    //! @param max The maximum value of the range (inclusive).
    //! @return A random integer in the specified range.
    int getRandomInt(int min, int max);

    //! @brief Generates a random double in the specified range [min, max).
    //! @param min The minimum value of the range (inclusive).
    //! @param max The maximum value of the range (exclusive).
    //! @return A random double in the specified range.
    double getRandomDouble(double min, double max);

    //! @brief Generates a random number following a normal distribution with the specified mean and standard deviation.
    //! @param mean The mean of the normal distribution.
    //! @param stddev The standard deviation of the normal distribution.
    //! @return A random number following the specified normal distribution.
    double getRandomNormal(double mean, double stddev);

private:
    std::mt19937 mRng; //!< Mersenne Twister random number generator
};

} // Namespace DFR
