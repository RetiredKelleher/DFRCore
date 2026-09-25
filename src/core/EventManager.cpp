/////////////////////////////////////////////////////////////////////////////////
// @file    EventManager.cpp
// @brief DRF::EventManager ... TODO
// @author  Thomas Kelleher
/////////////////////////////////////////////////////////////////////////////////

#include <DFRCore/EventManager.hpp>
#include "PriorityQueueEventManager.hpp"
#include <iostream>

//#define USEDOUT 1
#if USEDOUT
//! @brief Internal macro for "Debug Output" that can be compiled out of use. Sends string to standard out.
#define DOUT(_a) { std::cout << _a << std::endl; }
#else
//! @brief Internal macro for "Debug Output" that can be compiled out of use. Sends string to standard out.
#define DOUT(_a) {}
#endif

namespace DFR {

EventManager::EventManager()
{
    DOUT("EventManager was created!");
}

/////////////////////////////////////////////////////////////////////////////////
// Static Method Supports
/////////////////////////////////////////////////////////////////////////////////

//! @brief Creates a priority-based event manager instance.
//! @return A unique pointer to the created priority-based event manager.
std::unique_ptr<EventManager> EventManager::createPriorityBasedEventManager()
{
    // Implement the creation of a priority-based event manager
    return std::make_unique<PriorityQueueEventManager>();
}
} // namespace DFR
