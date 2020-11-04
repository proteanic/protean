# General project setup and options

project(
  protean HOMEPAGE_URL https://github.com/proteanic/protean LANGUAGES CXX)
cmake_minimum_required(
  VERSION 3.0)

option(
  ENABLE_TESTS "Enable tests" ON)
option(
  BOOST_STATIC_LINK_PATH "Force required Boost libs to be statically linked using this location" "")

set(
  CMAKE_CXX_STANDARD 11)
set(
  CMAKE_CXX_STANDARD_REQUIRED ON)
string(
  REPLACE "-O2" "-O3" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(
    CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build" FORCE)
endif()

# Common compiler / linker setup

add_compile_options(
  -fdata-sections -ffunction-sections -Wno-deprecated-declarations -Wno-multichar)

set(
  BoostLibs boost_date_time boost_filesystem boost_iostreams boost_regex boost_system)
if(BOOST_STATIC_LINK_PATH)
  list(
    TRANSFORM BoostLibs PREPEND ${BOOST_STATIC_LINK_PATH}/lib)
  list(
    TRANSFORM BoostLibs APPEND .a)
endif()

# Platform-specific compiler / linker setup (including setting up variables for later use)

if(APPLE)
  add_link_options(
    -Wl,-dead_strip)
else()
  add_link_options(
    -Wl,--gc-sections -Wl,--no-undefined)
endif()

# Target libprotean

file(
  GLOB_RECURSE LibSourceFiles
  CONFIGURE_DEPENDS
  "src/*.cpp")
add_library(
  protean SHARED ${LibSourceFiles})
set_target_properties(
  protean PROPERTIES BUILD_RPATH_USE_ORIGIN TRUE)
target_include_directories(
  protean PUBLIC .)
target_link_libraries(
  protean PUBLIC xerces-c ${BoostLibs} z)

# Target protean_test

if(ENABLE_TESTS)
  enable_testing()

  file(
    GLOB_RECURSE TestSourceFiles
    CONFIGURE_DEPENDS
    "test/*.cpp")
  add_executable(
    protean_test ${TestSourceFiles})
  target_compile_definitions(
    protean_test PRIVATE BOOST_TEST_DYN_LINK)
  target_link_libraries(
    protean_test PRIVATE protean boost_unit_test_framework)
endif()
