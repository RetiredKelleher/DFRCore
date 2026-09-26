////////////////////////////////////////////////////////////////////////////////////////
// @file sharedTest_EventManager.cpp
// @brief Shared unit tests for DFR::EventManager (priority-queue implementation).  
// @note These tests are intended to be included in multiple test files.
////////////////////////////////////////////////////////////////////////////////////////

#include <DFRCore/EventManager.hpp>
#include <DFRCore/Event.hpp>
#include <iostream>
#include <memory>
#include <vector>

#define GLOBALVARS 1
#include "sharedTest_EventManager.hpp"

extern std::unique_ptr<DFR::EventManager> makeManager();

// assert() is compiled out under NDEBUG (Release builds), so use a check that always runs.
#define CHECK(cond)                                                              \
    do {                                                                         \
        if (!(cond)) {                                                           \
            std::cerr << __FILE__ << ":" << __LINE__ << ": CHECK failed: " #cond \
                      << std::endl;                                              \
            ++DFR::gFailures;                                                         \
        }                                                                        \
    } while (0)
// End of shared test utilities

void runAllTests()
{
    testEmptyManager();
    testEventCountersAreUniqueAndAssigned();
    testOrderingBySimTime();
    testOrderingByPriorityAtSameTime();
    testFifoTieBreakAtSameTimeAndPriority();
    testPeekDoesNotRemove();
    testClearEvents();
    testRemoveEvent();
    testRemoveEventPreservesDerivedEvents();
    // test requeueEvent functionality would go here if implemented
    testRequeueNullEvent();
    testRequeueOrderingBySimTime();
    testRequeueOrderingByPriorityAtSameTime();
    testRequeueFifoTieBreakAtSameTimeAndPriority();

    // test GetNextEventAtOrBefore functionality
    testGetNextEventAtOrBeforeEmptyQueue();
    testGetNextEventAtOrBeforeReturnsEarlierEvent();
    testGetNextEventAtOrBeforeHoldsBackLaterEvent();
    testGetNextEventAtOrBeforeExcludesBoundary();
    testGetNextEventAtOrBeforeDrainsInOrder();
}
void testEmptyManager()
{
    std::cout << DFR::gEventManagerName << ": Test Empty Manager" << std::endl;
    auto em = makeManager();
    CHECK(em != nullptr);
    CHECK(em->peekNextEvent() == nullptr);
    CHECK(em->getNextEvent() == nullptr);
    CHECK(em->removeEvent(1) == false);
}

void testEventCountersAreUniqueAndAssigned()
{
    std::cout << DFR::gEventManagerName << ": Test Event Counters Are Unique And Assigned" << std::endl;
    auto em = makeManager();
    unsigned int c1 = em->addEvent(std::make_unique<DFR::Event>(1.0));
    unsigned int c2 = em->addEvent(std::make_unique<DFR::Event>(1.0)); 
    CHECK(c1 != 0);
    CHECK(c2 == c1 + 1);

    auto e = em->getNextEvent();
    CHECK(e != nullptr);
    CHECK(e->eventCounter() == c1);
}

void testOrderingBySimTime()
{
    std::cout << DFR::gEventManagerName << ": Test Ordering By Sim Time" << std::endl;
    auto em = makeManager();
    em->addEvent(std::make_unique<DFR::Event>(3.0));
    em->addEvent(std::make_unique<DFR::Event>(1.0));
    em->addEvent(std::make_unique<DFR::Event>(2.0));

    std::vector<double> times;
    while (auto e = em->getNextEvent())
    {
        times.push_back(e->simTimeOfEvent());
    }
    CHECK((times == std::vector<double>{1.0, 2.0, 3.0}));
}

void testOrderingByPriorityAtSameTime()
{
    // Lower priority value executes first ("inverted score", see Event.hpp).
    std::cout << DFR::gEventManagerName << ": Test Ordering By Priority At Same Time" << std::endl;
    auto em = makeManager();
    em->addEvent(std::make_unique<DFR::Event>(5.0, 20));
    em->addEvent(std::make_unique<DFR::Event>(5.0, 1));
    em->addEvent(std::make_unique<DFR::Event>(5.0, 10));

    std::vector<int> priorities;
    while (auto e = em->getNextEvent())
    {
        priorities.push_back(e->priority());
    }
    CHECK((priorities == std::vector<int>{1, 10, 20}));
}

void testFifoTieBreakAtSameTimeAndPriority()
{
    std::cout << DFR::gEventManagerName << ": Test FIFO Tie Break At Same Time And Priority" << std::endl;
    auto em = makeManager();
    unsigned int a = em->addEvent(std::make_unique<DFR::Event>(5.0, 16));
    unsigned int b = em->addEvent(std::make_unique<DFR::Event>(5.0, 16));
    unsigned int c = em->addEvent(std::make_unique<DFR::Event>(5.0, 16));

    CHECK(em->getNextEvent()->eventCounter() == a);
    CHECK(em->getNextEvent()->eventCounter() == b);
    CHECK(em->getNextEvent()->eventCounter() == c);
}

void testPeekDoesNotRemove()
{
    std::cout << DFR::gEventManagerName << ": Test Peek Does Not Remove" << std::endl;
    auto em = makeManager();
    em->addEvent(std::make_unique<DFR::Event>(1.0));

    const DFR::Event* peeked = em->peekNextEvent();
    CHECK(peeked != nullptr);
    CHECK(em->peekNextEvent() == peeked);

    auto got = em->getNextEvent();
    CHECK(got.get() == peeked);
    CHECK(em->peekNextEvent() == nullptr);
}

void testClearEvents()
{
    std::cout << DFR::gEventManagerName << ": Test Clear Events" << std::endl;
    auto em = makeManager();
    em->addEvent(std::make_unique<DFR::Event>(1.0));
    em->addEvent(std::make_unique<DFR::Event>(2.0));
    em->clearEvents();
    CHECK(em->peekNextEvent() == nullptr);
    CHECK(em->getNextEvent() == nullptr);
}

void testRemoveEvent()
{
    std::cout << DFR::gEventManagerName << ": Test Remove Event" << std::endl;
    auto em = makeManager();
    unsigned int c1 = em->addEvent(std::make_unique<DFR::Event>(1.0));
    unsigned int c2 = em->addEvent(std::make_unique<DFR::Event>(2.0));
    unsigned int c3 = em->addEvent(std::make_unique<DFR::Event>(3.0));

    CHECK(em->removeEvent(c2) == true);
    CHECK(em->removeEvent(c2) == false); // already gone
    CHECK(em->removeEvent(9999) == false); // never existed

    // Remaining events keep their order and counters.
    CHECK(em->getNextEvent()->getEventID() == c1);
    CHECK(em->getNextEvent()->getEventID() == c3);
    CHECK(em->getNextEvent() == nullptr);
}

void testRemoveEventPreservesDerivedEvents()
{
    std::cout << DFR::gEventManagerName << ": Test Remove Event Preserves Derived Events" << std::endl;
    // Events that stay in the queue after removeEvent() must keep their
    // derived type, so their Execute() still runs the user's function.
    auto em = makeManager();
    int calls = 0;
    std::function<void(DFR::Event&)> fn = [&calls](DFR::Event&) { ++calls; };

    em->addEvent(std::make_unique<DFR::OneShotEvent>(1.0, 16, fn));
    unsigned int toRemove = em->addEvent(std::make_unique<DFR::OneShotEvent>(2.0, 16, fn));

    CHECK(em->removeEvent(toRemove) == true);

    auto e = em->getNextEvent();
    CHECK(e != nullptr);
    if (e)
    {
        e->Execute();
    }
    CHECK(calls == 1);
}

////////////////////////////////////////////////////////////////////////////////
// Test requeue functionality of the EventManager
////////////////////////////////////////////////////////////////////////////////

void testRequeueNullEvent()
{
    std::cout << DFR::gEventManagerName << ": Test Requeue Null Event" << std::endl;
    auto em = makeManager();
    CHECK(em->requeueEvent(nullptr) == false);
}
void testRequeueOrderingBySimTime()
{
    std::cout << DFR::gEventManagerName << ": Test Requeue Ordering By Sim Time" << std::endl;
    auto em = makeManager();
    unsigned int c1 = em->addEvent(std::make_unique<DFR::Event>(1.0));
    unsigned int c2 = em->addEvent(std::make_unique<DFR::Event>(2.0));

    auto e = em->getNextEvent();
    CHECK(e != nullptr);
    if (e)
    {
        e->updateSimTime(3.0);
        CHECK(em->requeueEvent(std::move(e)) == true);
    }

    CHECK(em->getNextEvent()->getEventID() == c2);
    CHECK(em->getNextEvent()->getEventID() == c1);
    CHECK(em->getNextEvent() == nullptr);
}
void testRequeueOrderingByPriorityAtSameTime()
{
    std::cout << DFR::gEventManagerName << ": Test Requeue Ordering By Priority At Same Time" << std::endl;
    auto em = makeManager();
    unsigned int c1 = em->addEvent(std::make_unique<DFR::Event>(1.0, 10));
    unsigned int c2 = em->addEvent(std::make_unique<DFR::Event>(1.0, 20));

    auto e = em->getNextEvent();
    CHECK(e != nullptr);
    if (e)
    {
        e->setPriority(30);
        CHECK(em->requeueEvent(std::move(e)) == true);
    }

    CHECK(em->getNextEvent()->getEventID() == c2);
    CHECK(em->getNextEvent()->getEventID() == c1);
    CHECK(em->getNextEvent() == nullptr);
}

void testRequeueFifoTieBreakAtSameTimeAndPriority()
{
    std::cout << DFR::gEventManagerName << ": Test Requeue FIFO Tie Break At Same Time And Priority" << std::endl;
    auto em = makeManager();
    unsigned int c1 = em->addEvent(std::make_unique<DFR::Event>(1.0, 10));
    unsigned int c2 = em->addEvent(std::make_unique<DFR::Event>(1.0, 10));

    auto e = em->getNextEvent();
    CHECK(e != nullptr);
    if (e)
    {
        CHECK(em->requeueEvent(std::move(e)) == true);
    }

    CHECK(em->getNextEvent()->getEventID() == c2);
    CHECK(em->getNextEvent()->getEventID() == c1);
    CHECK(em->getNextEvent() == nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// Test getNextEventAtOrBefore usage
////////////////////////////////////////////////////////////////////////////////

void testGetNextEventAtOrBeforeEmptyQueue()
{
    std::cout << DFR::gEventManagerName << ": Test Get Next Event At Or Before Empty Queue" << std::endl;
    auto em = makeManager();
    bool queWasEmpty = false;
    CHECK(em->getNextEventAtOrBefore(10.0, queWasEmpty) == nullptr);
    CHECK(queWasEmpty == true);
}

void testGetNextEventAtOrBeforeReturnsEarlierEvent()
{
    std::cout << DFR::gEventManagerName << ": Test Get Next Event At Or Before Returns Earlier Event" << std::endl;
    auto em = makeManager();
    unsigned int c1 = em->addEvent(std::make_unique<DFR::Event>(1.0));
    em->addEvent(std::make_unique<DFR::Event>(20.0));

    bool queWasEmpty = true;
    auto e = em->getNextEventAtOrBefore(10.0, queWasEmpty);
    CHECK(queWasEmpty == false);
    CHECK(e != nullptr);
    CHECK(e && e->getEventID() == c1);

    // The returned event was removed; the later one is still queued.
    const DFR::Event* next = em->peekNextEvent();
    CHECK(next != nullptr);
    CHECK(next && next->simTimeOfEvent() == 20.0);
}

void testGetNextEventAtOrBeforeHoldsBackLaterEvent()
{
    std::cout << DFR::gEventManagerName << ": Test Get Next Event At Or Before Holds Back Later Event" << std::endl;
    auto em = makeManager();
    em->addEvent(std::make_unique<DFR::Event>(20.0));

    bool queWasEmpty = true;
    CHECK(em->getNextEventAtOrBefore(10.0, queWasEmpty) == nullptr);
    CHECK(queWasEmpty == false);
    CHECK(em->peekNextEvent() != nullptr); // still queued
}

void testGetNextEventAtOrBeforeExcludesBoundary()
{
    // Pins the ">" behavior: an event exactly at maxSimTime is deferred.
    std::cout << DFR::gEventManagerName << ": Test Get Next Event At Or Before Excludes Boundary" << std::endl;
    auto em = makeManager();
    em->addEvent(std::make_unique<DFR::Event>(10.0));

    bool queWasEmpty = true;
    CHECK(em->getNextEventAtOrBefore(10.0, queWasEmpty) != nullptr);
    CHECK(queWasEmpty == false);
    CHECK(em->peekNextEvent() == nullptr);
}

void testGetNextEventAtOrBeforeDrainsInOrder()
{
    std::cout << DFR::gEventManagerName << ": Test Get Next Event At Or Before Drains In Order" << std::endl;
    auto em = makeManager();
    em->addEvent(std::make_unique<DFR::Event>(3.0));
    em->addEvent(std::make_unique<DFR::Event>(1.0));
    em->addEvent(std::make_unique<DFR::Event>(2.0));
    em->addEvent(std::make_unique<DFR::Event>(5.0));
    em->addEvent(std::make_unique<DFR::Event>(4.0));

    std::vector<double> times;
    bool queWasEmpty = false;
    while (auto e = em->getNextEventAtOrBefore(4.0, queWasEmpty))
    {
        times.push_back(e->simTimeOfEvent());
    }
    CHECK((times == std::vector<double>{1.0, 2.0, 3.0, 4.0}));
    CHECK(queWasEmpty == false); // stopped because of time, not emptiness
}