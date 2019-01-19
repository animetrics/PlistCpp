cmake_minimum_required(VERSION 2.6) 
project(Plist) 
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${EXTRA_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_CXX_FLAGS} -Wall -DTEST_VERBOSE")

set(SCRIPT_EXT sh)
set(MY_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(ARCH_DIR ".")
if(APPLE)
	# on Mavericks, need to link to libstdc++ directly because libc++ is default
	# and won't work
	if(NOT ${CMAKE_SYSTEM_VERSION} VERSION_LESS 13.0)
		set(EXTRA_CXX_FLAGS "-stdlib=libstdc++")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lstdc++")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lstdc++")
	endif()
elseif(MSVC)
	set(MY_BUILD_TYPE "\$(Configuration)")
	set(ARCH_DIR "${MY_BUILD_TYPE}")
	set(SCRIPT_EXT bat)
endif()

find_package(Boost REQUIRED COMPONENTS locale)
find_package(UnitTest++ REQUIRED)

add_executable(runTests src/runTests.cpp src/plistTests.cpp src/pugixml.cpp src/Plist.cpp src/PlistDate.cpp)
target_include_directories(runTests PUBLIC ${UTPP_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(runTests UnitTest++)
add_dependencies(runTests UnitTest++)

add_custom_command(
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	TARGET runTests
	POST_BUILD
	COMMAND ./runTests.${SCRIPT_EXT} ${MY_BUILD_TYPE} ${ARCH_DIR}
)
