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
The model consists of four basic steps:

1. Random times a patient will seek treatment and will attempt to be injected into the model.
1.1. The patient CAN be rejected if the hospital lobby is already filled with other patients.
2. The hospital check-in process.
2.1. The queue is to be simply checked into the hospital.
2.2. Admin In-take workers will process the queues AND take some random time to process.
2.3. After they check-in, the patient will stay in the lobby until they can be served.
3. The treatment center will take a check-in patient and treat them.
3.1. Workers will treat each patient, taking random times to treat.
3.2. Treat ALL sub-services items (ie. BloodDraw, XRay, etc)
3.3. After treatment the patient will be discharged.
4. The recovery room will perform the final treatment on the patients.
4.1. The patient will occupy a bed.
4.2. A worker will tend to the patient.
4.3. After tending, the patient will be removed from the hospital freeing the bed.

Key features of this model: All queues are FIFO (first-in first-out). A treatment level
could be modeled by modifying the HospitalQueue object and dequeuing the most
urgent patient.

## Hospital Design

The following objects were used in the design.  Again, its a simple model so be nice please:)

Base Support Objects:

- HospitialQueue - Simple FIFO que.  If you wanted to add treatment levels, could be done here.
- ServiceProvider - Abstract class for the basic service concept and processing.
- WorkerSupportService - Models both a lobby (seating area) and workers to process that group.

Models that are a little more interesting:

- WorkerService - Generic worker (doctor, admin, etc) use to service patients.
- AdminInTakeService - Represents the in-take lobby to hold both pre/post checked in patients.
- UrgentCareService - Represents some number of beds to serve the checked in patients
- RecoveryRoom Service - Out processing service that has workers to meter the patients.

Additional sub-service models that represent atomic processing and delays:

- BloodDrawSerivce - Represents a room that has workers drawing blood.

Other key items:

- The required treatment is stored as a bit-mask of required actions.  Built to support the idea
of having supporting sub-departments (ie. BloodDraw, XRay, CT, etc) that could be part of
required treatment plan for each patient.
- All control parameters used to constrain the system are in ControlSwitches.hpp
- Patient can be stalled due to follow-on services being unable to accept a new patient.
- Stalled services will block up-stream services and that is the point of the model.

Concept of flow looks like the following:

1.0 Admin InTake - seating area and number of admin are constraints
2.0 Urgent Care - beds and workers are constraints (Might require Sub-Services)
3.0 Sub-Services - Optional constraints after Urgent Care, add to the processing time
3.1   Blood Draw Sub-Service - seating area and workers are constraints
3.2   NOTE: Can add additional sub-services here, as some are proposed in Patient bitmask needs.
4.0 RecoverRoomService - After both Urgent Care and Sub-Services, beds and workers contrainted
5.0 Main App - dispatch Patient called after done with recovery room.
