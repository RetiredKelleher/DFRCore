/////////////////////////////////////////////////////////////////////////////////
// @file        SimClock.cpp
// @brief       TBD
// @author      Thomas Kelleher
////////////////////////////////////////////////////////////////////////////////

#include <DFRCore/SimClock.hpp>
#include <limits>

namespace DFR {

// @brief Constructs a DFR::SimClock with the specified initial time.
// @param initialTime The initial time to set the simulation clock to. Default is 0.0.
// @param syncRate The initial sync rate for the clock. Default is 1.0 (real-time).
SimClock::SimClock(const double initialTime, const double syncRate)
    : mCurrentTime(initialTime),
      mSyncRate(syncRate)
{
}

// @brief Gets the current simulation time.
// @return The current simulation time.
double SimClock::getCurrentTime() const
{
    return mCurrentTime;
}

// @brief Advances the simulation time by the specified amount.
// @param newTimeValue The new simulation time value.
void SimClock::advanceTime(const double newTimeValue)
{
    mCurrentTime = newTimeValue;
}

//! @brief Batch tick: returns immediately with no upper bound on sim advancement.
//! @return The maximum simulation time allowed for this tick, which is effectively unlimited for the batch clock. 
double SimClock::tick() const
{
    return std::numeric_limits<double>::max();
}

//! @brief Pure query: no limit on advance time for the base (batch) clock.
//! @return The maximum simulation time the engine may advance to (no side effects).
double SimClock::getMaxAdvanceTime() const
{
    return std::numeric_limits<double>::max();
}

/// @brief Resets the simulation clock to the specified initial time.
/// @param initialTime The initial time to reset the simulation clock to.
void SimClock::reset(const double initialTime)
{
    mCurrentTime = initialTime;
}

} // namespace DFR