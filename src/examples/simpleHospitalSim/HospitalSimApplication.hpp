////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <DFRCore/Application.hpp>

class HospitalSimApplication : public DFR::Application
{
public:
    HospitalSimApplication(bool inBatchMode);
    virtual ~HospitalSimApplication() = default;

    //! @brief Initialize the hospital simulation.
    void initialize() override;

    /** @name Example Hospital Simulation Methods */
    //@{
    //! @brief Inject a patient into the hospital simulation.
    void injectPatient();
    //@}

private:

};
