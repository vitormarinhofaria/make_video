if(CMAKE_GENERATOR MATCHES "Visual Studio")
    message(FATAL_ERROR "Visual Studio generator not supported, use: cmake -G Ninja")
endif()

set(CMAKE_C_COMPILER "zig" cc)
set(CMAKE_CXX_COMPILER "zig" c++)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL  "Windows")
set(CMAKE_RC_COMPILER "llvm-rc")
endif()
