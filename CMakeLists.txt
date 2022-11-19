cmake_minimum_required(VERSION 3.5)

project(EamonInterpreter VERSION 0.3.0 LANGUAGES CXX)

if(CMAKE_SOURCE_DIR MATCHES CMAKE_BINARY_DIR)
      message(STATUS "Please do an out-of-tree build:")
      message(STATUS "rm -f CMakeCache.txt && mkdir build && cd build; cmake .. && make")
      message(FATAL_ERROR "In-tree-build detected!")
endif()

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE OFF)

#-----------------------------------------------------------------------------
# OPTIONS
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# build options
#-----------------------------------------------------------------------------
#option(BUILD_SHARED_LIBS "Build shared libraries" ON)
#option(BUILD_TESTS "Build tests" ON)

#-----------------------------------------------------------------------------
# DEPENDENCIES
#-----------------------------------------------------------------------------

# Specify search path for CMake modules to be loaded by include() and find_package()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# load the package to obtain information from pkgconfig
# ensure that packages etc. in /usr/local are found
# do NOT use CMAKE_PREFIX_PATH as it is used by qtcreator to select the Qt version
set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig:/usr/local/lib64/pkgconfig")
find_package(PkgConfig REQUIRED)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

find_package(Qt5 COMPONENTS Widgets Xml REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# CMake config files
include(CMakePackageConfigHelpers)

#-----------------------------------------------------------------------------
# COMPILE DEFINITIONS
#-----------------------------------------------------------------------------

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(EAMON_DEF)
list(APPEND EAMON_DEF -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND EAMON_DEF -DEAMON_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
list(APPEND EAMON_DEF -DEAMON_VERSION_MINOR=${PROJECT_VERSION_MINOR})
list(APPEND EAMON_DEF -DEAMON_VERSION_PATCH=${PROJECT_VERSION_PATCH})

#-----------------------------------------------------------------------------
# INSTALL
#-----------------------------------------------------------------------------

include(GNUInstallDirs)

# keep rpath to actual Qt framework in executable
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)



#-----------------------------------------------------------------------------
# BUILD ALL COMPONENTS
#-----------------------------------------------------------------------------

add_subdirectory(src/eamon)



#-----------------------------------------------------------------------------
# PACKAGE GENERATION
#-----------------------------------------------------------------------------



