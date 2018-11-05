#include "gtest/gtest.h"
#include "zero_copy_value.hpp"

using namespace eventual;
class test_zero_copy_value : public ::testing::Test {
protected:
	test_zero_copy_value() {

	}

	virtual ~test_zero_copy_value() {

	}

	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).


	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).


	}

	zero_copy_value x;
};


// testcase: test_zero_copy_value
// testname: ctor
TEST_F(test_zero_copy_value, ctor) {
	int i = 1;
	EXPECT_EQ(1, i);
}

// testcase: test_zero_copy_value
// testname: operator_eq
TEST_F(test_zero_copy_value, operator_eq) {
	int i = 1;
	EXPECT_EQ(1, i);
}





