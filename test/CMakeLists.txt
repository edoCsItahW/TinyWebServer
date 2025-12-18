cmake_minimum_required(VERSION 3.28.1)

project(TEST_TINY_WEB_SERVER VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(CMAKE_PREFIX_PATH )

# find_package()

include_directories(../include)
# link_directories()

file(GLOB_RECURSE SOURCES ../src/*.cpp ../src/**/*.cpp)

add_executable(TestTinyWebServer test_socket.cpp ${SOURCES})

# target_link_libraries(MyTestExecutable PRIVATE ${Boost_LIBRARIES})
