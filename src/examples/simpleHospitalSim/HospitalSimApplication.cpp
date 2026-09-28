////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

#include "HospitalSimApplication.hpp"
#include <DFRCore/SimEngine.hpp>
#include <DFRCore/Logger.hpp>
#include <DFRCore/RandomNumber.hpp>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////
/// @brief Creates a one-time event that executes a simple action at a specified simulation time and priority.
/// @param[in] theApp A pointer to the application instance containing the simulation engine.
/// @param[in] simTime The simulation time at which the event will be executed.
/// @param[in] priority The priority of the event.
////////////////////////////////////////////////////////////////////////////////////////
void createOneTimeStopEvent(HospitalSimApplication& theApp, double simTime, int priority)
{
    // Get the SimEngine from the application instance.
    // NOTE: No event is created if SimEngine is NULL.
    DFR::SimEngine* engine = theApp.getSimEngine();
    if (engine == nullptr) {
        std::cerr << "Error: SimEngine pointer is null." << std::endl;
        return;
    }

    // Third Sanity check: Ensure we have a logger
    // NOTE: No event is created if Logger is NULL.
    if (theApp.getLogger() == nullptr) {
        std::cerr << "Error: Logger pointer is null." << std::endl;
        return;
    }

    // Create the one-time stop event function.
    auto oneTimeEventFn = [&theApp, engine](DFR::Event& event) {
        // Log the message
        std::string msg = "One-time stop event executed at simulation time: " + std::to_string(event.simTimeOfEvent());
        if (theApp.getLogger() != nullptr) {
            theApp.getLogger()->logInfo(msg);
        } else
        {
            std::cout << "Logger not available: " << msg << std::endl;
        }

        // Stop the execution
        engine->stopExecution();
    };

    std::function<void(DFR::Event&)> oneTimeEventExeFn = oneTimeEventFn;
    std::unique_ptr<DFR::Event> oneTimeEvent = std::make_unique<DFR::OneShotEvent>(simTime, priority, oneTimeEventExeFn);
    engine->addEvent(std::move(oneTimeEvent));
}

////////////////////////////////////////////////////////////////////////////////////////
/// @brief Creates a repeating event that executes a simple action at specified simulation times and priority, and reschedules itself for future execution.
/// @param[in] theApp A reference to the application instance containing the simulation engine.
/// @param[in] simTime The initial simulation time at which the event will be executed.
/// @param[in] priority The priority of the event.
/// @param[in] incrementTime The time increment for rescheduling the event for future execution.
/// @return The ID of the created event, or 0 if the event could not be created.
////////////////////////////////////////////////////////////////////////////////////////
unsigned int createPatientRepeatingEvent(HospitalSimApplication& theApp, double simTime, int priority, double incrementTime)
{
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

    auto repeatingEventFn = [&theApp, incrementTime](DFR::Event& event) {
        // Do Action!
        double diceRoll = theApp.getRandomNumberGenerator()->getRandomDouble(0.0, 100.0);
        bool addPatient = (diceRoll >= 50.0);
        std::string msg = "Patient poll event: at simulation time: " + std::to_string(event.simTimeOfEvent()) + ", dice roll: " + std::to_string(diceRoll);
        msg += (addPatient ? " - ADD patient" : " - NO patient added");
        theApp.getLogger()->logInfo(msg);

        // Perform the action
        if (addPatient) {
            theApp.injectPatient();
        }

        // Reschedule the event for future execution
        event.updateSimTime(event.simTimeOfEvent() + incrementTime); // Reschedule the event for a future time
        return DFR::Event::EventStatus::eReschedule; // Indicate that the event should be rescheduled
    };

    std::function<DFR::Event::EventStatus(DFR::Event&)> repeatingEventExeFn = repeatingEventFn;
    std::unique_ptr<DFR::Event> repeatingEvent = std::make_unique<DFR::RepeatingEvent>(simTime, priority, repeatingEventExeFn);
    return (engine->addEvent(std::move(repeatingEvent)));
}


////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
HospitalSimApplication::HospitalSimApplication(bool inBatchMode)
    : DFR::Application(inBatchMode)
{
}

//! @brief Initialize the hospital simulation (OVERLOADED METHOD)
void HospitalSimApplication::initialize()
{
    // Ensure the DFRCore application is initialized before performing local initialization.
    DFR::Application::initialize();

    // ===========================================================
    // Local initialization for the hospital simulation.
    // ===========================================================

    // KEY USAGE: Initialize Random Number Generator
    // NOTE: Initializing the random number generator with a fixed seed for reproducibility
    unsigned int seed = 12345;
    getRandomNumberGenerator()->setSeed(seed);
    std::string msg = "Random number generator initialized with seed: " + std::to_string(seed);
    getLogger()->logInfo(msg);

    // KEY USAGE: Determine the length of the simulation
    // NOTE: Implements a one-time event to stop the simulation after a certain simulation time
    double simulationLength = 60.0; // Total length of the simulation in simulation time units
    createOneTimeStopEvent(*this, simulationLength, 1);  // Priority 1 (lowest-best)

    // KEY USAGE: Determine the time increment for the repeating event for patients polling
    // NOTE: Implements a repeating event that occurs at regular intervals
    double incrementTime = 5.0; // Time increment for the repeating event
    unsigned int patientRepeatingEventID = createPatientRepeatingEvent(*this, 10.0, 1, incrementTime);
}

void HospitalSimApplication::injectPatient()
{
    // Implementation for injecting a patient into the hospital simulation.
    std::string msg = "Injecting a patient into the hospital simulation.";
    getLogger()->logInfo(msg);
}
