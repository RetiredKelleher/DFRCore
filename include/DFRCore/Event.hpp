///////////////////////////////////////////////////////////////////////////////
//! @file Event.hpp
//! @author  Thomas M Kelleher
//! @date    2024-06-01
//! @brief   Defines the DFR::Event base class and related template event types for the simulationframework. 
//! The DFR::Event class serves as a base class for specific event types like
//! DFR::OneShotEvent and DFR::RepeatingEvent, which can implement their own execution
//! logic while leveraging the common properties and scheduling capabilities of the
//! DFR::Event class. 
//! @note The EventManager class (not defined here) is responsible for managing the scheduling and execution of these events in the simulation.
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>

namespace DFR {

class EventManager; // Forward declaration of EventManager to allow it to be a friend class of Event

//! @brief The DFR::Event class represents a scheduled event in the DFRCore framework.
//! It contains properties such as simulation time, priority, and execution status,
//! and provides a virtual Execute() method that can be overridden by derived classes
//! to define specific event behaviors.
//! The DFR::Event class serves as a base class for specific event types like
//! DFR::OneShotEvent and DFR::RepeatingEvent, which can implement their own execution
//! logic while leveraging the common properties and scheduling capabilities of the
//! DFR::Event class.
class Event
{
    friend class EventManager; // Allow EventManager to access protected members of Event
public:

    enum class EventStatus
    {
        eDelete,      //!< Indicates that the event should be deleted after execution.
        eReschedule   //!< Indicates that the event should be rescheduled for future execution.
    };

    //! @brief Constructs a DFR::Event instance with the specified simulation time and priority. The event is initialized with a default event counter of 0 and an execution flag set to true.
    //! @param[in] simtime The simulation time at which the event is scheduled to execute. Default is 0.0.
    //! @param[in] priority The priority of the event, where lower values indicate higher priority (Inverted Score). Default is 16.
    Event(double simtime = 0.0, int priority = 16) :
        mEventCounter(0),
        mSimTime(simtime),
        mPriority(priority),
        mExecuteFlag(true)
    {
    }

    //! @brief Default destructor for the Event class.
    virtual ~Event() = default;

    //! @brief Get the simulation time of the event.
    //! @return The simulation time at which the event is scheduled to execute.
    double simTimeOfEvent() const { return mSimTime; }

    //! @brief Get the priority of the event. Events use an "Inverted Score" to determine execution order. Lower values are executed first, while higher values are executed later. This allows for a flexible scheduling system where events can be prioritized based on their importance or urgency.
    //! @return The priority level of this event.
    int priority() const { return mPriority; }

    //! @brief Check if the event is flagged for execution. This flag can be used to determine whether the event should be executed or skipped during the simulation run.
    //! @return True if the event is flagged for execution, false otherwise.
    bool executeFlag() const { return mExecuteFlag; }

    //! @brief Execute the event's action. This method should be overridden by derived classes to define the specific behavior of the event when it is executed. The default implementation returns EventStatus::eDelete, indicating that the event should be deleted after execution.
    //! @return The status of the event after execution, indicating whether it should be deleted or
    virtual EventStatus Execute() { return EventStatus::eDelete;};

    //! @brief Update the simulation time for this event. This method can be used by the EventManager to update the event's scheduled time when rescheduling is needed.
    //! @param[in] newSimTime The new simulation time to be assigned to the event.
    //! @note This does not automatically requeue the event in the EventManager; it only updates the internal simulation time of the event.
    void updateSimTime(double newSimTime) { mSimTime = newSimTime; }
    
    //! @brief Update the Priority of the event. This method can be used by the EventManager or other components to change the priority of the event when rescheduling is needed.
    //! @param[in] newPriority The new priority level to be assigned to the event.
    //! @note This does not automatically requeue the event in the EventManager; it only updates the internal priority of the event.
    void setPriority(int newPriority) { mPriority = newPriority; }
    //! @brief Set the EventID for this event.
    //! @param[in] eventID The unique identifier to be assigned to this event. This can be used for additional identification or categorization of events, separate from the unique event counter assigned by the EventManager. The EventID is a unique ID that can be used to identify or categorize events in the simulation, and is separate from the eventCounter which is a unique identifier assigned by the EventManager for scheduling purposes. The EventID can be set by the user or by the EventManager when the event is created, and can be used for tracking, diagnostics, or other purposes within the simulation.
    void setEventID(unsigned int eventID) { mEventID = eventID; }
    //! @brief Get the EventID for this event.
    //! @return The unique EventID assigned to this event, which can be used for identification
    unsigned int getEventID() const { return mEventID; }

    //! @brief Get the eventCounter for this event.
    //! The value is used to break ties between events with the same simTime and priority, ensuring a consistent order of execution for events that are scheduled at the same time with the same priority. The eventCounter is a unique identifier for each event in the simulation, assigned by the EventManager when the event is added to the event queue.
    //! @note This is exposed to support diagnostics and test purposes, but is not intended to be used for event management or scheduling logic outside of the EventManager. The eventCounter is primarily used internally by the EventManager to maintain the correct order of events in the priority queue.
    //! @return The unique event counter for this event.
    unsigned int eventCounter() const { return mEventCounter; }

    //! @brief Set the eventCounter for this event. 
    //! The eventCounter is a unique tag assigned to each scheduled event by the EventManager
    //! when the event is added to the event queue. It is used to break ties between events with the same simTime and priority, ensuring a consistent order of execution for events that are scheduled at the same time with the same priority. The eventCounter is incremented for each new event added to the EventManager, providing a unique identifier for each event in the simulation.
    //! @param[in] counter The unique event counter to be assigned to this event. This value is typically assigned by the EventManager when the event is added to the event queue.
    void setEventCounter(unsigned int counter) { mEventCounter = counter; }

 protected:

    unsigned int mEventCounter{0}; //!< Unique identifier for each event, set by the EventManager when the event is added to the event queue
    unsigned int mEventID{0}; //!< Optional event ID that can be used for additional identification or categorization of events, separate from the unique event counter assigned by the EventManager

    double mSimTime{0.0};  //!< The simulation time at which the event is scheduled to execute
    int    mPriority{0}; //!< Priority of the event, where higher values indicate lower priorities.

    bool   mExecuteFlag{true}; //!< Flag indicating whether the event should be executed.
};

//////////////////////////////////////////////////////////////////////////////////////
//! @brief Template class for DFR::Event that allows for flexible execution functions with varying return types and argument lists.
//! This class serves as a base for specific event types like DFR::OneShotEvent and DFR::RepeatingEvent,
//! which can define their own execution logic while leveraging the common properties
//! and scheduling capabilities of the DFR::Event class.
//! The template parameters RT and Args allow for defining the return type and argument
//! types of the execution function, enabling a wide range of event behaviors to be
//! implemented using this base class. The DFR::EventT class is designed to be inherited
//! by specific event types that will implement the Execute() method according to their
//! specific behavior, while still utilizing the common scheduling and management
//! features provided by the DFR::Event base class. 
//////////////////////////////////////////////////////////////////////////////////////
template <typename RT, typename... Args>
class EventT : public Event
{
protected:
    using ExeFN = std::function<RT(Args...)>;

    //! @param[in] simtime The simulation time at which the event is scheduled to execute. Default is 0.0.
    //! @param[in] priority The priority of the event, where lower values indicate higher priority (Inverted Score).
    //! @param[in] exeFn A reference to a function that defines the execution logic for this event. The function should match the signature defined by the template parameters RT and Args.
    EventT(double simtime,int priority, const ExeFN& exeFn) :
        Event(simtime, priority),
        mExeFn(exeFn)
        {}

        const ExeFN mExeFn;
};


//////////////////////////////////////////////////////////////////////////////
//! @brief A one-shot event that executes a provided function once and then is deleted from the event queue.
//! The DFR::OneShotEvent class inherits from DFR::EventT with a void return type
//! and takes a reference to a DFR::Event as an argument for the execution function.
//! When the Execute method is called, it executes the provided function and
//! then returns EventStatus::eDelete, indicating that the event should be
//! removed from the event queue after execution. This class is useful for 
//! scheduling events that need to occur once at a specific simulation time,
//! such as a one-time action or trigger in the simulation.
///////////////////////////////////////////////////////////////////////////////
class OneShotEvent : public EventT<void, Event&>
{
public:
    //! @param[in] simtime The simulation time at which the event is scheduled to execute. Default is 0.0.
    //! @param[in] priority The priority of the event, where lower values indicate higher priority (Inverted Score).
    //! @param[in] exeFn A reference to a function that defines the execution logic for this event. The function should match the signature defined by the template parameters RT and Args.
    OneShotEvent(double simtime, int priority, const ExeFN& exeFn) :
        EventT<void, Event&>(simtime, priority, exeFn)
        {}

    //! @brief Executes the event's action by calling the provided execution function and then returns EventStatus::eDelete, indicating that the event should be removed from the event queue after execution.
    //! @return EventStatus::eDelete, indicating that the event should be deleted after execution
    EventStatus Execute() override
    {
        mExeFn(*this);
        return EventStatus::eDelete;
    }
};

//////////////////////////////////////////////////////////////////////////////
//! @brief A repeating event that executes a provided function and can be rescheduled for future execution based on the return value of the execution function.
//! The DFR::RepeatingEvent class inherits from DFR::EventT with an EventStatus return type and takes a reference to a DFR::Event as an argument for the execution function. When the Execute method is called, it executes the provided function and returns the
//! EventStatus returned by the execution function, which can indicate whether the event should be deleted or rescheduled for future execution. This class is useful for scheduling events that need to occur repeatedly at specific simulation times, such as periodic actions or triggers in the simulation.
///////////////////////////////////////////////////////////////////////////////

class RepeatingEvent : public EventT<Event::EventStatus, Event&>
{
public:
    //! @param[in] simtime The simulation time at which the event is scheduled to execute. Default is 0.0.
    //! @param[in] priority The priority of the event, where lower values indicate higher priority (Inverted Score).
    //! @param[in] exeFn A reference to a function that defines the execution logic for this event. The function should match the signature defined by the template parameters RT and Args.
    RepeatingEvent(double simtime, int priority, const ExeFN& exeFn) :
        EventT<Event::EventStatus, Event&>(simtime, priority, exeFn)
        {}

    //! @brief Executes the event's action by calling the provided execution function and returns the EventStatus indicating whether the event should be deleted or rescheduled for future execution.
    //! @return The EventStatus returned by the execution function, which can be either EventStatus
    EventStatus Execute() override
    {
        return mExeFn(*this);
    }
};

} // namespace DFR
