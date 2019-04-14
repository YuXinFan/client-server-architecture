cmake_minimum_required(VERSION 3.5)
project(cs133_hw4)

set(CS133_HW4_VERSION_MAJOR 1)
set(CS133_HW4_VERSION_MINOR 0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

find_package(Threads REQUIRED)

FUNCTION(setup_gtest)
    execute_process(
            COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download
    )
    if (result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif ()

    execute_process(
            COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download
    )
    if (result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif ()

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    add_subdirectory(
            ${CMAKE_CURRENT_BINARY_DIR}/googletest-src
            ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
            EXCLUDE_FROM_ALL
    )
ENDFUNCTION()

configure_file(cmake/get_gtest.cmake.in googletest-download/CMakeLists.txt)
setup_gtest()

include_directories("${PROJECT_SOURCE_DIR}/include")

set(CS133_HW4_LIB ${PROJECT_NAME})
set(CS133_HW4_UTEST_EXE utest_${PROJECT_NAME})
set(CS133_HW4_SYSTEST_EXE systest_${PROJECT_NAME})

set(CS133_HW4_SRC
        ${PROJECT_SOURCE_DIR}/src/ClientID.cpp
        ${PROJECT_SOURCE_DIR}/src/JobID.cpp
        ${PROJECT_SOURCE_DIR}/src/JobInput.cpp
        ${PROJECT_SOURCE_DIR}/src/JobOutput.cpp
        ${PROJECT_SOURCE_DIR}/src/Socket.cpp
        ${PROJECT_SOURCE_DIR}/src/ServerThread.cpp
        ${PROJECT_SOURCE_DIR}/src/Server.cpp)

set(CS133_HW4_UTEST_SRC
        ${PROJECT_SOURCE_DIR}/test/test_id.cpp
        ${PROJECT_SOURCE_DIR}/test/test_queue.cpp
        ${PROJECT_SOURCE_DIR}/test/test_socket.cpp
        ${PROJECT_SOURCE_DIR}/test/test_job_io.cpp
        ${PROJECT_SOURCE_DIR}/test/test_server_thread.cpp
        ${PROJECT_SOURCE_DIR}/test/test_server.cpp)

set(CS133_HW4_SYSTEST_SRC
        ${PROJECT_SOURCE_DIR}/test/test_system.cpp)

add_library(${CS133_HW4_LIB} ${CS133_HW4_SRC})

add_executable(${CS133_HW4_UTEST_EXE} ${CS133_HW4_UTEST_SRC})
target_link_libraries(${CS133_HW4_UTEST_EXE} ${CS133_HW4_LIB} gtest_main Threads::Threads)

add_executable(${CS133_HW4_SYSTEST_EXE} ${CS133_HW4_SYSTEST_SRC})
target_link_libraries(${CS133_HW4_SYSTEST_EXE} ${CS133_HW4_LIB} Threads::Threads)
