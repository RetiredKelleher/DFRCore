////////////////////////////////////////////////////////////////////////////////////////
// @file HospitalSimApplication.hpp
// @brief Header file for the HospitalSimApplication class.
// @details This file contains the declaration of the HospitalSimApplication class,
// which is a derived class of DFR::Application. It provides the main application logic
// for the simple hospital simulation.  The class manages the simulation of a hospital
// environment, including patient intake, service provision, and discharge processes.
// It also handles logging and random number generation for the simulation.
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <DFRCore/Application.hpp>
#include "DataTypes.hpp"
#include <vector>

class AdminIntakeService;
class UrgentCareService;
class RecoveryRoomService;
class BloodDrawService;
class HospitalSimApplication : public DFR::Application
{
public:
    HospitalSimApplication(bool inBatchMode);
    virtual ~HospitalSimApplication();

    //! @brief Initialize the hospital simulation.
    void initialize() override;

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

    /** @name Example Hospital Simulation Methods */
    //@{
    //! @brief Total number of patients injected into the hospital simulation.
    int getTotalAcceptedPatients() const { return mTotalAcceptedPatients; }

    //! @brief Inject a patient into the hospital simulation.
    void injectPatient();

    //! @brief Attempt to discharge a patient into the urgent care if possible.
    //! @param[in] patient The patient to check for discharge eligibility.
    //! @return True if the patient can be discharged into the urgent care, false otherwise.
    bool dischargeToUrgentCareIfPossible(Patient* patient);

    //! @brief Attempt to discharge a patient into the recovery room if possible.
    //! @param[in] patient The patient to check for discharge eligibility.
    //! @return True if the patient can be discharged into the recovery room, false otherwise.
    bool dischargeToRecoveryRoomIfPossible(Patient* patient);

    //! @brief Discharge a patient from the hospital simulation.
    void dischargePatient(Patient* patient);

    //! @brief Attempt to get sub-service Blood Draw if possible.
    //! @param[in] patient The patient to check for discharge eligibility.
    //! @return True if the patient can be discharged into the urgent care, false otherwise.
    bool performBloodDrawServiceIfPossible(Patient* patient);

    //! @brief Notification of sub-service completion for a patient.
    //! @param[in] patient The patient for which the sub-service has been completed.
    void subServiceCompletionNotification(Patient* patient);
    //@}

    //! @brief Display final statistics for all hospital services.
    void displayFinalStatistics();

protected:

    //! @brief Generate a new patient for the hospital simulation.
    Patient* generatePatient();

private:

    std::vector<Patient*> mPatients;    //!< The list of patients in the hospital simulation.
    int mTotalRejectedPatients{0};    //!< The total number of patients rejected from the hospital simulation.
    int mTotalAcceptedPatients{0};    //!< The total number of patients accepted into the hospital simulation.
    int mTotalDischargedPatients{0};    //!< The total number of patients discharged from the hospital simulation.

    // Model Parts of this hospital
    AdminIntakeService* mAdminIntakeService{nullptr};    //!< The front desk administrative intake service.
    UrgentCareService* mUrgentCareService{nullptr};    //!< The urgent care service.
    RecoveryRoomService* mRecoveryRoomService{nullptr};    //!< The post-operative care recovery room service.
    BloodDrawService* mBloodDrawService{nullptr};    //!< The blood draw service.
};
