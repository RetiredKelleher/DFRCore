/////////////////////////////////////////////////////////////////////////////////
//! @file testEventManager.cpp
//! @author Thomas Kelleher
//! @date 2026-09-24
//! @brief Unit tests for DFR::EventManager (priority-queue implementation).
/////////////////////////////////////////////////////////////////////////////////

#include <DFRCore/EventManager.hpp>
#include <DFRCore/Event.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "sharedTest_EventManager.hpp"

std::unique_ptr<DFR::EventManager> makeManager()
{
    return DFR::EventManager::createPriorityBasedEventManager();
}

/////////////////////////////////////////////////////////////////////////////////
// Include shared EventManager tests
/////////////////////////////////////////////////////////////////////////////////
//#include "sharedTest_EventManager.cpp"

int main()
{
    // Set the name of the EventManager under test
    DFR::gEventManagerName = "Priority-Based EventManager";
    
    // Run all the tests
    runAllTests();
#if 0
    testEmptyManager();
    testEventCountersAreUniqueAndAssigned();
    testOrderingBySimTime();
    testOrderingByPriorityAtSameTime();
    testFifoTieBreakAtSameTimeAndPriority();
    testPeekDoesNotRemove();
    testClearEvents();
    testRemoveEvent();
    testRemoveEventPreservesDerivedEvents();
    testGetNextEventAtOrBeforeEmptyQueue();
    testGetNextEventAtOrBeforeReturnsEarlierEvent();
    testGetNextEventAtOrBeforeHoldsBackLaterEvent();
    testGetNextEventAtOrBeforeExcludesBoundary();
    testGetNextEventAtOrBeforeDrainsInOrder();
#endif

    // Determine if any checks failed
    if (DFR::gFailures != 0)
    {
        std::cerr << DFR::gFailures << " check(s) failed" << std::endl;
        return 1;
    }
    std::cout << DFR::gEventManagerName << ": All EventManager tests passed" << std::endl;
    return 0;
}
