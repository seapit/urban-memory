#include <ctime>
#include <gtest/gtest.h>
#include <memory>

#include "Databases/TelemetryDB.hpp"
#include "Telemetry/Telemetry.hpp"

// Define a test fixture class
class CriticalTelemetryDBTest : public ::testing::Test { // NOSONAR
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  CriticalTelemetryDBTest() { // NOSONAR
    // You can do set-up work for each test here.
  }

  ~CriticalTelemetryDBTest() override { // NOSONAR
    // You can do clean-up work that doesn't throw exceptions here.
  }

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test).
    testDb = std::make_unique<CriticalTelemetryDB>();

    aSimpleTelemetry.readValue = 0;
    aSimpleTelemetry.timestamp = 0;
  }
  void incrementTM() {
    aSimpleTelemetry.readValue += 1;
    aSimpleTelemetry.timestamp += 1;
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
  tmSample aSimpleTelemetry;

  std::unique_ptr<CriticalTelemetryDB> testDb;
};

// Test cases using the test fixture
// I usually use this test to ensure I can link
TEST_F(CriticalTelemetryDBTest, InitProbably) {}

TEST_F(CriticalTelemetryDBTest, inputChecking) {
  constexpr std::size_t aSize = (sizecast(TelemetryIds::MAX) + 1);

  // will just use index = id
  std::array<tmSample, aSize> associatedTelemetries;

  // setup values for TM
  for (std::size_t i = 0; i < aSize; ++i) {
    std::cout << TelemetryIdsStrings[i] << std::endl;
    associatedTelemetries[i].readValue = static_cast<double>(i);
    associatedTelemetries[i].timestamp = (i);

    // insert the telemetry into the db
    EXPECT_NO_FATAL_FAILURE(testDb->publish(static_cast<TelemetryIds>(i),
                                            associatedTelemetries[i]));
  }

  std::cout << "Test1" << std::endl;
  for (std::size_t i = 0; i < aSize; ++i) {
    const auto &aInputTelemetry = associatedTelemetries[i];
    const auto &aReturnTelemetry =
        testDb->getLatest(static_cast<TelemetryIds>(i));

    // test we didn't receive the same object accidentally and ensure we
    // returned by value
    EXPECT_NE(&aInputTelemetry, &aReturnTelemetry);

    if (i < (aSize - 1)) {
      // Ensure we received the same values input as we did output
      EXPECT_EQ(aInputTelemetry.readValue, aReturnTelemetry.readValue);
      EXPECT_EQ(aInputTelemetry.readValue, aReturnTelemetry.readValue);
    } else {
      // Ensure we received an empty telemetry one
      EXPECT_NE(aInputTelemetry.readValue, aReturnTelemetry.readValue);
      EXPECT_NE(aInputTelemetry.readValue, aReturnTelemetry.readValue);
    }
  }
}