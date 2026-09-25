/////////////////////////////////////////////////////////////////////////////////
// @file    PriorityQueueEventManager.cpp
// @brief DFR::PriorityQueueEventManager implementation
// @author  Thomas Kelleher
/////////////////////////////////////////////////////////////////////////////////

#include "PriorityQueueEventManager.hpp"

namespace DFR {

unsigned int PriorityQueueEventManager::addEvent(std::unique_ptr<DFR::Event> event)
{
    std::lock_guard<std::recursive_mutex> lock(mEventMutex);
    // Sanity check for null event pointer
    if (event == nullptr)
    {
        return 0; // or some invalid event counter value
    }

    // Assign a unique event counter to the event and push it into the priority queue
    unsigned int eventCounter = mEventCounter++;
    event->setEventCounter(eventCounter);
    mEventQueue.push(DFR::EventManager::EventLocal(std::move(event), eventCounter));
    return eventCounter;
}

//! @brief Requeues an event to be added back to the event manager. This is used when an event's Execute function returns eReschedule, indicating that the event should be rescheduled for future execution.
//! @param[in] event The event to be requeued.
//! @note Assumes event has adjusted the next simulation time before being requeued.  Also the event Counter remains the same.
void PriorityQueueEventManager::requeueEvent(std::unique_ptr<DFR::Event> event)
{
    std::lock_guard<std::recursive_mutex> lock(mEventMutex);
    // Ensure we have a valid event before requeuing it
    if (event)
    {
        unsigned int eventCounter = event->eventCounter();
        mEventQueue.push(EventManager::EventLocal(std::move(event), eventCounter));
    }
}

//! @brief Gets the next event from the event manager if its simulation time is at or before the specified maximum simulation time.
//! @param[in] maxSimTime The maximum simulation time for the next event to be retrieved.
//! @note Combines both peekNextEvent and getNextEvent into thread-safe operation.
//! @return A unique pointer to the next event if its simulation time is at or before maxSimTime, or nullptr otherwise.
std::unique_ptr<DFR::Event> PriorityQueueEventManager::getNextEventAtOrBefore(double maxSimTime, bool& queWasEmpty)
{
    std::lock_guard<std::recursive_mutex> lock(mEventMutex);

    // Determine if the event queue was empty at the time of the call
    queWasEmpty = mEventQueue.empty();

    // Case 1: Queue was empty at the time of the call
    if (queWasEmpty)
    {
        return nullptr; // Queue was empty, return nullptr immediately
    }

    // Case 2: Queue is not empty, check the simulation time of the next event
    if (mEventQueue.top().mEventPtr->simTimeOfEvent() > maxSimTime)
    {
        return nullptr; // Next event's simulation time is not at or before maxSimTime
    }

    // Case 3: Queue is not empty and the next event's simulation time is before maxSimTime
    auto nextEvent = std::move(mEventQueue.top().mEventPtr);
    mEventQueue.pop();
    return nextEvent;
}

const DFR::Event* PriorityQueueEventManager::peekNextEvent()
{
    std::lock_guard<std::recursive_mutex> lock(mEventMutex);
    if (!mEventQueue.empty())
    {
        const auto& nextEvent = mEventQueue.top();
        // You can add code here to display or log the next event details
        return nextEvent.mEventPtr.get();
    }
    return nullptr; // No events to peek at
}

std::unique_ptr<DFR::Event> PriorityQueueEventManager::getNextEvent()
{
    std::lock_guard<std::recursive_mutex> lock(mEventMutex);
    if (mEventQueue.empty())
    {
        return nullptr; // No events to process
    }
    auto nextEvent = std::move(mEventQueue.top().mEventPtr);
    mEventQueue.pop();
    return nextEvent;
}

void PriorityQueueEventManager::clearEvents()
{
    std::lock_guard<std::recursive_mutex> lock(mEventMutex);
    while (!mEventQueue.empty())
    {
        mEventQueue.pop();
    }
}

bool PriorityQueueEventManager::removeEvent(unsigned int eventCounter)
{
    std::lock_guard<std::recursive_mutex> lock(mEventMutex);
    std::vector<std::unique_ptr<DFR::Event>> tempEvents;

    bool found = false;
    while (!mEventQueue.empty())
    {
        // Move the top event out of the queue to check its event counter
        auto topEvent = std::move(mEventQueue.top().mEventPtr);
        mEventQueue.pop();

        // Check if the event counter matches the one we want to remove
        if (topEvent->eventCounter() == eventCounter)
        {
            found = true; // Found the event to remove
            break; // Exit the loop after finding the event
        }
        else
        {
            tempEvents.push_back(std::move(topEvent)); // Temporarily store events that are not removed
        }
    }

    // Push back the events that were not removed
    for (auto& event : tempEvents)
    {
        requeueEvent(std::move(event)); // Requeue the events that were not removed
    }

    return found; // Return whether the event was found and removed
}


//////////////////////////////////////////////////////////////////////////////////////
// Private method to requeue an event for future execution
} // namespace DFR
