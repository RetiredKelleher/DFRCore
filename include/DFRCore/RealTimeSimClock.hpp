///////////////////////////////////////////////////////////////////////////////
//! @file RealTimeSimClock.hpp
//! @brief DFR::RealTimeSimClock....TODO
//! @author Thomas Kelleher
//! @date 2024-06-01
//!
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <DFRCore/SimClock.hpp>
#include <chrono>

namespace DFR {

//! @brief A RealTimeSimClock that advances based on real elapsed time.
//!
//! Supports two rate controls:
//!  - syncRate (inherited): desired ratio of sim time to wall time (e.g. 2.0 = 2x real time).
//!  - wallClockLimitRate: hard cap on sim-time advancement per tick as a multiple of tickInterval.
//!    Prevents unbounded jumps after scheduling delays or catch-up situations.
//!
//! Each tick, the sim advances by:
//!   min(elapsedWall * syncRate,  tickInterval * wallClockLimitRate)
class RealTimeSimClock : public DFR::SimClock
{
public:
    //! @param initialTime        Starting simulation time.
    //! @param syncRate           Ratio of sim time to wall time (1.0 = real-time, 2.0 = 2x faster).
    //! @param wallClockLimitRate Max sim advance per tick as a multiple of tickInterval. Must be >= 1.0.
    //! @param tickInterval       Wall-clock sleep duration per tick in seconds.
    RealTimeSimClock(double initialTime        = 0.0,
                     double syncRate           = 1.0,
                     double wallClockLimitRate = 1.0,
                     double tickInterval       = 0.1);
    ~RealTimeSimClock() = default;

    /** Clock State and Management */
    //@{
    //! Records the wall clock reference point and transitions to Running state.
    void startClock() override;
    //! Transitions to Stopped state.
    void stopClock() override;
    //@}

    /** @name Simulation Time Management */
    //@{
    //! Advances sim time to newTimeValue and snapshots the wall clock for the next tick's elapsed calculation.
    void advanceTime(double newTimeValue) override;

    //! Sleeps for tickInterval, then returns the next sim time target clamped by wallClockLimitRate.
    //! This is the primary pacing point for real-time mode — the sleep lives here, not in getMaxAdvanceTime().
    double tick() const override;

    //! Pure query: returns the sim time target based on elapsed wall time since the last advanceTime().
    //! No sleeping — safe to call multiple times without side effects.
    double getMaxAdvanceTime() const override;
    //@}

    /** Rate Configuration */
    //@{
    void   setWallClockLimitRate(double rate);
    double getWallClockLimitRate() const { return mWallClockLimitRate; }

    void   setTickInterval(double seconds);
    double getTickInterval() const { return mTickInterval; }
    //@}

private:
    double mWallClockLimitRate; //!< Hard cap: max sim delta per tick = tickInterval * wallClockLimitRate
    double mTickInterval;       //!< Wall-clock sleep per tick (seconds)

    //! Snapshot of wall clock at the moment advanceTime() last completed. Updated each tick.
    std::chrono::steady_clock::time_point mLastTickTime;
};

} // namespace DFR
