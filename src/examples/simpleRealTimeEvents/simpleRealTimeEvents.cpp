////////////////////////////////////////////////////////////////////////////////////////
//! @file simpleRealTimeEvents.cpp
//! @brief simpleRealTimeEvents: Basic Simulation using real-time clock
//! @author Thomas Kelleher
//! @date 2024-06-01
//! @version 1.0
//! @details This example demonstrates a basic simulation using the DFRCore framework. It initializes
//! a simple simulation environment, runs a few steps, and outputs the results.
////////////////////////////////////////////////////////////////////////////////////////

#include <DFRCore/Application.hpp>
#include <DFRCore/SimEngine.hpp>
#include <iostream>
//#include <memory>
//#include <functional>

////////////////////////////////////////////////////////////////////////////////////////
/// @brief Creates a one-time event that executes a simple action at a specified simulation time and priority.
/// @param engine A pointer to the simulation engine to which the event will be added. 
/// @param simTime The simulation time at which the event will be executed.
/// @param priority The priority of the event.
////////////////////////////////////////////////////////////////////////////////////////
void createOneTimeEvent(DFR::SimEngine* engine, double simTime, int priority)
{
    auto oneTimeEventFn = [](DFR::Event& event) {
        std::cout << "One-time event executed at simulation time: " << event.simTime() << std::endl;
    };
    std::function<void(DFR::Event&)> oneTimeEventExeFn = oneTimeEventFn;
    std::unique_ptr<DFR::Event> oneTimeEvent = std::make_unique<DFR::OneShotEvent>(simTime, priority, oneTimeEventExeFn);
    engine->addEvent(std::move(oneTimeEvent), priority);
}

////////////////////////////////////////////////////////////////////////////////////////
/// @brief Creates a one-time event that executes a simple action at a specified simulation time and priority.
/// @param engine A pointer to the simulation engine to which the event will be added. 
/// @param simTime The simulation time at which the event will be executed.
/// @param priority The priority of the event.
////////////////////////////////////////////////////////////////////////////////////////
void createOneTimeStopEvent(DFR::SimEngine* engine, double simTime, int priority)
{
    auto oneTimeEventFn = [engine](DFR::Event& event) {
        std::cout << "One-time stop event executed at simulation time: " << event.simTime() << std::endl;
        engine->stopExecution();
    };
    std::function<void(DFR::Event&)> oneTimeEventExeFn = oneTimeEventFn;
    std::unique_ptr<DFR::Event> oneTimeEvent = std::make_unique<DFR::OneShotEvent>(simTime, priority, oneTimeEventExeFn);
    engine->addEvent(std::move(oneTimeEvent), priority);
}

////////////////////////////////////////////////////////////////////////////////////////
/// @brief Creates a repeating event that executes a simple action at specified simulation times and priority, and reschedules itself for future execution.
/// @param engine A pointer to the simulation engine to which the event will be added. 
/// @param simTime The initial simulation time at which the event will be executed.
/// @param priority The priority of the event.
/// @param incrementTime The time increment for rescheduling the event for future execution.
/// @param eventID An optional identifier for the event, which can be used for tracking or categorization purposes.
////////////////////////////////////////////////////////////////////////////////////////
void createRepeatingEvent(DFR::SimEngine* engine, double simTime, int priority, double incrementTime,
const int eventID = 0)
{
    auto repeatingEventFn = [incrementTime](DFR::Event& event) {
        std::cout << "Repeating event executed at simulation time: " << event.simTime() << std::endl;
        event.updateSimTime(event.simTime() + incrementTime); // Reschedule the event for a future time
        return DFR::Event::EventStatus::eReschedule; // Indicate that the event should be rescheduled
    };
    std::function<DFR::Event::EventStatus(DFR::Event&)> repeatingEventExeFn = repeatingEventFn;
    std::unique_ptr<DFR::Event> repeatingEvent = std::make_unique<DFR::RepeatingEvent>(simTime, priority, repeatingEventExeFn);
    repeatingEvent->setEventID(eventID);
    engine->addEvent(std::move(repeatingEvent), priority);
}

// ===========================================================================
// Main function to run the simulation
// ===========================================================================
int main() {
    // Create an instance of the DFRApplication
    bool inBatchMode = false; // Set to true for batch mode (no rendering), false for real-time mode (with rendering)
    DFR::Application app(inBatchMode); // false indicates we are not in batch mode (i.e., we want rendering)

    // Initialize the application
    app.initialize();

    std::cout << "Starting simulation engine..." << std::endl;
    double incrementTime = 5.0; // Time increment for the repeating event

    // Define some events and add them to the engine's event manager here (not implemented in this snippet)
    {
        // Example of adding an event to the event manager
        // std::unique_ptr<Event> event = std::make_unique<YourEventType>(/* event parameters */);
        // engine.getEventManager().addEvent(std::move(event), /* priority */);

        // Add one-time event to perform stop execution after a certain simulation time
        createOneTimeEvent(app.getSimEngine(), 38.0, 1);

        // Add one-time event to perform stop execution after a certain simulation time
        createOneTimeStopEvent(app.getSimEngine(), 50.0, 1);

        // Add a repeating event to demonstrate normal event processing
        createRepeatingEvent(app.getSimEngine(), 10.0, 3, incrementTime, 1);

        // Add a repeating event to demonstrate normal event processing
        createRepeatingEvent(app.getSimEngine(), 10.0, 1, incrementTime, 2);
    }

    // Start the simulation engine
    app.startSimulation();

    // Simulation has completed at this point.
    std::cout << "Simulation completed." << std::endl;
    return 0;
}