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

//! @brief The DFR::EventManager class is responsible for managing events in the DFRSimArch framework. It provides methods for adding, peeking, retrieving, and removing events based on their simulation time and priority. The EventManager maintains a priority queue of events and ensures thread-safe access to the event queue using a recursive mutex. This class is designed to be used by the SimEngine to manage the execution of events in the simulation loop.
class PriorityQueueEventManager : public EventManager
{
public:
    PriorityQueueEventManager() = default;
    ~PriorityQueueEventManager() = default;

    // Add event handling methods here
    //! Adds an event to the event manager with the specified priority.
    //! @param event The event to be added to the event manager.
    //! @param priority The priority of the event. Higher priority events will be executed before lower priority events.
    //! @return The unique event counter for the added event.
    unsigned int addEvent(std::unique_ptr<DFR::Event> event, const int priority);

    //! @brief Peeks at the next event in the event manager without removing it.
    //! @return A pointer to the next event in the event manager, or nullptr if there are no events.
    DFR::Event* peekNextEvent();

    //! @brief Gets the next event from the event manager and removes it from the event manager.
    //! @return A unique pointer to the next event in the event manager, or nullptr if there are no events.
    std::unique_ptr<DFR::Event> getNextEvent();

    //! Clears all events from the event manager.
    //! @brief Clears all events from the event manager.
    void clearEvents();

    //! @brief Removes a specific event from the event manager based on its unique event counter.
    //! @param eventCounter The unique event counter of the event to be removed.
    //! @return True if the event was found and removed, false otherwise.
    bool removeEvent(unsigned int eventCounter);

protected:

private:

    //! @brief Requeues an event to be added back to the event manager. This is used when an event's Execute function returns eReschedule, indicating that the event should be rescheduled for future execution.
    //! @param event The event to be requeued.
    void requeueEvent(std::unique_ptr<DFR::Event> event);

    using EventQueue = std::priority_queue<DFR::EventManager::EventLocal,
     std::vector<DFR::EventManager::EventLocal>,
     std::greater<DFR::EventManager::EventLocal>>;
    EventQueue mEventQueue;        //!< Priority queue to hold events ordered by their keys (simTime, priority, eventCounter)

};

} // Namespace DFR
