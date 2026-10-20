////////////////////////////////////////////////////////////////////////////////////////
//! @file DataTypes.hpp
//! @brief Defines the data types used in the simple hospital simulation.
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

//! @brief Simple Stats collection struct
struct Stats
{
    int mServiceProcessed;  //!< The number of time a service was processed.
    double mTotalServiceTime;  //!< The total service time for all patients.

    Stats() : mServiceProcessed(0), mTotalServiceTime(0.0) {}  //!< Default constructor initializing the stats.

    void addServiceTime(double serviceTime)  //!< Add a service time to the stats.
    {
        mServiceProcessed++;
        mTotalServiceTime += serviceTime;
    }
};

//! @brief Represents a patient in the hospital simulation.
struct Patient
{
    // Define the types of service required by the patient.
    // NOTE: Uses bit-wise flags to allow for multiple service types to be combined.
    enum class ServiceType
    {
        eComplete        = 0x0000, //!< Complete service (default).
        eAdminIntake     = 0x0001, //!< Administrative intake service (mandatory).
        eRecoveryRoom    = 0x0002, //!< Post-operative recovery room service (mandatory).
        eUrgentCare      = 0x0004, //!< Urgent care service.
        eEmergencyRoom   = 0x0008, //!< FUTURE: Emergency room service.

        eBloodDraw       = 0x0010, //!< Blood test service.
        eXRay            = 0x0020, //!< FUTURE: X-ray service.
        eMRI             = 0x0040, //!< FUTURE: MRI service.
        eSurgery         = 0x0080, //!< FUTURE: Surgery service.
        ePhysicalTherapy = 0x0100, //!< FUTURE: Physical therapy service.
        eAllServices   = eBloodDraw | eXRay | eMRI | eSurgery | ePhysicalTherapy  //!< don't include checkin or recovery services combined.
    };
    Patient() : mID(0), mIsIdle(true) {}  //!< Default constructor initializing the patient as idle.

    int mID;       //!< The unique identifier for the patient.
    bool mIsIdle;  //!< Indicates whether the patient is currently idle.
    bool mActiveSubService{false};  //!< Indicates whether the patient is currently undergoing a sub-service.
    int mServiceRequired{int(ServiceType::eComplete)};  //!< The type of service required by the patient.
};
