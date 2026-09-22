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

    //! @brief Adds an event to the event manager with the specified priority.
    //! @param[in] event The event to be added to the event manager.
    //! @param[in] priority The priority of the event. Higher priority events will be executed before lower priority events.
    //! @return The unique event counter for the added event.
    virtual unsigned int addEvent(std::unique_ptr<DFR::Event> event, const int priority) = 0;

    //! @brief Peeks at the next event in the event manager without removing it.
    //! @return A pointer to the next event in the event manager, or nullptr if there are no events.
    virtual DFR::Event* peekNextEvent() = 0;

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
             : mKey(std::make_tuple(event->simTime(), event->priority(), eventCounter)),
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
