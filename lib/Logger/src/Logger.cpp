/**
 * \file Logger.cpp
 * \brief A brief description of what this file is.
 */

/// INCLUDES

/// CMAKE INCLUDES
#include "Logger/version.h"

/// USER INCLUDES
#include <utility>

#include "Common/HelperMacros.hpp"
#include "Logger/Logger.hpp"

/// USER INCLUDES

/// NAMESPACE

/// DEFINES
namespace CommonTool {
  void Logger::log(const std::string& rhsMessage, CommonTool::LogType rhsLogType) const
  {
    using namespace CommonTool;
    syslog(std::to_underlying(rhsLogType), "%s - %s", std::string(getLogTypestr(rhsLogType)).c_str(), rhsMessage.c_str());
  }

#ifdef DEBUG
  void Logger::logVersion() const
  {
    std::string aName = MODULE_NAME;
    syslog(std::to_underlying(CommonTool::LogType::INFO), "%s module v%i.%i.%i.%i start.", MODULE_NAME,
           Logger_VERSION_MAJOR, Logger_VERSION_MINOR, Logger_VERSION_PATCH, Logger_VERSION_TWEAK);
  }
#endif

} // namespace CommonTool