//
// main() for monolithic build of the gumbo_parser tests
//

#include "gtest/gtest.h"

#include <iostream>
#include <sstream>

using namespace std;

using ::testing::InitGoogleTest;
using ::testing::TestEventListeners;
using ::testing::TestWithParam;
using ::testing::UnitTest;
using ::testing::Test;
using ::testing::Values;


int main(int argc, const char** argv)
{
	int rv = 0;

	InitGoogleTest(&argc, argv);

	rv = RUN_ALL_TESTS();

	return rv;
}
