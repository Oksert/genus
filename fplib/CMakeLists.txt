cmake_minimum_required(VERSION 2.6)

set(SRC src/fptree.cpp src/fptreealg.cpp src/fptreealgfactory.cpp src/headertable.cpp src/headertablevalue.cpp src/fpnode.cpp src/prefixnode.cpp src/supportgreater.cpp src/tfilereader.cpp)

include_directories(include)

add_library(fp STATIC ${SRC})
