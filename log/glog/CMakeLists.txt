include(FetchContent)
FetchContent_Declare(
  glog
  GIT_REPOSITORY https://github.com/google/glog.git
  GIT_TAG v0.6.0
)
FetchContent_MakeAvailable(glog)

