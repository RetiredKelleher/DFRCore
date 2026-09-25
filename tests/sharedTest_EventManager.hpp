// @file sharedTest_EventManager.hpp
// @brief Shared unit test declarations for DFR::EventManager (priority-queue implementation).  
// @note These tests are intended to be included in multiple test files.   

#ifdef GLOBALVARS
#define EXTERN
#else
#define EXTERN extern
#endif
namespace DFR {
/// @brief Global failure counter for shared EventManager tests.
EXTERN int gFailures;
/// @brief Global name of the EventManager under test.
EXTERN std::string gEventManagerName;
} // namespace DFR

/// @brief Run all shared EventManager tests.
void runAllTests();

void testGetNextEventAtOrBefore();
// Shared EventManager test declarations
// Test basic EventManager functionality (peekNextEvent and getNextEvent)
void testEmptyManager();
void testEventCountersAreUniqueAndAssigned();
void testOrderingBySimTime();
void testOrderingByPriorityAtSameTime();
void testFifoTieBreakAtSameTimeAndPriority();
void testPeekDoesNotRemove();
void testClearEvents();
void testRemoveEvent();
void testRemoveEventPreservesDerivedEvents();


// Test getNextEventAtOrBefore usage
void testGetNextEventAtOrBeforeEmptyQueue();
void testGetNextEventAtOrBeforeReturnsEarlierEvent();
void testGetNextEventAtOrBeforeHoldsBackLaterEvent();
void testGetNextEventAtOrBeforeExcludesBoundary();
void testGetNextEventAtOrBeforeDrainsInOrder();
