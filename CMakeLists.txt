cmake_minimum_required(VERSION 2.8)
PROJECT(SodaPopStation CXX)
SET(SodaPopStation_VERSION_MAJOR 0)
SET(SodaPopStation_VERSION_MINOR 1)
SET(CMAKE_CXX_COMPILER g++)
SET(CMAKE_CXX_FLAGS_DEBUG "-g")

#FIND_PACKAGE(FindZLIB)
SET(ZLIB_LIBRARIES "-lz")


execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3)
        message(STATUS "Version >= 4.3")
endif()

FIND_PACKAGE(wxWidgets REQUIRED xml adv core base)
if (wxWidgets_FOUND)
  execute_process(COMMAND  wx-config --cxxflags
                   OUTPUT_VARIABLE  wxWidgets_CXX_FLAGS)
  STRING(STRIP ${wxWidgets_CXX_FLAGS} wxWidgets_CXX_FLAGS)
  SET(CMAKE_CXX_FLAGS "-std=c++0x ${wxWidgets_CXX_FLAGS}")
  ADD_EXECUTABLE(SodaPopStation my_popstation.cpp gameData.cpp popstrip.cpp popstation.cpp popstationGUI.cpp config.cpp popstationGUI.h gameData.h header.h config.h data.h popstrip.h)
	TARGET_LINK_LIBRARIES(SodaPopStation ${ZLIB_LIBRARIES} ${wxWidgets_LIBRARIES})
else()
   MESSAGE("wxWidgets not found!")
endif()
