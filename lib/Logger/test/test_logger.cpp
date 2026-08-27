#include "Logger/Logger.hpp"
#include <gtest/gtest.h>

// Define a test fixture class
class LoggerTest : public ::testing::Test {
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  LoggerTest() {
    // You can do set-up work for each test here.
  }

  ~LoggerTest() override {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test).
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

// Test cases using the test fixture
TEST_F(LoggerTest, InitProbably) {
  CommonTool::Logger aLogger("GoogleTest", CommonTool::LogFamily::LOCAL0);
  EXPECT_EQ(aLogger.getCallerName(), "GoogleTest");

#ifdef DEBUG
  EXPECT_NO_THROW(aLogger.logVersion());
#endif

  EXPECT_NO_THROW(aLogger.debug("Debug level test message"));
  EXPECT_NO_THROW(aLogger.info("Info level test message"));
  EXPECT_NO_THROW(aLogger.notice("Notice level test message"));
  EXPECT_NO_THROW(aLogger.warning("Warning level test message"));
  EXPECT_NO_THROW(aLogger.error("Error level test message"));
  EXPECT_NO_THROW(aLogger.critical("Critical level test message"));
  EXPECT_NO_THROW(aLogger.alert("Alert level test message"));
  EXPECT_NO_THROW(aLogger.emergency("Emergency level test message"));

#ifdef DEBUG
  EXPECT_NO_THROW(aLogger.logVersion());
#endif
}
