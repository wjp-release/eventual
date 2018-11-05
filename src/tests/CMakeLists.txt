aux_source_directory(. DIRSRCS)
add_executable(tests ${DIRSRCS})
target_link_libraries(tests gtest_main eventual)
