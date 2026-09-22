/////////////////////////////////////////////////////////////////////////////////
//! @file Application.cpp
//! @author Thomas Kelleher
//! @date 2024-06-01
//! @brief DFR::Application class implementation.
/////////////////////////////////////////////////////////////////////////////////

#include <DFRCore/Application.hpp>
#include <DFRCore/Logger.hpp>
#include <DFRCore/RandomNumber.hpp>
#include "PriorityQueueEventManager.hpp"
#include <DFRCore/SimEngine.hpp>
#include <iostream>

//#define USEDOUT 1
#if USEDOUT
//! @brief Internal macro for "Debug Output" that can be compiled out of use. Sends string to standard out.
#define DOUT(_a) { std::cout << _a << std::endl; }
#else
//! @brief Internal macro for "Debug Output" that can be compiled out of use. Sends string to standard out.
#define DOUT(_a) {}
#endif

//! @namespace DFR
//! @brief DFRCore functionality is held within the Dialable-Fidelity-Resolution (DFR) namespace
//! The intent is to fully encapulate DFRCore's names enabling easy extention or overloading of objects from the users perfered namespace.
namespace DFR {

//! @brief Constructs a DFR::Application instance.
//! @param inBatchMode A boolean flag indicating whether the application should run in batch mode (without rendering). Default is true.
Application::Application(const bool inBatchMode) :
    mLogger(nullptr),
    mSimEngine(nullptr),
    mIsInBatchMode(inBatchMode)
{
    DOUT("DFR::Application constructor called.");
}

//! @brief Destructor - release the SimEngine
Application::~Application()
{
    DOUT("DFR::Application destructor called.");
    delete mSimEngine;
}

//! @brief Initialize the application.  Creates both the DFR::Logger and the DFR::SimEngine objects.
void Application::initialize()
{
    DOUT("Initializing DFR::Application...");
    mLogger = createLogger();
    mRandomNumberGenerator = createRandomNumberGenerator();

    // Create the simulation engine 
    std::unique_ptr<DFR::EventManager> eventManager = std::make_unique<DFR::PriorityQueueEventManager>();

    mSimEngine = new DFR::SimEngine(std::move(eventManager), mIsInBatchMode);
}

void Application::startSimulation()
{
    if (mSimEngine)
    {
        std::string theMsg{"Starting simulation engine..."};
        if (getLogger() != nullptr)
            getLogger()->logInfo(theMsg);
        else
            std::cout << "INFO: " << theMsg << std::endl;

            mSimEngine->run();
    }
    else
    {
        std::string theMsg{"Simulation engine not initialized. Call initialize() before starting the simulation."};
         if (getLogger() != nullptr)
            getLogger()->logError(theMsg);
        else
            std::cerr << "Error: " << theMsg << std::endl;
    }
}

std::unique_ptr<DFR::Logger> Application::createLogger()
{
    // Create and return a unique pointer to a DFRLogger instance
    return std::make_unique<DFR::Logger>();
}

std::unique_ptr<DFR::RandomNumber> Application::createRandomNumberGenerator()
{
    // Create and return a unique pointer to a DFRRandomNumber instance
    return std::make_unique<DFR::RandomNumber>();
}

} // namespace DFR
