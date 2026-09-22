/////////////////////////////////////////////////////////////////////////////////
// @file    PriorityQueueEventManager.cpp
// @brief DRF::EventManager ... TODO
// @author  Thomas Kelleher
/////////////////////////////////////////////////////////////////////////////////

#include "PriorityQueueEventManager.hpp"

namespace DFR {

unsigned int PriorityQueueEventManager::addEvent(std::unique_ptr<DFR::Event> event, const int priority)
{
    std::lock_guard<std::recursive_mutex> lock(mEventMutex);
    unsigned int eventCounter = mEventCounter++;
    event->setEventCounter(eventCounter);
    mEventQueue.push(DFR::EventManager::EventLocal(std::move(event), eventCounter));
    return eventCounter;
}

DFR::Event* PriorityQueueEventManager::peekNextEvent()
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
    std::vector<DFR::Event> tempEvents;

    bool found = false;
#if 1
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
            tempEvents.push_back(std::move(*topEvent)); // Temporarily store events that are not removed
//            tempEvents.push_back(std::move(topEvent)); // Temporarily store events that are not removed
        }
    }

    // Push back the events that were not removed
    for (auto& event : tempEvents)
    {
        requeueEvent(std::make_unique<DFR::Event>(std::move(event))); // Requeue the events that were not removed
    }
#endif

    return found; // Return whether the event was found and removed
}


//////////////////////////////////////////////////////////////////////////////////////
// Private method to requeue an event for future execution
void PriorityQueueEventManager::requeueEvent(std::unique_ptr<DFR::Event> event)
{
    std::lock_guard<std::recursive_mutex> lock(mEventMutex);
    mEventQueue.push(EventManager::EventLocal(std::move(event), event->eventCounter()));
}

} // namespace DFR
