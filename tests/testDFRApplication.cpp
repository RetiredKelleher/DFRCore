/////////////////////////////////////////////////////////////////////////////////
//! @file testDFRApplication.cpp
//! @author Thomas Kelleher
//! @date 2024-06-01
//! @brief Unit tests for DFRApplication class.
/////////////////////////////////////////////////////////////////////////////////

#include <DFRCore/Application.hpp>
#include <cassert>

void testDFRApplicationInitialization()
{
    DFR::Application app;
    app.initialize();
    // Add assertions here to verify the state of the application after initialization
}

int main()
{
    testDFRApplicationInitialization();
    // Add calls to other test functions here
    return 0;
}