cmake_minimum_required(VERSION 3.28.1)

project(TINY_WEB_SERVER VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(CMAKE_PREFIX_PATH )

# find_package()

#include_directories(./include)

#file(GLOB HEADERS ./include/**/*.hpp)
file(GLOB_RECURSE SOURCES ./src/**/*.cpp)

add_executable(TinyWebServer main.cpp ${SOURCES} ${HEADERS})

# target_link_libraries(MyTestExecutable PRIVATE ${Boost_LIBRARIES})
