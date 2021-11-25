set(flags
  -Wall
  -Wextra
  -Wpedantic
  -Wno-missing-field-initializers
  -Wno-int-in-bool-context
  -Werror
)
list(JOIN flags " " CMAKE_CXX_FLAGS_INIT)
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/stage" CACHE STRING ON)

set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wno-parentheses")

set(BUILD_TESTING ON CACHE BOOL "")

set(Boost_USE_STATIC_LIBS TRUE CACHE BOOL "")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
