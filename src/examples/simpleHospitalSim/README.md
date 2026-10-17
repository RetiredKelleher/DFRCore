# Hospital Simulation Example

Simple Hospital queuing simulation using the DFRCore package.  The simulation
will model a random patient intake, check-in times, and service times of each patient.
The intent of the model is NOT to perform a realistic model of a given hospital, but
to use the DFRCore capabilites (V0.0.1) and ensure it can support a simple queing
model.

## DFRCore Usage

The package has limited capabilites, at this point, but are useful enough to
support a simple use-case. The capabilities used are:

    1. Overload of the application object
    2. Use of events, both one-time and reoccuring.
    3. Events created at both the main application level and with-in objects.
    4. Use of the logging method.
    5. Use of the Random Number Generator
    6. Simulation engine setup and execution

The main application, HospitalSimApplication, creates both a one-time and reocurring events.
The one-time event manages the stop-time, informing the simulation to stop execution.
While the reocurring event is used as a polling technique to determine if a patient
wants to be injected into the simulation.

NOTE: This was NOT intended to be the best way to model a hospital, but to
demonistrate how to package can be used.

## Hospital Model

This vesion of the hospital model is very simple.  It is intended to show how
certain delays can impact the overall ability to processing incoming patients.
The model consists of three basic steps:

1. Random times a patient will seek treatment and will attempt to be injected into the model.
1.1. The patient CAN be rejected if the hospital lobby is already filled with other patients.
2. The hospital check-in process consists of two wait queues.
2.1. The first queue is to be simply checked into the hospital.
2.2. Admin In-take workers will process these queues AND take some random time to process.
2.3. After they check-in, the patient will stay in the lobby until they can be served.
3. The treatment center will take a check-in patient and treat them.
3.1. Workers will treat each patient, taking random times to treat.
3.2. After treatment the patient will be discharged.

Key features of this model: All queues are FIFO (first-in first-out). A treatment level
could be modeled by modifying the HospitalQueue object and dequeuing the most
urgent patient.

## Hospital Design

The following objects were used in the design.  Again, its a simple model so be nice please:)

Base Support Objects:
- HospitialQueue - Simple FIFO que.  If you wanted to add treatment levels, could be done here.
- WorkerSupportService - Models both a lobby (seating area) and workers to process that group.

Models that are a little more interesting:
- WorkerService - Generic worker (doctor, admin, etc) use to service patients.
- AdminInTakeService - Represents the in-take lobby, has que for pre/post check-in before any service is provided.
- UrgentCareService - Represents some number of beds to serve the checked in patients
- RecoveryRoom Service - Out processing service that has workers to meter the patients.

Other key items:
- The required treatment is stored as a bit-mask of required actions.  Built to support the idea
of having supporting sub-departments (ie. BloodDraw, XRay, CT, etc) that could be part of
required treatment plan for each patient.
- All control parameters used to constrain the system are in ControlSwitches.hpp
