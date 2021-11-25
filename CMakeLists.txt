cmake_minimum_required(VERSION 3.13.0)


project(magnit_meetup VERSION 21.11 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)


add_subdirectory(presentation)
find_package(Boost QUIET REQUIRED)
find_package(qt QUIET REQUIRED Core)


enable_testing()
add_subdirectory(test)
