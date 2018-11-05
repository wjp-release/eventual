# CMake generated Testfile for 
# Source directory: /Users/wjp/Desktop/workspace/eventual/src
# Build directory: /Users/wjp/Desktop/workspace/eventual/src/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(run_all_tests "build/tests/tests")
subdirs("eventual")
subdirs("googletest")
subdirs("tests")
