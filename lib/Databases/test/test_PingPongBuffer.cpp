#include <ctime>
#include <gtest/gtest.h>
#include <memory>

#include "Databases/PingPongBuffer.hpp"
#include "Telemetry/Telemetry.hpp"

// Define a test fixture class
class PingPongBufferTest : public ::testing::Test { // NOSONAR
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  PingPongBufferTest() { // NOSONAR
    // You can do set-up work for each test here.
  }

  ~PingPongBufferTest() override { // NOSONAR
    // You can do clean-up work that doesn't throw exceptions here.
  }

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test).
    testBuffer = std::make_unique<PingPongBuffer<tmSample>>();

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

  std::unique_ptr<PingPongBuffer<tmSample>> testBuffer;
};

// Test cases using the test fixture
// I usually use this test to ensure I can link
TEST_F(PingPongBufferTest, InitProbably) {}

TEST_F(PingPongBufferTest, IncrementHelperTest) {

  EXPECT_EQ(aSimpleTelemetry.readValue, 0);
  EXPECT_EQ(aSimpleTelemetry.timestamp, 0);

  incrementTM();
  EXPECT_EQ(aSimpleTelemetry.readValue, 1);
  EXPECT_EQ(aSimpleTelemetry.timestamp, 1);
}

// Thank god I did this test, implemented a bug
// used
TEST_F(PingPongBufferTest, push) {
  const auto &aSample = testBuffer->getLatest();

  EXPECT_EQ(aSample.readValue, aSimpleTelemetry.readValue);
  EXPECT_EQ(aSample.timestamp, aSimpleTelemetry.timestamp);

  incrementTM();

  testBuffer->update(aSimpleTelemetry);

  const auto &aSecondSample = testBuffer->getLatest();

  EXPECT_EQ(aSecondSample.readValue, aSimpleTelemetry.readValue);
  EXPECT_EQ(aSecondSample.timestamp, aSimpleTelemetry.timestamp);
  EXPECT_NE(aSample.readValue, aSimpleTelemetry.readValue);
  EXPECT_NE(aSample.timestamp, aSimpleTelemetry.timestamp);

  incrementTM();

  testBuffer->update(aSimpleTelemetry);

  auto aSThirdSample = testBuffer->getLatest();
  const auto &aExtraSample = testBuffer->getLatest();

  EXPECT_EQ(aSThirdSample.readValue, aSimpleTelemetry.readValue);
  EXPECT_EQ(aSThirdSample.timestamp, aSimpleTelemetry.timestamp);

  EXPECT_NE(aSample.readValue, aSimpleTelemetry.readValue);
  EXPECT_NE(aSample.timestamp, aSimpleTelemetry.timestamp);

  EXPECT_NE(aSecondSample.readValue, aSimpleTelemetry.readValue);
  EXPECT_NE(aSecondSample.timestamp, aSimpleTelemetry.timestamp);

  // const ref vs auto
  EXPECT_NE(&aSample, &aSThirdSample);

  EXPECT_NE(&aSecondSample, &aSThirdSample);

  // test we get values by value not by ref
  EXPECT_NE(&aSample, &aExtraSample);
};