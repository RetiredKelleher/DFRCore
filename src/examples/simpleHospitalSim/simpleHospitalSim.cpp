////////////////////////////////////////////////////////////////////////////////////////
//! @file simpleHospitalSim.cpp
//! @brief simpleHospitalSim: Simple Hospital Queue Simulation using DFRCore
//! @author Thomas Kelleher
//! @date 2024-06-01
//! @version 1.0
//! @details This example demonstrates a basic simulation using the DFRCore framework.
//! The concept is to model a hospital queue system, using the DFRCore events.
//!
//! Key concepts to use from DFRCore:
//! - Creating one-time events
//! - Creating repeating events
//! - Managing event priorities
//! - Use of the Logger
//! - Use of the Random Number Generator
//! - Stopping the simulation engine
//!
//! Main flow of the simulation:
//! 1. Initialize the simulation engine.
//! 2. Initialize the Random Number Generator with a seed value.
//! 2.1. This ensures reproducibility of random events in the simulation.
//! 3. Create a repeating event to poll if a new patient has arrived.
//! 3.1. Use Random Number Generator to determine if a new patient has arrived.
//! 4. Run the simulation engine.
//! 5. Stop the simulation engine when appropriate.
//!
////////////////////////////////////////////////////////////////////////////////////////

#include "HospitalSimApplication.hpp"
#include <DFRCore/Logger.hpp>

// ===========================================================================
// Main function to run the simulation
// ===========================================================================
int main() {
    // Create an instance of the HospitalSimApplication
    bool inBatchMode = true; // Set to true for batch mode (no rendering), false for real-time mode (with rendering)
    HospitalSimApplication app(inBatchMode); // false indicates we are not in batch mode (i.e., we want rendering)

    // Initialize the application
    app.initialize();

    // Start the simulation engine
    app.startSimulation();

    // Simulation has completed at this point.
    std::string msg = "Simulation completed.";
    app.getLogger()->logInfo(msg);
    return 0;
}