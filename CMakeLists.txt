cmake_minimum_required(VERSION 2.6) 
project(Plist) 
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${EXTRA_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_CXX_FLAGS} -Wall -DTEST_VERBOSE")

find_package(UnitTest++ REQUIRED)

add_executable(runTests src/runTests.cpp src/plistTests.cpp src/pugixml.cpp src/Plist.cpp src/PlistDate.cpp)
target_include_directories(runTests PUBLIC ${UTPP_INCLUDE_DIRS})
add_dependencies(runTests UnitTest++)

add_custom_command(
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	TARGET runTests
	POST_BUILD
	COMMAND ./runTests.${SCRIPT_EXT} ${MY_BUILD_TYPE} ${ARCH_DIR}
)
