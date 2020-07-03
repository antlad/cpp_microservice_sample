cmake_minimum_required(VERSION 3.12)

project(service_sample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_TESTS "enable tests" ON)

add_definitions(-DSPDLOG_FMT_EXTERNAL)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


find_package(restinio CONFIG REQUIRED)

include_directories(
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/include"
        "${CMAKE_CURRENT_LIST_DIR}/src"
        "${CMAKE_CURRENT_LIST_DIR}/common"
)

set(service_sample_core_SRC
        src/defs.hpp
        src/pool/asynctask.hpp
        src/pool/asynctask.cpp
        src/pool/pool.hpp
        src/pool/pool.cpp

       )

set(service_sample_SRCS
        src/main.cpp
        src/logger.hpp
        src/rest.cpp
        src/rest.hpp
        )
add_executable(service_sample ${service_sample_SRCS} ${service_sample_core_SRC})
target_link_libraries(service_sample ${CONAN_LIBS})


if (ENABLE_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
    set(testing_SRCS
            tests/check1.cpp
            ${service_sample_core_SRC}
            )

    add_executable(default_test_suite ${testing_SRCS} ${service_sample_core_SRC})

    target_link_libraries(default_test_suite GTest::GTest GTest::Main ${CONAN_LIBS})
    add_test(DefaultTestSuite default_test_suite)
endif ()


