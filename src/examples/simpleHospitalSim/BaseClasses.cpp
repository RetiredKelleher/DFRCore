////////////////////////////////////////////////////////////////////////////////////////
//! @file BaseClasses.cpp
//! @brief Implementation of the base classes for the hospital simulation.
////////////////////////////////////////////////////////////////////////////////////////

#include "BaseClasses.hpp"
#include <DFRCore/SimEngine.hpp>
#include <DFRCore/Logger.hpp>

#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////
// ServiceProvider methods.
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
/// @brief Creates a one-time event that will notify ServiceProvider on completion of task.
/// @details This event executes a simple action at a specified simulation time and priority.
/// @param[in] simTime The simulation time at which the event will be executed.
/// @param[in] priority The priority of the event.
/// @note The event waits for a given time and will notify the service provider upon completion.
/// @return The ID of the created event. If the event could not be created, behavior is undefined.
////////////////////////////////////////////////////////////////////////////////////////
unsigned int ServiceProvider::createOneTimeEvent(double simTime, int priority)
{
    HospitalSimApplication& theApp = *mApp;
     ServiceProvider& serviceProvider = *this;

    // Get the SimEngine from the application instance.
    // NOTE: No event is created if SimEngine is NULL.
    DFR::SimEngine* engine = theApp.getSimEngine();
    if (engine == nullptr) {
        std::cerr << "Error: SimEngine pointer is null." << std::endl;
        return 0;
    }

    // Third Sanity check: Ensure we have a logger
    // NOTE: No event is created if Logger is NULL.
    if (theApp.getLogger() == nullptr) {
        std::cerr << "Error: Logger pointer is null." << std::endl;
        return 0;
    }

    // Create the one-time stop event function.
    auto oneTimeEventFn = [&theApp, engine, &serviceProvider](DFR::Event& event) {
        // Log the message
        std::string msg = "Service one-time event executed: ID(" + 
        std::to_string(event.getEventID()) + ") at simulation time: " + std::to_string(event.simTimeOfEvent());
        if (theApp.getLogger() != nullptr) {
            theApp.getLogger()->logInfo(msg);
        } else
        {
            std::cout << "Logger not available: " << msg << std::endl;
        }

        // Notify the service provider about the completion of the event
        serviceProvider.notifyEventCompletion(event.getEventID());
    };

    std::function<void(DFR::Event&)> oneTimeEventExeFn = oneTimeEventFn;
    std::unique_ptr<DFR::Event> oneTimeEvent = std::make_unique<DFR::OneShotEvent>(simTime, priority, oneTimeEventExeFn);
    return engine->addEvent(std::move(oneTimeEvent));
}

void ServiceProvider::logInfo(const std::string& message)
{
    if (mApp)
    {
        mApp->logInfo(message);
    }
}

void ServiceProvider::logWarning(const std::string& message)
{
    if (mApp)
    {
        mApp->logWarning(message);
    }
}

void ServiceProvider::logError(const std::string& message)
{
    if (mApp)
    {
        mApp->logError(message);
    }
}