///////////////////////////////////////////////////////////////////////////////
//! @file SimEngine.hpp
//! @brief DFR::SimEngine....TODO
//! @author Thomas Kelleher
//! @date 2024-06-01
//!
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <DFRCore/EventManager.hpp>
#include <DFRCore/SimClock.hpp>
#include <DFRCore/Event.hpp>
#include <DFRCore/ICommPoller.hpp>
#include <vector>

namespace DFR {

class SimEngine
{
public:
    //! @param inBatchMode  true  → SimClock (unlimited advancement, no pacing).
    //!                     false → RealTimeSimClock (wall-clock paced via tick()).
    SimEngine(const bool inBatchMode = true);
    ~SimEngine();

    //! @brief Runs the main loop of the simulation.
    //! Each iteration: tick the clock, drain all due events, poll communications.
    void run();

    //! @brief Signals the engine to stop after the current iteration completes.
    void stopExecution() { mStopExecution = true; }

    /** @name Events */
    //@{
    //! @brief Adds an event to the simulation engine's event manager with the specified priority.
    //! @param event The event to be added to the simulation engine's event manager.
    //! @param priority The priority of the event. Higher priority events will be executed before lower priority events.
    void addEvent(std::unique_ptr<DFR::Event> event, int priority);
    //@}

    /** @name Communication Polling
     *  Register one or more IDFRCommPoller implementations to be called every
     *  iteration inside updateSimulationState(). Pollers are called in
     *  registration order after all due events for that tick have executed.
     */
    //@{
    void registerPoller(std::unique_ptr<ICommPoller> poller);
    //@}

protected:

private:
    //! Drains and executes all events whose simTime <= maxSimTime.
    void processEventsUpTo(double maxSimTime);

    //! Calls poll() on every registered IDFRCommPoller.
    //! Called once per iteration regardless of whether any events fired.
    void updateSimulationState();

    bool mStopExecution{false};
    bool mInBatchMode{true};
    DFR::EventManager* mSimEventManager;
    DFR::SimClock*     mSimClock;
    std::vector<std::unique_ptr<ICommPoller>> mCommPollers;
};

} // namespace DFR
