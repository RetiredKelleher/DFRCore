///////////////////////////////////////////////////////////////////////////////
//! @file EventManager.hpp
//! @brief Interface class definition, responsible for managing events in
//! the DFRCore framework.
//! @author Thomas Kelleher
//! @date 2024-06-01
//!
//! The DFR::EventManager class provides interface functionality for scheduling,
//! peeking, and retrieving events based on their simulation time and priority.
//! @note The EventManager is designed to be used by the SimEngine to manage 
//! the execution of events in the simulation loop.
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <DFRCore/Event.hpp>
#include <mutex>
#include <memory>
#include <tuple>

namespace DFR {

//! @brief The DFR::EventManager interface class is responsible for managing
//! events in the DFRCore framework. It provides methods for adding, peeking,
//! retrieving, and removing events based on their simulation time and priority.
//! The interface class contains a mutex, that can be used by subclasses
//! to ensure thread-safe access to the event queue.
class EventManager
{
public:
    EventManager();
    virtual ~EventManager() = default;

    //! @brief Adds an event to the event manager.
    //! @param[in] event The event to be added to the event manager.
    //! @return The unique event counter for the added event. Used to track the event.  
    virtual unsigned int addEvent(std::unique_ptr<DFR::Event> event) = 0;

    //! @brief Requeues an event to be added back to the event manager. This is used when an event's Execute function returns eReschedule, indicating that the event should be rescheduled for future execution.
    //! @param[in] event The event to be requeued.
    //! @note Assumes event has adjusted the next simulation time before being requeued.  Also the event counter remains the same.
    virtual void requeueEvent(std::unique_ptr<DFR::Event> event) = 0;

    //! @brief Gets the next event from the event manager if its simulation time is at or before the specified maximum simulation time.
    //! @param[in] maxSimTime The maximum simulation time for the next event to be retrieved.
    //! @param[out] queWasEmpty Indicates whether the event queue was empty at the time of the call
    //! @note Combines both peekNextEvent and getNextEvent into thread-safe operation.
    //! @return A unique pointer to the next event if its simulation time is at or before maxSimTime, or nullptr otherwise.
    virtual std::unique_ptr<DFR::Event> getNextEventAtOrBefore(double maxSimTime, bool& queWasEmpty) = 0;

    //! @brief Peeks at the next event in the event manager without removing it.
    //! @return A pointer to the next event in the event manager, or nullptr if there are no events.
    virtual const DFR::Event* peekNextEvent() = 0;

    //! @brief Gets the next event from the event manager and removes it from the event manager.
    //! @return A unique pointer to the next event in the event manager, or nullptr if there are no events.
    virtual std::unique_ptr<DFR::Event> getNextEvent() = 0;

    //! Clears all events from the event manager.
    //! @brief Clears all events from the event manager.
    virtual void clearEvents() = 0;

    //! @brief Removes a specific event from the event manager based on its unique event counter.
    //! @param[in] eventCounter The unique event counter of the event to be removed.
    //! @return True if the event was found and removed, false otherwise.
    virtual bool removeEvent(unsigned int eventCounter) = 0;

    /** @name Supported Event Manager Implementations */
    //@{
    //! @brief Implementation of a Priority-based Event Manager.
    static std::unique_ptr<EventManager> createPriorityBasedEventManager();
    //@}
    
protected:

    //! Mutex to protect access to the event queue
    mutable std::recursive_mutex mEventMutex{};

    //! Internal structure to hold events in the priority queue with their keys for ordering    
    struct EventLocal
    {
        //! Key for ordering events in the priority queue: (simTime, priority, eventCounter)
        //! This ensures that events are ordered first by simulation time, then by priority, and finally by the order they were added (event counter) to maintain a consistent order for events with the same simTime and priority.
        using Key = std::tuple<double, int, unsigned int>;

        //! Constructor to create an EventLocal from an Event and its key components
        EventLocal(std::unique_ptr<DFR::Event> event, unsigned int eventCounter)
             : mKey(std::make_tuple(event->simTimeOfEvent(), event->priority(), eventCounter)),
               mEventPtr(std::move(event))
            {}

         //! Comparison operator for ordering events in the priority queue based on their keys
        bool operator>(const EventLocal& other) const
        {
            return (mKey > other.mKey); // Compare by simTime, then priority, then event counter
        }

        Key mKey;  //!<Key for ordering events in the priority queue
        mutable std::unique_ptr<DFR::Event> mEventPtr; //!< Pointer to the event
    };

    unsigned int mEventCounter{1}; //!< To ensure unique keys for events with same simTime and priority 

private:

};

} // Namespace DFR
