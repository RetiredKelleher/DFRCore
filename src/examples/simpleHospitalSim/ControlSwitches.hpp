////////////////////////////////////////////////////////////////////////////////////////
//! @file ControlSwitches.hpp
//! @brief Header file for control switches in the simple hospital simulation.
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// Switches for controlling the overall hospital simulation
static const double cSimulationDuration = 100;  //!< Duration (seconds) of the overall hospital simulation.
static const unsigned int cSimulationRandomSeed = 42;  //!< Random seed for the overall hospital simulation.
static const double cPatientInTakePollingInterval = 2.0;  //!< Time interval (seconds) for polling the patient intake process.
static const int cMaxPatientsInjected = 1;  //!< Maximum number of patients that can be injected into the hospital simulation.

// Switches for controlling the patient IN-TAKE process
static const int cAdminInTakeLobbySize = 10;  //!< Limits the number of patients in the admin intake lobby.
static const int cAdminInTakeNumberOfWorkers = 3;  //!< Number of workers in the admin intake process.
static const double cAdminInTakeMinServiceTime = 1.0;  //!< Minimum service time (seconds) for the admin intake process.
static const double cAdminInTakeMaxServiceTime = 5.0;  //!< Maximum service time (seconds) for the admin intake process.

// Switches for controlling the urgent care service
static const int cUrgentCareLobbySize = 5;  //!< Limits the number of patients in the urgent care lobby.
static const int cUrgentCareNumberOfWorkers = 3;  //!< Number of workers in the urgent care service.
static const double cUrgentCareMinServiceTime = 1.5;  //!< Minimum service time (seconds) for the urgent care service.
static const double cUrgentCareMaxServiceTime = 4.0;  //!< Maximum service time (seconds) for the urgent care service.

// Switches for controlling the post-operative recovery room service
static const int cRecoveryRoomLobbySize = 5;  //!< Limits the number of patients in the recovery room lobby.
static const int cRecoveryRoomNumberOfWorkers = 2;  //!< Number of workers in the recovery room service.
static const double cRecoveryRoomMinServiceTime = 9.0;  //!< Minimum service time (seconds) for the recovery room service.
static const double cRecoveryRoomMaxServiceTime = 15.0;  //!< Maximum service time (seconds) for the recovery room service.

////////////////////////////////////////////////////////////////////////////////////////
// Sub-Services for the hospital simulation
////////////////////////////////////////////////////////////////////////////////////////

// Control the type of sub-services required by each patient.
static double cBloodDrawRandomChance = 100.0;  //!< Percentage chance that a patient will require the blood draw sub-service.

// Switches for controlling the blood draw service
static const int cBloodDrawLobbySize = 5;  //!< Limits the number of patients in the blood draw lobby.
static const int cBloodDrawNumberOfWorkers = 2;  //!< Number of workers in the blood draw service.
static const double cBloodDrawMinServiceTime = 2.0;  //!< Minimum service time (seconds) for the blood draw service.
static const double cBloodDrawMaxServiceTime = 4.0;  //!< Maximum service time (seconds) for the blood draw service.
