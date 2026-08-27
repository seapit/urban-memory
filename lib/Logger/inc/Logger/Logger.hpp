/**
 * \file Logger.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
#ifndef LOGGER__
#define LOGGER__

/// INCLUDES
#include <string>
#include <sys/syslog.h>
#include <utility>

/// CMAKE INCLUDES

/// USER INCLUDES
#include "Common/HelperMacros.hpp"

/// NAMESPACE
namespace CommonTool {

/// DEFINES
constexpr std::string_view FAIL_CONN_CLOSE{"Failed to close connection to: "};

ENUM_AND_STRING(LogType, (EMERGENCY), (ALERT), (CRITICAL), (ERROR), (WARNING),
                (NOTICE), (INFO), (DBG))
// GET_ENUM_IMPL(LogType)

ENUM_AND_STRING(LogFamily, (KERN), (USER), (MAIL), (DAEMON), (AUTH), (SYSLOG),
                (LPR), (NEWS), (UUCP), (CRON), (AUTHPRIV), (FTP), (RSVD1),
                (RSVD2), (RSVD3), (RSVD4), (LOCAL0), (LOCAL1), (LOCAL2),
                (LOCAL3), (LOCAL4), (LOCAL5), (LOCAL6), (LOCAL7))

#define EMERG_LOG(loggerName, ...)                                             \
  if (loggerName) {                                                            \
    loggerName->emergency(std::format(__VA_ARGS__));                           \
  } else {                                                                     \
  }

#define ALERT_LOG(loggerName, ...)                                             \
  if (loggerName) {                                                            \
    loggerName->alert(std::format(__VA_ARGS__));                               \
  } else {                                                                     \
  }

#define CRITICAL_LOG(loggerName, ...)                                          \
  if (loggerName) {                                                            \
    loggerName->critical(std::format(__VA_ARGS__));                            \
  } else {                                                                     \
  }

#define ERROR_LOG(loggerName, ...)                                             \
  if (loggerName) {                                                            \
    loggerName->error(std::format(__VA_ARGS__));                               \
  } else {                                                                     \
  }

#define NOTICE_LOG(loggerName, ...)                                            \
  if (loggerName) {                                                            \
    loggerName->notice(std::format(__VA_ARGS__));                              \
  } else {                                                                     \
  }

#define WARN_LOG(loggerName, ...)                                              \
  if (loggerName) {                                                            \
    loggerName->warn(std::format(__VA_ARGS__));                                \
  } else {                                                                     \
  }

#define INFO_LOG(loggerName, ...)                                              \
  if (loggerName) {                                                            \
    loggerName->info(std::format(__VA_ARGS__));                                \
  } else {                                                                     \
  }

#ifdef DEBUG
#define DEBUG_LOG(loggerName, ...)                                             \
  if (loggerName) {                                                            \
    loggerName->debug(std::format(__VA_ARGS__));                               \
  } else {                                                                     \
  }
#else
#define DEBUG_LOG(loggernName, ...) ;
#endif

class Logger {
public:
  explicit Logger(const std::string& CallerName = "DefaultName",
                  const CommonTool::LogFamily& rhsFamily = CommonTool::LogFamily::LOCAL0) :
           m_CallerName(CallerName)
  {
    auto aFamily = (std::to_underlying(rhsFamily) << 3);

  #ifdef DEBUG
    openlog(m_CallerName.c_str(), LOG_PID | LOG_CONS | LOG_PERROR, aFamily);
  #else
    openlog(m_CallerName.c_str(), LOG_PID, aFamily);
  #endif
  }

  void debug(const std::string &msg) const {
    log(msg, CommonTool::LogType::DBG);
  }

  void info(const std::string &msg) const {
    log(msg, CommonTool::LogType::INFO);
  }

  void warning(const std::string &msg) const {
    log(msg, CommonTool::LogType::WARNING);
  }

  void notice(const std::string &msg) const {
    log(msg, CommonTool::LogType::NOTICE);
  }

  void error(const std::string &msg) const {
    log(msg, CommonTool::LogType::ERROR);
  }

  void critical(const std::string &msg) const {
    log(msg, CommonTool::LogType::CRITICAL);
  }

  void alert(const std::string &msg) const {
    log(msg, CommonTool::LogType::ALERT);
  }

  void emergency(const std::string &msg) const {
    log(msg, CommonTool::LogType::EMERGENCY);
  }

  ~Logger() { closelog(); }

  std::string_view getCallerName() const { return m_CallerName; };

#ifdef DEBUG
  void logVersion() const;
#endif
  // protected:
private:
  void log(const std::string &rhsMessage, CommonTool::LogType rhsLogType) const;

  std::string m_CallerName;
};
} // namespace CommonTool

#endif // LOGGER__