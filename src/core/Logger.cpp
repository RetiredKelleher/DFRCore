/////////////////////////////////////////////////////////////////////////////////
//! @file        Logger.cpp
//! @brief       Implementation of the DFRLogger class, providing logging functionality for the DFRSimArch framework.
//! @author      Thomas Kelleher
////////////////////////////////////////////////////////////////////////////////

#include <DFRCore/Logger.hpp>
#include <iostream>

namespace DFR {

Logger::Logger() : mLogLevel(DFR::Logger::LogLevel::eInfo)
{
    // Constructor implementation (if needed)
}

Logger::~Logger()
{
    // Destructor implementation (if needed)
}

void Logger::setLogLevel(DFR::Logger::LogLevel level)
{
    std::lock_guard<std::recursive_mutex> lock(mMutex);
    mLogLevel = level;
}

bool Logger::canLog(DFR::Logger::LogLevel level) const
{
    std::lock_guard<std::recursive_mutex> lock(mMutex);
    return static_cast<int>(level) >= static_cast<int>(mLogLevel);
}

void Logger::logInfo(const std::string& message)
{
    if (canLog(LogLevel::eInfo))
    {
        std::lock_guard<std::recursive_mutex> lock(mMutex);
        logInfoImpl(message);
    }
}

void Logger::logWarning(const std::string& message)
{
    if (canLog(LogLevel::eWarning))
    {
        std::lock_guard<std::recursive_mutex> lock(mMutex);
        logWarningImpl(message);
    }
}

void Logger::logError(const std::string& message)
{
    if (canLog(LogLevel::eError))
    {
        std::lock_guard<std::recursive_mutex> lock(mMutex);
        logErrorImpl(message);
    }
}

void Logger::logInfoImpl(const std::string& message)
{
    // Default implementation (can be overridden by derived classes)
    std::cout << "[INFO] " << message << std::endl;
}

void Logger::logWarningImpl(const std::string& message)
{
    // Default implementation (can be overridden by derived classes)
    std::cout << "[WARNING] " << message << std::endl;
}

void Logger::logErrorImpl(const std::string& message)
{
    // Default implementation (can be overridden by derived classes)
    std::cerr << "[ERROR] " << message << std::endl;
}

} // namespace DFR
