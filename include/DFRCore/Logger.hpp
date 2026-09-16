/////////////////////////////////////////////////////////////////////////////
// @file DFRLogger.hpp
// @brief Logger class for DFRCore, providing logging functionality for the simulation architecture.
// @author Thomas Kelleher
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <iostream>
#include <mutex>

namespace DFR {

    //! @brief DFR::Logger is a base class for logging messages in the DFRCore framework.
//! It will provide a simple interface for logging messages with time stamps.
//! It is expected to be inherited by other classes that require logging functionality,
//! allowing them to log messages to a file or console as needed.
class Logger
{
public:

    // Enums for log levels (e.g., INFO, WARNING, ERROR)
    enum class LogLevel
    {
        eInfo,       //!< Informational messages that highlight the progress of the application at coarse-grained level.
        eWarning,    //!< Warning messages that indicate a potential issue.
        eError       //!< Error messages that indicate a serious problem.
    };

    // Constructor that initializes the logger with a specified log file name
    Logger();

    // Destructor that closes the log file stream
    ~Logger();

    //! @brief Set the log level for filtering messages
    //! @param level The log level to set (e.g., INFO, WARNING, ERROR)
    void setLogLevel(DFR::Logger::LogLevel level);

    //! @brief Can logging occur at the specified log level?
    //! @param level The log level to check (e.g., INFO, WARNING, ERROR)
    //! @return true if logging is allowed at the specified level, false otherwise
    bool canLog(DFR::Logger::LogLevel level) const;

    /** @name Log a message to the log file with a timestamp */
    //@{
    //! @brief Log an informational message to the log file with a timestamp.
    //! @param message The message to log
    void logInfo(const std::string& message);
    //! @brief Log a warning message to the log file with a timestamp.
    //! @param message The message to log
    void logWarning(const std::string& message);
    //! @brief Log an error message to the log file with a timestamp.
    //! @param message The message to log
    void logError(const std::string& message);
    //@}

protected:

    /** @name Actual logging methods - Expect to be overridden by derived classes */
    //@{
    //! Log Info message - to be overridden by derived classes
    //! @param message The message to log
    virtual void logInfoImpl(const std::string& message);
    //! Log Warning message - to be overridden by derived classes
    //! @param message The message to log
    virtual void logWarningImpl(const std::string& message);
    //! Log Error message - to be overridden by derived classes
    //! @param message The message to log
    virtual void logErrorImpl(const std::string& message);
    //@}

private:

    mutable std::recursive_mutex mMutex{}; //!< Mutex to protect access to the log file stream and log level

    DFR::Logger::LogLevel mLogLevel; // Current log level for filtering messages
};

} // Namespace DFR