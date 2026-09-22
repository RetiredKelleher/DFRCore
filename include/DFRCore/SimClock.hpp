///////////////////////////////////////////////////////////////////////////////
//! @file SimClock.hpp
//! @brief SimClock class definition, providing a base class for simulation clocks in the DFRCore framework.
//! @details The DFR::SimClock class provides a common interface for both the basic
//! BATCH mode clock and the more complex Real-Time clock.
//! @author Thomas Kelleher
//! @date 2024-06-01
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace DFR {

//! @brief Base class for simulation clocks in the DFRCore framework.
//! The DFR::SimClock class provides a common interface for both the basic BATCH mode
//! clock and the more complex Real-Time clock.
class SimClock
{
public:
    //! @param[in] initialTime The initial time to set the simulation clock to. Default is 0.0.
    //! @param[in] syncRate The initial sync rate for the clock. Default is 1.0 (real-time).
    SimClock(const double initialTime = 0.0,
             const double syncRate = 1.0);
    virtual ~SimClock() = default;

    enum ClockState
    {
        eStartup, //!< The clock is in the startup state, initializing resources and preparing to run.
        eRunning, //!< The clock is running, advancing time and allowing events to be processed.
        eCatchUp, //!< The clock is in catch-up mode (Real-Time mode only), trying to advance time to catch up with the current simulation time after being paused or delayed.
        eStopped  //!< The clock is stopped, not advancing time and not allowing events to be processed.
    };

    /** Clock State and Management  */
    //@{
    //! @brief Gets the current state of the simulation clock.
    //! @return The current state of the simulation clock.
    ClockState getClockState() const { return mClockState; }
    //! @brief Put the Clock in into RUN State. This allows the clock to advance time and process events.
    virtual void startClock() { mClockState = ClockState::eRunning; }
    //! @brief Put the Clock in into STOPPED State. This prevents the clock from advancing time and
    virtual void stopClock() { mClockState = ClockState::eStopped; }

    //! @brief Set SyncRate for the clock. 
    //! This is the rate at which the clock will advance time when in RUNNING state.
    //! @note The sync rate is only used when using the Real-Time mode of the clock. In this mode, the clock will advance time at the specified sync rate, which can be used to speed up or slow down the simulation time relative to real time.
    //! @param[in] syncRate The new sync rate for the clock.
    virtual void setSyncRate(const double syncRate) { mSyncRate = syncRate; }
    //! @brief Gets the current sync rate of the simulation clock.
    //! @return The current sync rate of the simulation clock.
    double getSyncRate() const { return mSyncRate; }

    //@}

    /** @name Simulation Time Management */
    //@{
    //! @brief Gets the current simulation time.
    //! @return The current simulation time.
    double getCurrentTime() const;
    //! @brief Sets the current simulation time to the specified value.
    //! @param[in] newTimeValue The new simulation time value to set.
    void setCurrentTime(const double newTimeValue) { mCurrentTime = newTimeValue; }
    
    //! @brief Advances the simulation time by the specified amount.
    //! @param[in] newTimeValue The new simulation time value.
    virtual void advanceTime(const double newTimeValue);

    //! @brief Wait for the next simulation tick and return the maximum sim time allowed this iteration.
    //!
    //! Batch clock: returns immediately with std::numeric_limits<double>::max() (no pacing).
    //! Real-time clock: sleeps for one tickInterval, then returns the wall-time-bounded sim target.
    //!
    //! Separating the wait from getMaxAdvanceTime() keeps the query side-effect-free.
    //! @return The maximum simulation time the engine may advance to in this iteration.
    virtual double tick() const;

    //! @brief Pure query: returns the maximum sim time reachable this tick without any waiting.
    //! @return The maximum simulation time the engine may advance to (no side effects).
    virtual double getMaxAdvanceTime() const;

    //! @brief Resets the simulation clock to the specified initial time.
    //! @param initialTime The initial time to reset the simulation clock to.
    void reset(const double initialTime=0.0);
    //@}

private:
    //! The current simulation time.
    double mCurrentTime{0.0};
    //! The rate at which the clock will advance time when in RUNNING state.
    double mSyncRate{1.0};

    //! The current state of the simulation clock.
    ClockState mClockState{ClockState::eStartup};
};

} // namespace DFR