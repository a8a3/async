cmake_minimum_required(VERSION 3.2)

if(DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(async VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
    project(async VERSION 0.0.1)
endif()

find_package(Threads REQUIRED)

add_library(async SHARED async/async.cpp)

set_target_properties(async PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

if (MSVC)
    target_compile_options(async PRIVATE
        /W4 -O3
    )
else ()
    target_compile_options(async PRIVATE
        -Wall -Wextra -pedantic -Werror -O3
    )
endif()

add_executable(async_cli main.cpp)
target_include_directories(async_cli PRIVATE "./async")
target_link_libraries(async_cli async)

add_executable(async_test async_test.cpp)
target_link_libraries(async_test PRIVATE Threads::Threads)

install(TARGETS async LIBRARY DESTINATION lib)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT async@mail.ru)

include(CPack)
