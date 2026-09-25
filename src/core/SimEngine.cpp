/////////////////////////////////////////////////////////////////////////////////
// @file        SimEngine.cpp
// @brief       TBD
// @author      Thomas Kelleher
////////////////////////////////////////////////////////////////////////////////

#include <DFRCore/SimEngine.hpp>
#include <DFRCore/RealTimeSimClock.hpp>

#include <iostream>

namespace DFR {

SimEngine::SimEngine(std::unique_ptr<DFR::EventManager> eventManager, const bool inBatchMode) : 
    mInBatchMode(inBatchMode), mSimEventManager(std::move(eventManager)),
     mSimClock(nullptr)
{
    /// Create the Event Manager
//    mSimEventManager = new PriorityQueueEventManager();

    /// Determine type of simClock - true DES or Wall-Clock
    if (mInBatchMode)
    {
        std::cout << "Initializing DFR::SimEngine in Batch Mode (no rendering)." << std::endl;
        mSimClock = new DFR::SimClock();
    }
    else
    {
        std::cout << "Initializing DFR::SimEngine in Real-Time Mode (with rendering)." << std::endl;
        mSimClock = new DFR::RealTimeSimClock();
    }
}

SimEngine::~SimEngine()
{
    delete mSimClock;
}

void SimEngine::run()
{
    // Sanity Check on creation
    if (mSimClock == nullptr || mSimEventManager == nullptr)
    {
        std::cerr << "DFR::SimEngine: Exiting due to SimClock or EventManager NULLPTR" << std::endl;
        mStopExecution = true;
        return;
    }

    mSimClock->startClock();

    while (!mStopExecution)
    {
        // tick() paces the loop: batch returns DBL_MAX immediately;
        // real-time sleeps tickInterval then returns the wall-bounded sim target.
        double maxSimTime = mSimClock->tick();

        std::cout << "Current Simulation Time: " << mSimClock->getCurrentTime() << std::endl;

        processEventsUpTo(maxSimTime);
        updateSimulationState();
    }
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SimEngine::processEventsUpTo(double maxSimTime)
{
    // Initialize with the current simulation time
    double lastEventTime = mSimClock->getCurrentTime();

    // Keep processing until a break-condition exists
    while (true)
    {
        // Get the next event to be processed
        bool queWasEmpty = false;
        std::unique_ptr<DFR::Event> nextEvent = mSimEventManager->getNextEventAtOrBefore(maxSimTime, queWasEmpty);

        ////////////////////////////////////////
        // Case 1: event queue is empty, if so then quit
        ////////////////////////////////////////
        if (queWasEmpty)
        {
            stopExecution(); // Queue exhausted — signal the loop to stop.
            break;
        }

        ////////////////////////////////////////
        // Case 2: Next event lies beyond what the clock allows this tick.
        // NOTE: This would occur when using the Real-Time clock
        ////////////////////////////////////////
        if (!queWasEmpty && nextEvent == nullptr)
        {
            mSimClock->setCurrentTime(maxSimTime);  // Advance clock to max allowed time.
            break;
        }

        ////////////////////////////////////////
        // Case 3: Process the NEXT event
        ////////////////////////////////////////
        std::cout << "V2: Processing Event with priority: " << nextEvent->priority()
                << ", eventCounter: " << nextEvent->eventCounter()
                << ", EventID: " << nextEvent->getEventID() << std::endl;

        // Update the clock for any events in the list
        lastEventTime = nextEvent->simTimeOfEvent();
        mSimClock->setCurrentTime(lastEventTime);
        DFR::Event::EventStatus status = nextEvent->Execute();

        if (status == DFR::Event::EventStatus::eReschedule)
        {
            mSimEventManager->requeueEvent(std::move(nextEvent));
        }

        ////////////////////////////////////////
        // Case 4: Event has told simulation to STOP
        ////////////////////////////////////////
        if (mStopExecution) break;
    }
}

//! Calls poll() on every registered ICommPoller.
//! Invoked once per iteration regardless of whether any events fired.
void SimEngine::updateSimulationState()
{
    double t = mSimClock->getCurrentTime();
    for (auto& poller : mCommPollers)
        poller->poll(t);
}

//! @brief Adds an event to the simulation engine with the specified priority.
//! @param[in] event The event to be added to the simulation engine.
void SimEngine::addEvent(std::unique_ptr<DFR::Event> event)
{
    mSimEventManager->addEvent(std::move(event));
}

void SimEngine::registerPoller(std::unique_ptr<ICommPoller> poller)
{
    mCommPollers.push_back(std::move(poller));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Protected Items
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


} // Namespace DFR
