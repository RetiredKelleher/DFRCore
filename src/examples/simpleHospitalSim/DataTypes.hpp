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
        eComplete      = 0x00, //!< Complete service (default).
        eAdminIntake   = 0x01, //!< Administrative intake service.
        eRecoveryRoom  = 0x02, //!< Post-operative recovery room service.
        eUrgentCare    = 0x04, //!< Urgent care service.
        eAllServices   = eUrgentCare  //!< don't include checkin or recovery services combined.
    };
    Patient() : mID(0), mIsIdle(true) {}  //!< Default constructor initializing the patient as idle.

    int mID;       //!< The unique identifier for the patient.
    bool mIsIdle;  //!< Indicates whether the patient is currently idle.
    int mServiceRequired{int(ServiceType::eComplete)};  //!< The type of service required by the patient.
};
