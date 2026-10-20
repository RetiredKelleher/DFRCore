////////////////////////////////////////////////////////////////////////////////////////
//! @file HospitalUtils.cpp
//! @brief Utility functions and classes for the hospital simulation.
////////////////////////////////////////////////////////////////////////////////////////

#include "HospitalUtils.hpp"
#include "ControlSwitches.hpp"

#include <DFRCore/SimEngine.hpp>
#include <DFRCore/RandomNumber.hpp>

static int gWorkerIdCounter = 0;  //!< Global counter to assign unique IDs to workers.

////////////////////////////////////////////////////////////////////////////////////////
// HospitalQueue Implementation
////////////////////////////////////////////////////////////////////////////////////////

HospitalQueue::HospitalQueue(HospitalSimApplication* app)
    : mApp(app)
{
}

void HospitalQueue::enqueue(Patient* patient)
{
    mQueue.push(patient);
}

Patient* HospitalQueue::dequeue()
{
    if (mQueue.empty())
    {
        return nullptr;
    }
    Patient* patient = mQueue.front();
    mQueue.pop();
    return patient;
}

Patient* HospitalQueue::dequeueIdleStatePatient(bool isIdle)
{
    if (mQueue.empty())
    {
        return nullptr;
    }

    std::queue<Patient*> tempQueue;
    Patient* idlePatient = nullptr;

    while (!mQueue.empty())
    {
        Patient* patient = mQueue.front();
        mQueue.pop();
        if (patient->mIsIdle == isIdle && idlePatient == nullptr)
        {
            idlePatient = patient;
        }
        else
        {
            tempQueue.push(patient);
        }
    }

    mQueue = std::move(tempQueue);
    return idlePatient;
}

Patient* HospitalQueue::dequeuePatientWithServiceCompleted(int serviceTypeBitmask, bool isIdle)
{
    if (mQueue.empty())
    {
        return nullptr;
    }

    std::queue<Patient*> tempQueue;
    Patient* matchingPatient = nullptr;

    while (!mQueue.empty())
    {
        Patient* patient = mQueue.front();
        mQueue.pop();
        if (patient->mIsIdle == isIdle && (patient->mServiceRequired & serviceTypeBitmask) == 0 && matchingPatient == nullptr)
        {
            matchingPatient = patient;
        }
        else
        {
            tempQueue.push(patient);
        }
    }

    mQueue = std::move(tempQueue);
    return matchingPatient;
}

Patient* HospitalQueue::dequeueWaitingPatientForService(int serviceTypeBitmask, bool isIdle)
{
    if (mQueue.empty())
    {
        return nullptr;
    }

    std::queue<Patient*> tempQueue;
    Patient* matchingPatient = nullptr;

    while (!mQueue.empty())
    {
        Patient* patient = mQueue.front();
        mQueue.pop();
        if ((patient->mServiceRequired & serviceTypeBitmask) != 0 &&
            patient->mIsIdle == isIdle && matchingPatient == nullptr)
        {
            matchingPatient = patient;
        }
        else
        {
            tempQueue.push(patient);
        }
    }

    mQueue = std::move(tempQueue);
    return matchingPatient;
}

bool HospitalQueue::isEmpty() const
{
    return mQueue.empty();
}

size_t HospitalQueue::getQueueSizeForService(int serviceTypeBitmask) const
{
    size_t count = 0;
    std::queue<Patient*> tempQueue = mQueue;  // Create a copy to iterate through

    while (!tempQueue.empty())
    {
        Patient* patient = tempQueue.front();
        tempQueue.pop();
        if ((patient->mServiceRequired & serviceTypeBitmask) != 0)
        {
            ++count;
        }
    }

    return count;
}
size_t HospitalQueue::getQueueSizeForIdleState(bool isIdle) const
{
    size_t count = 0;
    std::queue<Patient*> tempQueue = mQueue;  // Create a copy to iterate through

    while (!tempQueue.empty())
    {
        Patient* patient = tempQueue.front();
        tempQueue.pop();
        if (patient->mIsIdle == isIdle)
        {
            ++count;
        }
    }

    return count;
}

double HospitalQueue::getSimulationTime() const
{
    return mApp->getSimEngine()->getCurrentSimTime();
}

////////////////////////////////////////////////////////////////////////////////////////
//! @brief Model a worker in the hospital simulation.
//! The worker service performs some action on patients, such as providing admin intake
//! or providing other types of services.
////////////////////////////////////////////////////////////////////////////////////////    
 WorkerService::WorkerService(HospitalSimApplication* app, ServiceProvider* parent,
     double minServiceTime, double maxServiceTime, Patient::ServiceType serviceTypeServed)
    : ServiceProvider(app), mParent(parent), mCurrentPatient(nullptr), 
    mMinServiceTime(minServiceTime), mMaxServiceTime(maxServiceTime)
{
    mServiceTypeServed = int(serviceTypeServed);  // Store the type of service this worker provides.

    mWorkerId = gWorkerIdCounter++;  // Create ID for logging
    mCurrentEventID = 0;  // Initialize the event ID for the worker service.

    std::string msg = getWorkerServiceInfo() + " : Initialized";
    logInfo(msg);
}
bool WorkerService::canProvideService()
{
    // Implement the logic to determine if the worker service can currently provide service
    // For simplicity, let's assume the worker service can always provide service.
    bool canProvide = (mCurrentPatient == nullptr);
//    std::string msg = getWorkerServiceInfo() + " : canProvideService: " + std::string(canProvide ? "true" : "false");
//    logInfo(msg);
    return canProvide;
}

void WorkerService::intakePatient(Patient* patient)
{
    // Perform a sanity check to ensure the worker is not already serving another patient.
    if (mCurrentPatient != nullptr)
    {
        std::string msg = getWorkerServiceInfo() + " : intakePatient: ERROR - Worker is already serving another patient.";
        logInfo(msg);
        return;
    }

    // Implement the intake logic for the worker service here
    mCurrentPatient = patient;
    mCurrentPatient->mIsIdle = false;
    std::string msg = getWorkerServiceInfo() + " : intakePatient: Patient intake started.";
    logInfo(msg);

    // Create an event for the patient intake process.
    mScheduledServiceTime = mMaxServiceTime;
    if (mMaxServiceTime != mMinServiceTime)
        mScheduledServiceTime = mApp->getRandomNumberGenerator()->getRandomDouble(mMinServiceTime, mMaxServiceTime);

    double scheduleTime = mApp->getSimEngine()->getCurrentSimTime() + mScheduledServiceTime;
    int priority = 16;  // Example priority, adjust as needed

    mCurrentEventID = createOneTimeEvent(scheduleTime, priority);
    if (mCurrentPatient != nullptr)
    {
        std::string msg = getWorkerServiceInfo() + " : checkInTime: " + std::to_string(scheduleTime) + " intakePatient: Patient intake completed.";
        logInfo(msg);
    }
}

void WorkerService::dischargePatient(Patient* patient)
{
    // Implement the discharge logic for the worker service here
    if (mCurrentPatient == patient && mCurrentPatient != nullptr)
    {
        mCurrentPatient->mIsIdle = true;
        mCurrentPatient = nullptr;
    }
    std::string msg = getWorkerServiceInfo() + " : dischargePatient: Patient discharged.";
    logInfo(msg);
}

void WorkerService::displayFinalStatistics(const int indentSpaces)
{
    // Implement the logic to display final statistics for the worker service here
    std::string indent(indentSpaces, ' ');
    std::string msg = indent + getWorkerServiceInfo() + " : Processed (Count/Time) = " +
     std::to_string(mStats.mServiceProcessed) + " / " + std::to_string(mStats.mTotalServiceTime);
    if (mCurrentPatient != nullptr)
    {
        msg += " (Currently serving a patient)";
    }
    logInfo(msg);
}

void WorkerService::notifyEventCompletion(unsigned int eventId)
{
    // Perform sanity check to ensure the event ID is valid.
    if (eventId != mCurrentEventID)
    {
        std::string msg = getWorkerServiceInfo() + " : notifyEventCompletion: ERROR - Event ID " + std::to_string(eventId) + " does not match the current event ID.";
        logInfo(msg);
        return;
    }

    // Notify parent ServiceProvider about the event completion.
    if (mParent != nullptr)
    {
        std::string msg = getWorkerServiceInfo() + " : notifyEventCompletion: Service completed for Patient ID(" +
          std::to_string(mCurrentPatient->mID) + "). Service Type: " + 
          std::to_string(mServiceTypeServed) + " Patient bitmask service: " + 
          std::to_string(mCurrentPatient->mServiceRequired);
        logInfo(msg);

        // Update Patient states and track the stats
        if (mCurrentPatient->mServiceRequired & mServiceTypeServed)
        {
            mCurrentPatient->mServiceRequired ^= mServiceTypeServed;  // Update the patient's service type to reflect the completed service.
            mCurrentPatient->mIsIdle = true;  // Mark the patient as idle after service completion.
            msg = getWorkerServiceInfo() + " : notifyEventCompletion: Updated Patient ID(" +
             std::to_string(mCurrentPatient->mID) + ") service type to: " + std::to_string(mCurrentPatient->mServiceRequired);
            logInfo(msg);
        }
        else
        {
            std::string msg = getWorkerServiceInfo() + " : notifyEventCompletion: ERROR - Patient service type mismatch.";
            logInfo(msg);
        }
        mStats.addServiceTime(mScheduledServiceTime);

        mParent->notifyEventCompletion(eventId);
    }
    
    // Reset internal state related to the event.
    // NOTE: This is done AFTER sending the notification to the parent ServiceProvider.
    mCurrentEventID = 0;  // Reset the event ID to indicate no active event.
    mCurrentPatient = nullptr;  // Reset the current patient to indicate no patient is being served.
}

//! @brief Return common information about the worker service.
//! @return A string containing common information about the worker service.
std::string WorkerService::getWorkerServiceInfo() const
{
    std::string info = "WorkerService ID:" + std::to_string(mWorkerId);
    return info;
}

////////////////////////////////////////////////////////////////////////////////////////
// WorkerSupportedService Implementation
////////////////////////////////////////////////////////////////////////////////////////
WorkerSupportedService::WorkerSupportedService(HospitalSimApplication* app)
    : ServiceProvider(app)
{
    mServingQueue = std::make_unique<HospitalQueue>(app);
}

void WorkerSupportedService::initWorkerQueue(Patient::ServiceType serviceTypeServed, double minServiceTime,
         double maxServiceTime, int numWorkers, int maxQueueSize)
{
    std::string msg = mServiceName + " initialized.";
    logInfo(msg);

    mServiceTypeServed = int(serviceTypeServed);
    mMaxQueueSize = maxQueueSize;
    // Add all the workers and their processing times
    for (int i = 0; i < numWorkers; ++i) {
        mWorkerService.push_back(std::make_unique<WorkerService>(mApp, this, minServiceTime, maxServiceTime, serviceTypeServed));
    }
}

bool WorkerSupportedService::canProvideService()
{
    // Lets model a room capacity for both serving and waiting to be served queues.
    // NOTE: models a worker providing a service OR parking patient in a lobby queue.
    int totalInQueues = mServingQueue->getQueueSize();
    bool canProvide = (totalInQueues < mMaxQueueSize);
    std::string msg = mServiceName + " canProvideService: " + std::string(canProvide ? "true" : "false");
    msg += " Total in queues: " + std::to_string(totalInQueues) + " / " + std::to_string(mMaxQueueSize) + ": Serving Que:" + std::to_string(mServingQueue->getQueueSize());
    logInfo(msg);
    return canProvide;
}

void WorkerSupportedService::intakePatient(Patient* patient)
{
    // Implement the intake logic here
    patient->mIsIdle = true;  // Mark the patient as idle when they are added to the queue.
    mServingQueue->enqueue(patient);
    std::string msg = mServiceName + ": Patient enqueued into the waiting queue.";
    logInfo(msg);

    // Check if any worker service is available to take the patient from the waiting queue.
    performPatientCheckInProcess();
    performPatientCheckOutProcess();  // Check if any patients can be discharged to the recovery room after intake. 
}


//! @brief Display final statistics for the admin intake service.
void WorkerSupportedService::displayFinalStatistics(const int indentSpaces)
{
    std::string indent(indentSpaces, ' ');
    std::string msg;
    msg = indent + "Total patients queued     : " + std::to_string(mServingQueue->getQueueSize());
    logInfo(msg);
    msg = indent + "Total idle patients       : " + std::to_string(mServingQueue->getQueueSizeForIdleState(true));
    logInfo(msg);
    msg = indent + "Total non-served patients : " + std::to_string(mServingQueue->getQueueSizeForService(mServiceTypeServed));
    logInfo(msg);

    // Display worker service statistics
    for (const auto& worker : mWorkerService) {
        worker->displayFinalStatistics(indentSpaces + 2);
    }
}

//! @brief Notify event completion.
//! @param[in] eventId The ID of the event that has completed.
//! @note Supports one-shot or repeating events, but the implementation is left to the derived class.
void WorkerSupportedService::notifyEventCompletion(unsigned int eventID)
{
    // Iterate through the worker services to find the one that completed the event.
    for (auto& worker : mWorkerService) {
        // Check if the worker has completed the event with the given eventID.
        if (worker->mCurrentEventID == eventID) {
            worker->dischargePatient(worker->mCurrentPatient);
            break;  // Exit the loop once the worker is found and processed.
        }
    }

    std::string msg = mServiceName + ": Worker service completed event: ID(" + std::to_string(eventID) + ")";
    logInfo(msg);

    // After worker completion, check if we can remove or start any new worker services.
    performPatientCheckOutProcess();
    performPatientCheckInProcess();  // Check if any new patients can be assigned to workers after the event completion.
}

//! @brief Check and assign a worker to a patient from the waiting queue if available.
void WorkerSupportedService::performPatientCheckInProcess()
{
    // Quick exit on empty queue
    if (mServingQueue->isEmpty())
        return;

    // Find all free workers and assign patients that can be served by the worker service.
    // NOTE: We want to ensure the patient still holds a spot in the current serving queue,
    //       so we will re-enqueue the patient back into the serving queue after intake.
    for (auto& worker : mWorkerService) {
        if (worker->canProvideService()) {
            Patient* patient = mServingQueue->dequeueWaitingPatientForService(mServiceTypeServed, true);
            if (patient != nullptr) {
                std::string msg = mServiceName + ": Assigning patient to worker service.";
                logInfo(msg);   
                worker->intakePatient(patient);
                // NOTE: action to get patient removed it from the serving queue.
                mServingQueue->enqueue(patient);  // Re-enqueue the patient back into the serving queue after intake.
            }
        }
    }
}

//! @brief Move a patient from service queue into the appropriate next stage or discharge
void WorkerSupportedService::performPatientCheckOutProcess()
{
    // Extracted patients from serving queue
    std::queue<Patient*> tempQueue;

    // Process ALL patients that are idle and have completed the required service.
    while(true) {
        Patient* idlePatient = mServingQueue->dequeuePatientWithServiceCompleted(mServiceTypeServed, true);
        if (idlePatient == nullptr) {
            break;  // No more idle patients to process.
        }
        
        // Process the idle patient for discharge to the recovery room if possible.
        std::string msg = mServiceName + ": Checking out patient from serving queue, patient ptr: " + std::to_string(reinterpret_cast<std::uintptr_t>(idlePatient));
        logInfo(msg);
        if (attemptDischargeToNextStage(idlePatient)) {
            // Patient successfully discharged to recovery room.
            std::string msg = mServiceName + ": Patient ID(" + std::to_string(idlePatient->mID) + ") discharged to next stage.";
            logInfo(msg);
        } else {
            // Handle the case where the patient cannot be discharged to the next stage.
            std::string msg = mServiceName + ": Patient ID(" + std::to_string(idlePatient->mID) + ") can NOT be discharged to next stage.";
            logInfo(msg);
            tempQueue.push(idlePatient);  // Temporarily store the idle patient for processing.
        }
    }

    // Re-enqueue any patients that were temporarily stored in the tempQueue back into the serving queue.
    while (!tempQueue.empty()) {
        Patient* patient = tempQueue.front();
        tempQueue.pop();
        mServingQueue->enqueue(patient);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// RecoveryRoomService - Post-Operative Care Service
////////////////////////////////////////////////////////////////////////////////////////

RecoveryRoomService::RecoveryRoomService(HospitalSimApplication* app)
    : WorkerSupportedService(app)
{
    mServiceName = "RecoveryRoomService";

    // Add workers to the Recovery Room service.
    initWorkerQueue(Patient::ServiceType::eRecoveryRoom, cRecoveryRoomMinServiceTime,
         cRecoveryRoomMaxServiceTime, cRecoveryRoomNumberOfWorkers, cRecoveryRoomLobbySize);
}

void RecoveryRoomService::dischargePatient(Patient* patient)
{
    // Discharge patient from hospital simulation. 
    mApp->dischargePatient(patient);
}

void RecoveryRoomService::displayFinalStatistics(const int indentSpaces)
{
    // Implement the logic to display final statistics for the admin intake service here
    std::string indent(indentSpaces, ' ');
    std::string msg = indent + "Final statistics for RecoveryRoomService:";
    logInfo(msg);

    // Ask subclass to display its own statistics
    WorkerSupportedService::displayFinalStatistics(indentSpaces + 2);
}

//! @brief Attempt to discharge a patient into the next stage of care if possible, otherwise re-enqueue the patient into the serving queue.
//! @return True if the patient can be discharged into the next stage of care, false otherwise.
bool RecoveryRoomService::attemptDischargeToNextStage(Patient* patient)
{
    // Discharge this patient.
    dischargePatient(patient);
    return true;  // Assuming the patient can always be discharged from the recovery room.
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
// UrgentCareService - Immediate Medical Attention Service
////////////////////////////////////////////////////////////////////////////////////////

UrgentCareService::UrgentCareService(HospitalSimApplication* app)
    : WorkerSupportedService(app)
{
    mServiceName = "UrgentCareService";

    // Add workers to the Urgent Care service.
    initWorkerQueue(Patient::ServiceType::eUrgentCare, cUrgentCareMinServiceTime,
         cUrgentCareMaxServiceTime, cUrgentCareNumberOfWorkers, cUrgentCareLobbySize);
}

void UrgentCareService::dischargePatient(Patient* patient)
{
    // No action here - the discharge logic is handled in the attemptDischargeToNextStage method.
}

void UrgentCareService::displayFinalStatistics(const int indentSpaces)
{
    // Implement the logic to display final statistics for the admin intake service here
    std::string indent(indentSpaces, ' ');
    std::string msg = indent + "Final statistics for UrgentCareService:";
    logInfo(msg);

    // Ask subclass to display its own statistics
    WorkerSupportedService::displayFinalStatistics(indentSpaces + 2);
}

//! @brief Reevalute all patients discharge eligibility
void UrgentCareService::reevaluatePatientDischargeEligibility()
{
    performPatientCheckOutProcess();  // Reevaluate all patients in the serving queue for discharge eligibility.
    performPatientCheckInProcess();  // Reevaluate all patients in the serving queue for intake eligibility.
}
    

//! @brief Attempt to discharge a patient into the next stage of care if possible, otherwise re-enqueue the patient into the serving queue.
//! @return True if the patient can be discharged into the next stage of care, false otherwise.
bool UrgentCareService::attemptDischargeToNextStage(Patient* patient)
{
    // Perform a sanity check to ensure the patient is not null.
    if (patient == nullptr || patient->mActiveSubService)
    {
        return false;
    }

    // Does the patient need any sub-services (blood draw, x-ray, MRI, surgery, physical therapy) before discharge?
    if ((patient->mServiceRequired & int(Patient::ServiceType::eAllServices)) != 0)
    {
        // Is Blood Draw service required? If so, attempt to provide it if possible.
        if ((patient->mServiceRequired & int(Patient::ServiceType::eBloodDraw)) != 0)
        {
            if (mApp->performBloodDrawServiceIfPossible(patient))
            {
                std::string msg = mServiceName + ": Patient ID(" + std::to_string(patient->mID) + ") discharged to Blood Draw service.";
                logInfo(msg);
                return false;  // Patient still in Urgent Care, but removing sub-service task.
            }
        }

        std::string msg = mServiceName + ": Patient ID(" + std::to_string(patient->mID) + ") still requires additional services before discharge.";
        logInfo(msg);
        return false;  // Patient still requires additional services, cannot discharge to next stage.
    }

    // Discharge MIGHT be possible based on Recovery Room availability.
    // If not, the patient will be re-enqueued into the serving queue.
    return mApp->dischargeToRecoveryRoomIfPossible(patient);
}

////////////////////////////////////////////////////////////////////////////////////////
// BloodDrawService - Blood Draw Service
////////////////////////////////////////////////////////////////////////////////////////

BloodDrawService::BloodDrawService(HospitalSimApplication* app)
    : WorkerSupportedService(app)
{
    mServiceName = "BloodDrawService";

    // Add workers to the Blood Draw service.
    initWorkerQueue(Patient::ServiceType::eBloodDraw, cBloodDrawMinServiceTime,
         cBloodDrawMaxServiceTime, cBloodDrawNumberOfWorkers, cBloodDrawLobbySize);
}

//! @brief Intake a patient.
//! @param[in] patient The patient to intake.
void BloodDrawService::intakePatient(Patient* patient)
{
    // Mark as active sub-service for the patient.
    if (patient != nullptr)
    {
        patient->mActiveSubService = true;  // Mark the patient as undergoing a sub-service (blood draw).
    }   
    // Forward the intake to the base class to handle the queueing and worker assignment.
    WorkerSupportedService::intakePatient(patient);
}

void BloodDrawService::dischargePatient(Patient* patient)
{
    // Notify of completion.
    mApp->subServiceCompletionNotification(patient);  // Notify the application that the sub-service has been completed for the patient.
}

void BloodDrawService::displayFinalStatistics(const int indentSpaces)
{
    // Implement the logic to display final statistics for the blood draw service here
    std::string indent(indentSpaces, ' ');
    std::string msg = indent + "Final statistics for BloodDrawService:";
    logInfo(msg);

    // Ask subclass to display its own statistics
    WorkerSupportedService::displayFinalStatistics(indentSpaces + 2);
}
//! @brief Attempt to discharge a patient into the next stage of care if possible, otherwise re-enqueue the patient into the serving queue.
//! @return True if the patient can be discharged into the next stage of care, false otherwise.
bool BloodDrawService::attemptDischargeToNextStage(Patient* patient)
{
    // Always succeed on discharge to the next stage of care for blood draw service.
    // NOTE: The model does not own the patient's next stage of care.
    // It will only remove the required sub-service for that patient.
    if (patient != nullptr)
        patient->mActiveSubService = false;  // Mark the patient as no longer undergoing a sub-service.

    dischargePatient(patient);  // Notify of completion.
    
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////
// AdminIntakeService - Front Desk Administrative Intake Service
////////////////////////////////////////////////////////////////////////////////////////

AdminIntakeService::AdminIntakeService(HospitalSimApplication* app)
    : WorkerSupportedService(app)
{
    mServiceName = "AdminIntakeService";

    // Add workers to the Admin Intake service.
    initWorkerQueue(Patient::ServiceType::eAdminIntake, cAdminInTakeMinServiceTime,
         cAdminInTakeMaxServiceTime, cAdminInTakeNumberOfWorkers, cAdminInTakeLobbySize);
}

void AdminIntakeService::dischargePatient(Patient* patient)
{
    // Implement the discharge logic here
}

void AdminIntakeService::displayFinalStatistics(const int indentSpaces)
{
    // Implement the logic to display final statistics for the admin intake service here
    std::string indent(indentSpaces, ' ');
    std::string msg = indent + "Final statistics for AdminIntakeService:";
    logInfo(msg);

    // Ask subclass to display its own statistics
    WorkerSupportedService::displayFinalStatistics(indentSpaces + 2);
}
//! @brief Attempt to discharge a patient into the next stage of care if possible, otherwise re-enqueue the patient into the serving queue.
//! @return True if the patient can be discharged into the next stage of care, false otherwise.
bool AdminIntakeService::attemptDischargeToNextStage(Patient* patient)
{
    // TODO - Determine if patient should go to Emergency room.

    // Attempt to discharge the patient into the urgent care if possible.
    // If not, the patient will be re-enqueued into the serving queue.
    return mApp->dischargeToUrgentCareIfPossible(patient);
}

