// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

// -- BEGIN LICENSE BLOCK ----------------------------------------------
// -- END LICENSE BLOCK ------------------------------------------------

//----------------------------------------------------------------------
/*!\file
 *
 * \author  Felix Exner exner@fzi.de
 * \date    2022-01-13
 *
 */
//----------------------------------------------------------------------

#pragma once

#include <string>

namespace driver_svh {
enum class LogLevel
{
  DEBUG = 0,
  INFO,
  WARN,
  ERROR,
  FATAL,
  NONE
};

inline std::string logLevelToString(const LogLevel level)
{
  switch (level)
  {
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARN:
      return "WARN";
    case LogLevel::ERROR:
      return "ERROR";
    case LogLevel::FATAL:
      return "FATAL";
    default:
      return "NONE";
  }
}
} // namespace driver_svh
