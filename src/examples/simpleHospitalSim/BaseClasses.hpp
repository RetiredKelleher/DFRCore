////////////////////////////////////////////////////////////////////////////////////////
//! @file BaseClasses.hpp
//! @brief Contains the base classes for the simple hospital simulation.
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "HospitalSimApplication.hpp"
#include "DataTypes.hpp"

//! @brief Base Class to provide service to patients.
class ServiceProvider
{
public:
    ServiceProvider(HospitalSimApplication* app) : mApp(app) {}
    virtual ~ServiceProvider() = default;

    //! @brief Can the service provider currently provide service to a patient?
    //! @return True if the service provider can currently provide service, false otherwise.
    virtual bool canProvideService() = 0;

    //! @brief Intake a patient.
    //! @param[in] patient The patient to intake.
    virtual void intakePatient(Patient* patient) = 0;

    //! @brief Discharge a patient.
    //! @param[in] patient The patient to discharge.
    virtual void dischargePatient(Patient* patient) = 0;

    //! @brief Display final Statistics for the service provider.
    //! Used to inform the user about the final statistics of the service provider.
    virtual void displayFinalStatistics(const int indentSpaces=0) = 0;

    /** @name Support for event callbacks */
    //@{
    //! @brief Create a one-time event for the service provider.
    //! @param[in] simTime The simulation time at which the event will be executed.
    //! @param[in] priority The priority of the event.
    //! @return The ID of the created event. If the event could not be created, behavior is undefined.
    virtual unsigned int createOneTimeEvent(double simTime, int priority);
    //! @brief notifiy event completion
    //! @param[in] eventId The ID of the event that has completed.
    //! @note Supports one-shot or repeating events, but the implementation is left to the derived class.
    virtual void notifyEventCompletion(unsigned int eventId) {};
    //@}

protected:

    /** @name Log a message to the log file with a timestamp */
    //@{
    //! @brief Log an informational message to the log file with a timestamp.
    //! @param[in] message The message to log
    void logInfo(const std::string& message);
    //! @brief Log a warning message to the log file with a timestamp.
    //! @param[in] message The message to log
    void logWarning(const std::string& message);
    //! @brief Log an error message to the log file with a timestamp.
    //! @param[in] message The message to log
    void logError(const std::string& message);
    //@}

    HospitalSimApplication* mApp;    //!< Pointer to the hospital simulation application instance.
};
