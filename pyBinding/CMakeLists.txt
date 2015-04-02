cmake_minimum_required(VERSION 2.6)

set(SRC  src/pyBinding.cpp src/Context.cpp src/Logger.cpp)

include(FindPythonLibs)
#find_path(PYTHON_PATH Python.h PATH_SUFFIXES python2.5)
find_path(ICONV_PATH iconv.h)
find_path(BDB_PATH db_cxx.h PATH_SUFFIXES db42)
include(FindPkgConfig)
#pkg_search_module(BDB db42)
find_path(CPPUNIT_PATH cppunit/extensions/HelperMacros.h)
	
#find_library(PYTHON_LIB python2.5 PATH_SUFFIXES python2.5 python25)
#find_library(PYTHON_LIB python25 PATH_SUFFIXES python2.5 python25)
find_path(BOOST_PATH boost/asio.hpp)
find_library(BOOST_PYTHON_LIB boost_python)
find_library(CPPUNIT_LIB cppunit)
find_library(ICONV_LIB icobv)

include_directories(../classifier/include ../frequencymeasure/include ../parser/src include ../fplib/include ${PYTHON_INCLUDE_DIRS} ${ICONV_PATH} ${BDB_PATH} ${CPPUNIT_PATH}  ${BOOST_PATH})

add_library(CMODULE SHARED ${SRC})
set_target_properties(CMODULE PROPERTIES 
                                OUTPUT_NAME "CModule"
                                LIBRARY_OUTPUT_DIRECTORY "../python"
                                PREFIX "")


target_link_libraries(CMODULE frequencymeasure classifier parser fp ${PYTHON_LIBRARIES} ${BOOST_PYTHON_LIB} ${CPPUNIT_LIB} ${ICONV_LIB})
