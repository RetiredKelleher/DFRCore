////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

#include "HospitalSimApplication.hpp"
#include <DFRCore/SimEngine.hpp>
#include <DFRCore/Logger.hpp>
#include <DFRCore/RandomNumber.hpp>

#include "HospitalUtils.hpp"
#include "ControlSwitches.hpp"

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
        // Stop if the maximum number of patients has been reached
        if (theApp.getTotalAcceptedPatients() < cMaxPatientsInjected)
        {
            double diceRoll = theApp.getRandomNumberGenerator()->getRandomDouble(0.0, 100.0);
            bool addPatient = (diceRoll >= 50.0);
            std::string msg = "Patient poll event: at simulation time: " + std::to_string(event.simTimeOfEvent()) + ", dice roll: " + std::to_string(diceRoll);
            msg += (addPatient ? " - ADD patient" : " - NO patient added");
            theApp.getLogger()->logInfo(msg);

            // Perform the action
            if (addPatient) {
                theApp.injectPatient();
            }
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
    // NOTE: Services created in the initialize method
}

HospitalSimApplication::~HospitalSimApplication()
{
    delete mAdminIntakeService;
    delete mRecoveryRoomService;
    delete mUrgentCareService;
    delete mBloodDrawService;

    // Release all the dynamically allocated patients
    for (Patient* patient : mPatients) {
        delete patient;
    }
    mPatients.clear(); // Clear the vector after deleting the patients
}

//! @brief Initialize the hospital simulation (OVERLOADED METHOD)
//! @details This method initializes the hospital simulation, including (1) setting up the
//! random number generator, (2) determining the simulation length, and (3) creating repeating
//! events for patient polling.
void HospitalSimApplication::initialize()
{
    // Ensure the DFRCore application is initialized before performing local initialization.
    DFR::Application::initialize();

    // ===========================================================
    // Local initialization for the hospital simulation.
    // ===========================================================

    // KEY USAGE: Initialize Random Number Generator
    // NOTE: Initializing the random number generator with a fixed seed for reproducibility
    getRandomNumberGenerator()->setSeed(cSimulationRandomSeed);
    std::string msg = "Random number generator initialized with seed: " + std::to_string(cSimulationRandomSeed);
    logInfo(msg);

    // KEY USAGE: Determine the length of the simulation
    // NOTE: Implements a one-time event to stop the simulation after a certain simulation time
    double simulationLength = cSimulationDuration; // Total length of the simulation in simulation time units
    createOneTimeStopEvent(*this, simulationLength, 1);  // Priority 1 (lowest-best)
    msg = "One-time stop event created with simulation length: " + std::to_string(simulationLength);
    logInfo(msg);

    // KEY USAGE: Determine the time increment for the repeating event for patients polling
    // NOTE: Implements a repeating event that occurs at regular intervals
    double incrementTime = cPatientInTakePollingInterval; // Time increment for the repeating event
    unsigned int patientRepeatingEventID = createPatientRepeatingEvent(*this, cPatientInTakePollingInterval, 1, incrementTime);
    msg = "Patient repeating event created with interval: " + std::to_string(cPatientInTakePollingInterval);
    logInfo(msg);

    //
    // Create the Hospital Services
    //
    mRecoveryRoomService = new RecoveryRoomService(this);  //!< Initialize the post-operative care recovery room service.
    mUrgentCareService = new UrgentCareService(this);  //!< Initialize the urgent care service.
    mAdminIntakeService = new AdminIntakeService(this);  //!< Initialize the front desk administrative intake service.

    // Sub-service for the hospital simulation
    mBloodDrawService = new BloodDrawService(this);  //!< Initialize the blood draw service.
}

//! @brief Log an informational message to the log file with a timestamp.
//! @param[in] message The message to log
void HospitalSimApplication::logInfo(const std::string& message)
{
    getLogger()->logInfo(message);
}

//! @brief Log a warning message to the log file with a timestamp.
//! @param[in] message The message to log
void HospitalSimApplication::logWarning(const std::string& message)
{
    getLogger()->logWarning(message);
}

//! @brief Log an error message to the log file with a timestamp.
//! @param[in] message The message to log
void HospitalSimApplication::logError(const std::string& message)
{
    getLogger()->logError(message);
}

//! @brief Inject a patient into the hospital simulation.
//! @details This method handles the process of injecting a patient into the hospital simulation, including logging the action.
void HospitalSimApplication::injectPatient()
{
    // Determine if we CAN inject a new patient into the hospital simulation based on current conditions.
    bool canInjectPatient = mAdminIntakeService->canProvideService();

    // Generate a new patient and add it to the list of patients in the hospital simulation.
    if (canInjectPatient)
    {
        Patient* newPatient = generatePatient();
        mPatients.push_back(newPatient);

        // Implementation for injecting a patient into the hospital simulation.
        std::string msg = "Injecting a patient into the hospital simulation.";
        logInfo(msg);

        // Additional implementation for injecting a patient can be added here.
        mAdminIntakeService->intakePatient(newPatient);
        mTotalAcceptedPatients++;
    }
    else
    {
        mTotalRejectedPatients++;
        std::string msg = "REJECT: Cannot inject a patient into the hospital simulation at this time.";
        logInfo(msg);
    }

}

//! @brief Can Discharge into Urgent Care.
//! @param[in] patient The patient to check for discharge eligibility.
//! @return True if the patient can be discharged into the urgent care, false otherwise.
bool HospitalSimApplication::dischargeToUrgentCareIfPossible(Patient* patient)
{
    if (mUrgentCareService && mUrgentCareService->canProvideService())
    {
        mUrgentCareService->intakePatient(patient);
        return true;
    }
    return false;
}

//! @brief Can Discharge into Recovery Room.
//! @param[in] patient The patient to check for discharge eligibility.
//! @return True if the patient can be discharged into the recovery room, false otherwise.
bool HospitalSimApplication::dischargeToRecoveryRoomIfPossible(Patient* patient)
{
    if (mRecoveryRoomService && mRecoveryRoomService->canProvideService())
    {
        mRecoveryRoomService->intakePatient(patient);
        return true;
    }
    return false;
}

//! @brief Discharge a patient from the hospital simulation.
//! @param[in] patient The patient to discharge.
void HospitalSimApplication::dischargePatient(Patient* patient)
{
    if (patient)
    {
        mTotalDischargedPatients++;
        std::string msg = "Discharging a patient from the hospital simulation.";
        logInfo(msg);
        // Additional implementation for discharging a patient can be added here.
    }
}

//! @brief Attempt to get sub-service Blood Draw if possible.
//! @param[in] patient The patient to check for discharge eligibility.
//! @return True if the patient can be discharged into the urgent care, false otherwise.
bool HospitalSimApplication::performBloodDrawServiceIfPossible(Patient* patient)
{
    if (mBloodDrawService && mBloodDrawService->canProvideService())
    {
        mBloodDrawService->intakePatient(patient);
        return true;
    }
    return false;
}

void HospitalSimApplication::subServiceCompletionNotification(Patient* patient)
{
    if (patient)
    {
        patient->mActiveSubService = false;  // Mark the patient as no longer undergoing a sub-service
        std::string msg = "Sub-service completed for Patient ID(" + std::to_string(patient->mID) + ").";
        logInfo(msg);

        // Ask services to reevaluate the patient for discharge eligibility
        if (mUrgentCareService)
        {
            mUrgentCareService->reevaluatePatientDischargeEligibility();
        }
    }
}

//! @brief Display final statistics for all hospital services.
void HospitalSimApplication::displayFinalStatistics()
{
    std::string msg = "===============================";
    getLogger()->logInfo(msg);
    msg = "Displaying final statistics for all hospital services.";
    getLogger()->logInfo(msg);
    msg = "Total Accepted Patients: " + std::to_string(mTotalAcceptedPatients);
    getLogger()->logInfo(msg);
    msg = "Total Rejected Patients: " + std::to_string(mTotalRejectedPatients);
    getLogger()->logInfo(msg);
    msg = "Total Discharged Patients: " + std::to_string(mTotalDischargedPatients);
    getLogger()->logInfo(msg);
    msg = "===============================";
    getLogger()->logInfo(msg);

    if (mAdminIntakeService)
    {
        mAdminIntakeService->displayFinalStatistics();
    }
    if (mUrgentCareService)
    {
        mUrgentCareService->displayFinalStatistics();
    }
    if (mBloodDrawService)
    {
        mBloodDrawService->displayFinalStatistics();
    }
    if (mRecoveryRoomService)
    {
        mRecoveryRoomService->displayFinalStatistics();
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// Protected Methods
////////////////////////////////////////////////////////////////////////////////////////

Patient* HospitalSimApplication::generatePatient()
{
    Patient* newPatient = new Patient();
    newPatient->mID = int(mPatients.size());
    newPatient->mIsIdle = true;  // Initialize the patient as idle

    // Determine the types of mandatory service required by the patient
    newPatient->mServiceRequired ^= int(Patient::ServiceType::eAdminIntake);  // All patients require admin intake
    newPatient->mServiceRequired ^= int(Patient::ServiceType::eRecoveryRoom); // Some patients may require recovery room

    // Determine the types of service required by the patient based on random selection
    newPatient->mServiceRequired ^= int(Patient::ServiceType::eUrgentCare);   // Some patients may require urgent care

    double diceRoll{0};
    // Determine OPTIONAL sub-service: Blood Draw
    diceRoll = getRandomNumberGenerator()->getRandomDouble(0.0, 100.0);
    if (diceRoll < cBloodDrawRandomChance) // cBloodDrawRandomChance% chance of requiring blood draw service
    {
        newPatient->mServiceRequired ^= int(Patient::ServiceType::eBloodDraw);
    }

    return newPatient;
}