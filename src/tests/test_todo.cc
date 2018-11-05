#include "gtest/gtest.h"
#include "todo.hpp"

using namespace eventual;
// testcase: test_todo
class test_todo : public ::testing::Test {
protected:
	test_todo() {

	}

	virtual ~test_todo() {

	}

	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).


	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).


	}

	todo x;
};


// testcase: test_todo
// testname: ctor
TEST_F(test_todo, ctor) {
	int i = 1;
	EXPECT_EQ(1, i);
}

// testcase: test_todo
// testname: then
TEST_F(test_todo, then) {
	int i = 1;
	EXPECT_EQ(1, i);
}

// testcase: test_todo
// testname: operator_eq
TEST_F(test_todo, operator_eq) {
	int i = 1;
	EXPECT_EQ(1, i);
}

