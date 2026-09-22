///////////////////////////////////////////////////////////////////////////////
//! @file SimEngine.hpp
//! @brief DFR::SimEngine class definition, responsible for managing the simulation loop and event execution.
//! @details The DFR::SimEngine class provides the main simulation loop,
//! handling event execution, communication polling, and time management.
//! It utilizes an EventManager to manage events and a SimClock to control the
//! simulation time.
//! @author Thomas Kelleher
//! @date 2024-06-01
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
    //! @brief Constructs a DFR::SimEngine instance.
    //! @param[in] eventManager A unique pointer to an EventManager implementation that will be used to manage events in the simulation engine. The EventManager is responsible for scheduling, peeking, and retrieving events based on their simulation time and priority.
    //! @param[in] inBatchMode  true  → SimClock (unlimited advancement, no pacing).
    //!                     false → RealTimeSimClock (wall-clock paced via tick()).
    SimEngine(std::unique_ptr<DFR::EventManager> eventManager,
        const bool inBatchMode = true);
    ~SimEngine();

    //! @brief Runs the main loop of the simulation.
    //! Each iteration: tick the clock, drain all due events, poll communications.
    void run();

    //! @brief Signals the engine to stop after the current iteration completes.
    void stopExecution() { mStopExecution = true; }

    /** @name Events */
    //@{
    //! @brief Adds an event to the simulation engine's event manager with the specified priority.
    //! @param[in] event The event to be added to the simulation engine's event manager.
    //! @param[in] priority The priority of the event. Higher priority events will be executed before lower priority events.
    void addEvent(std::unique_ptr<DFR::Event> event, int priority);
    //@}

    /** @name Communication Polling
     *  Register one or more IDFRCommPoller implementations to be called every
     *  iteration inside updateSimulationState(). Pollers are called in
     *  registration order after all due events for that tick have executed.
     */
    //@{
    //! @brief Registers a communication poller to be called every simulation loop iteration.
    //! @param[in] poller A unique pointer to an ICommPoller implementation to be registered with the simulation engine. The poller will be called once per iteration after events have been processed.
    void registerPoller(std::unique_ptr<ICommPoller> poller);
    //@}

protected:

private:
    //! @brief Drains and executes all events whose simTime <= maxSimTime.
    //! @param[in] maxSimTime The maximum simulation time up to which events should be processed. Events with a simTime greater than this value will not be executed in this iteration.
    void processEventsUpTo(double maxSimTime);

    //! Calls poll() on every registered IDFRCommPoller.
    //! Called once per iteration regardless of whether any events fired.
    void updateSimulationState();

    bool mStopExecution{false};  //!< Flag to indicate whether the simulation engine should stop execution after the current iteration. When set to true, the run() method will exit its loop and terminate the simulation.
    double lastEventTime{0.0};   //!< Tracks the simulation time of the last executed event. This is used to update the simulation clock and manage the timing of events in the simulation loop. It helps ensure that the simulation time progresses correctly and that events are executed in the correct order
    bool mInBatchMode{true};     //!< Flag indicating whether the simulation engine is running in batch mode (true) or real-time mode (false). In batch mode, the simulation clock can advance without pacing, while in real-time mode, the clock is paced according to wall-clock time.
    std::unique_ptr<DFR::EventManager> mSimEventManager;
    DFR::SimClock*     mSimClock;
    std::vector<std::unique_ptr<ICommPoller>> mCommPollers;
};

} // namespace DFR
