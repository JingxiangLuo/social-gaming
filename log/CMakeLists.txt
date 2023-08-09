# Set to not compile with glog's gtest headers 
# which causes compiler errors during linkage.
set(WITH_GTEST OFF)
# Set to not compile glog's testing code.
set(BUILD_TESTING OFF)    
add_subdirectory(glog)
add_subdirectory(config)

