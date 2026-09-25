///////////////////////////////////////////////////////////////////////////////
//! @file PriorityQueueEventManager.hpp
//! @brief Implementation of DFR::EventManager class definition, using a priority queue as the event management data structure.
//! PriorityQueueEventManager implements DFR::EventManager class using priority queues
//! to manage simulation events.  Access of the events are done in a thread-safe manner.
//! A key feature of this implementation is the quick insertation of the events.
//! Its major draw-back is the lack of random access and rescheduling of those events.
//! @note The EventManager is designed to be used by the SimEngine to manage 
//! the execution of events in the simulation loop.
//!
//! @author Thomas Kelleher
//! @date 2024-06-01
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <DFRCore/EventManager.hpp>
#include <queue>

namespace DFR {

//! @brief Implements the PriorityQueue-based event manager. The implementation
//! uses a priority queue to manage simulation events efficiently.
//! @note The EventManager is designed to be used by the SimEngine to manage 
//! the execution of events in the simulation loop.
class PriorityQueueEventManager : public EventManager
{
public:
    PriorityQueueEventManager() = default;
    ~PriorityQueueEventManager() override = default;

    //! @brief Adds an event to the event manager.
    //! @param[in] event The event to be added to the event manager.
    //! @return The unique event counter for the added event. Used to track the event.  
    virtual unsigned int addEvent(std::unique_ptr<DFR::Event> event) override;

    //! @brief Requeues an event to be added back to the event manager. This is used when an event's Execute function returns eReschedule, indicating that the event should be rescheduled for future execution.
    //! @param[in] event The event to be requeued.
    //! @note Assumes event has adjusted the next simulation time before being requeued.  Also the event counter remains the same.
    void requeueEvent(std::unique_ptr<DFR::Event> event) override;

    //! @brief Gets the next event from the event manager if its simulation time is at or before the specified maximum simulation time.
    //! @param[in] maxSimTime The maximum simulation time for the next event to be retrieved.
    //! @param[out] queWasEmpty Indicates whether the event queue was empty at the time of the call
    //! @note Combines both peekNextEvent and getNextEvent into thread-safe operation.
    //! @return A unique pointer to the next event if its simulation time is at or before maxSimTime, or nullptr otherwise.
    std::unique_ptr<DFR::Event> getNextEventAtOrBefore(double maxSimTime, bool& queWasEmpty) override;

    //! @brief Peeks at the next event in the event manager without removing it.
    //! @return A pointer to the next event in the event manager, or nullptr if there are no events.
    const DFR::Event* peekNextEvent() override;

    //! @brief Gets the next event from the event manager and removes it from the event manager.
    //! @return A unique pointer to the next event in the event manager, or nullptr if there are no events.
    std::unique_ptr<DFR::Event> getNextEvent() override;

    //! Clears all events from the event manager.
    //! @brief Clears all events from the event manager.
    void clearEvents() override;

    //! @brief Removes a specific event from the event manager based on its unique event counter.
    //! @param eventCounter The unique event counter of the event to be removed.
    //! @return True if the event was found and removed, false otherwise.
    bool removeEvent(unsigned int eventCounter) override;

protected:

private:

    using EventQueue = std::priority_queue<DFR::EventManager::EventLocal,
     std::vector<DFR::EventManager::EventLocal>,
     std::greater<DFR::EventManager::EventLocal>>;
    EventQueue mEventQueue;        //!< Priority queue to hold events ordered by their keys (simTime, priority, eventCounter)

};

} // Namespace DFR
