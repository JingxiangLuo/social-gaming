#include "logconfig.h"
#include "gtest/gtest.h"
#include <glog/logging.h>

using namespace testing;

/////////////////////////////////////////////////////////////////////////////
// Main function override for test suite
/////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  // Only custom actions that MUST be run before test which CANNOT be...
  // ...expressed within the gtest framework of fixtures/suites should...
  // ...be here
  google::InitGoogleLogging(argv[0], &customPrefix);
  FLAGS_alsologtostderr = 1;
  FLAGS_log_dir = "../social-gaming/log/files";

  return RUN_ALL_TESTS();
}
