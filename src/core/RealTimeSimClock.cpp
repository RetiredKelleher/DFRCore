/////////////////////////////////////////////////////////////////////////////////
// @file        RealTimeSimClock.cpp
// @brief       TBD
// @author      Thomas Kelleher
////////////////////////////////////////////////////////////////////////////////

#include <DFRCore/RealTimeSimClock.hpp>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace DFR {

RealTimeSimClock::RealTimeSimClock(double initialTime,
                                    double syncRate,
                                    double wallClockLimitRate,
                                       double tickInterval)
    : DFR::SimClock(initialTime, syncRate)
    , mWallClockLimitRate(wallClockLimitRate)
    , mTickInterval(tickInterval)
    , mLastTickTime(std::chrono::steady_clock::now())
{
    if (wallClockLimitRate < 1.0)
        throw std::invalid_argument("wallClockLimitRate must be >= 1.0");
    if (tickInterval <= 0.0)
        throw std::invalid_argument("tickInterval must be > 0");
}

void RealTimeSimClock::startClock()
{
    mLastTickTime = std::chrono::steady_clock::now();
    DFR::SimClock::startClock();
    std::cout << "DFR::RealTimeSimClock started."
              << " syncRate=" << getSyncRate()
              << " wallClockLimitRate=" << mWallClockLimitRate
              << " tickInterval=" << mTickInterval << "s\n";
}

void RealTimeSimClock::stopClock()
{
    DFR::SimClock::stopClock();
    std::cout << "DFR::RealTimeSimClock stopped.\n";
}

void RealTimeSimClock::advanceTime(const double newTimeValue)
{
    DFR::SimClock::advanceTime(newTimeValue);
    // Snapshot wall clock so the next getMaxAdvanceTime() measures elapsed from here.
    mLastTickTime = std::chrono::steady_clock::now();
}

double RealTimeSimClock::tick() const
{
    // Pace the simulation: sleep for one tick interval, then compute the sim time target.
    std::this_thread::sleep_for(std::chrono::duration<double>(mTickInterval));
    return getMaxAdvanceTime();
}

double RealTimeSimClock::getMaxAdvanceTime() const
{
    // Pure query — no sleeping. Measure elapsed wall time since the last advanceTime() call.
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - mLastTickTime;
    double elapsedSec = elapsed.count();

    // syncRate scales desired sim advancement relative to wall time.
    double desiredDelta = elapsedSec * getSyncRate();

    // wallClockLimitRate caps the per-tick jump regardless of catch-up or scheduling jitter.
    double maxDelta = mTickInterval * mWallClockLimitRate;

    return getCurrentTime() + std::min(desiredDelta, maxDelta);
}

void RealTimeSimClock::setWallClockLimitRate(double rate)
{
    if (rate < 1.0)
        throw std::invalid_argument("wallClockLimitRate must be >= 1.0");
    mWallClockLimitRate = rate;
}

void RealTimeSimClock::setTickInterval(double seconds)
{
    if (seconds <= 0.0)
        throw std::invalid_argument("tickInterval must be > 0");
    mTickInterval = seconds;
}

} // namespace DFR