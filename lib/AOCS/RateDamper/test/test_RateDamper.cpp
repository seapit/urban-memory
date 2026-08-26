#include "RateDamper/RateDamper.hpp"
#include "RateDamper/Wheels/RW1000.hpp"

#include <gtest/gtest.h>
// Define a test fixture class
class RateDamperTest : public ::testing::Test { // NOSONAR
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  RateDamperTest() { // NOSONAR
    // You can do set-up work for each test here.
  }

  ~RateDamperTest() override { // NOSONAR
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
  RateDamper aDamper{RW1000Configuration};
};

// Test cases using the test fixture
TEST_F(RateDamperTest, InitProbably) {}

TEST_F(RateDamperTest, boundedTests) {
  double aLimitCheckTorqueClamp =
      (RW1000Configuration.outputTorque_Floor_N_m + 100.0);

  double aVoltageCheck = (RW1000Configuration.nominalVoltageVariation_V + 0.1);

  EXPECT_EQ(aDamper.boundedTorqueValidation(aLimitCheckTorqueClamp),
            RW1000Configuration.outputTorque_Floor_N_m);
  EXPECT_EQ(aDamper.boundedTorqueValidation((0 - aLimitCheckTorqueClamp)),
            (0 - RW1000Configuration.outputTorque_Floor_N_m));

  EXPECT_EQ(aDamper.boundedVoltageValidation(
                RW1000Configuration.nominalVoltage_V + aVoltageCheck),
            (RW1000Configuration.nominalVoltage_V +
             RW1000Configuration.nominalVoltageVariation_V));
  EXPECT_EQ(aDamper.boundedVoltageValidation(
                (RW1000Configuration.nominalVoltage_V - aVoltageCheck)),
            (RW1000Configuration.nominalVoltage_V -
             RW1000Configuration.nominalVoltageVariation_V));

  double amaximumInputRate = RW1000Configuration.outputTorque_Floor_N_m /
                             RW1000Configuration.dampingGain_N_m_s_per_rad;

  EXPECT_EQ(aDamper.boundedAngularRateValidation(amaximumInputRate + 1),
            amaximumInputRate);
  EXPECT_EQ(aDamper.boundedAngularRateValidation((0 - amaximumInputRate)),
            amaximumInputRate);
}
