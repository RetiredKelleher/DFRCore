///////////////////////////////////////////////////////////////////////////////
//! @file ICommPoller.hpp
//! @brief Interface class definition, responsible for managing communication channel polling.
//! @author Thomas Kelleher
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace DFR {

//! @brief Interface for communication channel pollers.
//! Implement this interface and register with DFRSimEngine::registerPoller() to
//! receive a callback on every simulation loop iteration after events have been
//! processed. This is the intended hook for reading inbound messages, checking
//! socket buffers, or any other periodic I/O that must be serviced regardless of
//! whether a scheduled event fired during that tick.
//! @note Design assumes a single threaded simulation loop.
class ICommPoller
{
public:
    virtual ~ICommPoller() = default;

    //! @brief Called once per sim loop iteration after events are processed.
    //! @param[in] currentSimTime  Simulation time at the moment of polling.
    virtual void poll(double currentSimTime) = 0;
};

} // namespace DFR