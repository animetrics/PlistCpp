cmake_minimum_required (VERSION 2.6) 
project (Plist) 

include_directories (include)

set (SCRIPT_EXT sh)
set (MY_BUILD_TYPE ${CMAKE_BUILD_TYPE})
IF(APPLE)
	set (ARCH_DIR OSX)
	# on Mavericks, need to link to libstdc++ directly because libc++ is default
	# and won't work
	if(NOT ${CMAKE_SYSTEM_VERSION} VERSION_LESS 13.0)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libstdc++")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libstdc++")
		set(EXTRA_LINK_LIBRARIES stdc++)
	endif()
ELSEIF(MSVC)
	set (ARCH_DIR Windows)
	set (SCRIPT_EXT bat)
	set (MY_BUILD_TYPE "\$(Configuration)")
ELSE()
	set (ARCH_DIR Linux)
ENDIF()

link_directories (${CMAKE_SOURCE_DIR}/${ARCH_DIR}/lib)
include_directories (include ${CMAKE_SOURCE_DIR}/${ARCH_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -DTEST_VERBOSE")

add_executable(runTests src/runTests.cpp src/plistTests.cpp src/pugixml.cpp
    src/Plist.cpp src/PlistDate.cpp)

IF(MSVC10)
	target_link_libraries(runTests UnitTest++.vsnet2010-${MY_BUILD_TYPE}.lib ${EXTRA_LINK_LIBRARIES}) 
ELSE()
	target_link_libraries(runTests UnitTest++ ${EXTRA_LINK_LIBRARIES})
ENDIF()

ADD_CUSTOM_COMMAND(
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	TARGET runTests
	POST_BUILD
	COMMAND ./runTests.${SCRIPT_EXT} ${MY_BUILD_TYPE} ${ARCH_DIR}
	)
