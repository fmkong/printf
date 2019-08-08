set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.7)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER g++)
set(AS as)
set(AR ar)
set(OBJCOPY objcopy)
set(OBJDUMP objdump)
set(SIZE size)

set(CMAKE_CXX_FLAGS_INIT "-std=gnu++11")
set(CMAKE_C_FLAGS_INIT "-std=gnu11")

project(hummingbird C )
set(CMAKE_CXX_STANDARD 11)

include_directories(.)

link_libraries(m)

add_executable(
        ${PROJECT_NAME}.out
        printf.c
        test.c
)


