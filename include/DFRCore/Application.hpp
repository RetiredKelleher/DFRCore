///////////////////////////////////////////////////////////////////////////////
//! @file Application.hpp
//! @brief DFR::Application class definition.
//! This class in intended to be an outline of applications utilize the DFRCore
//! package.
//! @author Thomas Kelleher
//! @date 2024-06-01
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>

//! @namespace DFR
//! @brief DFRCore functionality is held within the Dialable-Fidelity-Resolution (DFR) namespace
//! The intent is to fully encapulate DFRCore's names enabling easy extention or overloading of objects from the users perfered namespace.
namespace DFR {
class Logger; // Forward declaration of DFR::Logger
class RandomNumber; // Forward declaration of DFR::RandomNumber
class SimEngine; // Forward declaration of DFR::SimEngine
} // Namespace DFR

namespace DFR {

//! @brief DFR::Application contains the basic building blocks for the simulation.
//! Its intended to provide a starting spot for others, while still supporting
//! customization for any project needs.
class Application
{
public:
    //! @brief Constructs a DFR::Application instance.
    //! @param[in] inBatchMode A boolean flag indicating whether the application should run in batch mode (as fast as possible). Default is true.
    Application(const bool inBatchMode = true);

    //! @brief Destroys the DFR::Application instance.
    virtual ~Application();
    
    //! @brief Initializes the application.
    virtual void initialize();

    /** @name Simulation Management */
    //@{
    //! @brief Expose the Simulation Engine to allow for adding events and controlling the simulation.
    //! @return A pointer to the simulation engine instance.
    DFR::SimEngine* getSimEngine() { return mSimEngine; }
    //! @brief Starts the simulation by running the simulation engine.
    void startSimulation();
    //@}

    //! @brief Gets the logger instance for logging messages from the application.
    //! @return A pointer to the DFR::Logger instance.
    DFR::Logger* getLogger() { return mLogger.get(); }

    //! @brief Gets the random number generator instance.
    //! @return A pointer to the DFR::RandomNumber instance.
    DFR::RandomNumber* getRandomNumberGenerator() { return mRandomNumberGenerator.get(); }

protected:

    /** @name Factory Methods */
    //!@{
    //! @brief Construct a DFR::Logger instance for logging messages from the application.
    //! Overload this method for project customization, but must assign mLogger variable.
    //! @return A pointer to the DFR::Logger instance.
    virtual std::unique_ptr<DFR::Logger> createLogger();

    //! @brief Construct a DFR::RandomNumber instance for generating random numbers in the application.
    //! Overload this method for project customization, but must assign mRandomNumberGenerator variable.
    //! @return A pointer to the DFR::RandomNumber instance.
    virtual std::unique_ptr<DFR::RandomNumber> createRandomNumberGenerator();
    //!@}

private:

    //! Logger instance for logging messages
    std::unique_ptr<DFR::Logger> mLogger;

    //! Random number generator instance
    std::unique_ptr<DFR::RandomNumber> mRandomNumberGenerator;

    //! Simulation engine instance
    DFR::SimEngine* mSimEngine;

    //! Flag to indicate if the application is running in batch mode (as fast as possible)
    bool mIsInBatchMode{false};
};

} // namespace DFR
