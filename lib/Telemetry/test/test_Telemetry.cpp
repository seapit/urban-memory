#include <gtest/gtest.h>
// Define a test fixture class
class TelemetryTest : public ::testing::Test { // NOSONAR
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  TelemetryTest() { // NOSONAR
    // You can do set-up work for each test here.
  }

  ~TelemetryTest() override { // NOSONAR
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
TEST_F(TelemetryTest, InitProbably) {}
