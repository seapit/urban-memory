// Chip-aware module template - test example. Only ever compiled if CHIPS
// (in lib/Database/CMakeLists.txt) is populated - that's what gates this, not
// a comment block: test/CMakeLists.txt's if(CHIPS) branch is the only
// thing that ever references this file.
#include <gtest/gtest.h>

#include "Database/Config.hpp"

TEST(DatabaseTest, ChipConfigHeaderIsReachable) {
  // Proves this chip's target resolves *a* Config.hpp - whichever one wins
  // (this chip's own override, or Common's default) is example content,
  // not something the template itself should assert a value for.
  EXPECT_GE(Database::kMaxRetries, 0);
}
