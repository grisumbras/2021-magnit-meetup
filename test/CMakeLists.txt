option(
  RUN_TESTING
  "Running tests is part of the default target."
  "${BUILD_TESTING}"
)
if(RUN_TESTING)
  message(STATUS "Tests will be run by default")
endif()

function(add_boost_test target source)
  add_executable(${target} ${source})
  target_link_libraries(${target} PRIVATE Boost::boost)
  if (NOT BUILD_TESTING)
    set_target_properties(${target} PROPERTIES EXCLUDE_FROM_ALL TRUE)
  endif()

  if (NOT BUILD_TESTING)
    return()
  endif()

  target_include_directories(${target}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
  )

  set(stamp_name "${CMAKE_CURRENT_BINARY_DIR}/check_${target}.stamp")
  add_custom_command(
    OUTPUT "${stamp_name}"
    DEPENDS $<TARGET_FILE:${target}>
    COMMAND ${target}
    COMMAND "${CMAKE_COMMAND}" -E touch "${stamp_name}"
    COMMENT "Running test ${target}"
  )

  add_custom_target(check_${target} DEPENDS "${stamp_name}")
  add_dependencies(check check_${target})

  add_test(NAME ${target} COMMAND ${target})
endfunction()

if(NOT BUILD_TESTING)
  return()
endif()


add_custom_target(check WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

if(RUN_TESTING)
  set_target_properties(check PROPERTIES EXCLUDE_FROM_ALL FALSE)
endif()


add_boost_test(running_meter running_meter.cpp)

add_boost_test(downcasting downcasting.cpp)
target_link_libraries(downcasting PRIVATE qt::qt)

add_boost_test(assoc_containers assoc_containers.cpp)
target_link_libraries(assoc_containers PRIVATE qt::qt)

add_boost_test(row_range row_range.cpp)
target_link_libraries(row_range PRIVATE qt::qt)
