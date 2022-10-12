CMAKE_MINIMUM_REQUIRED(VERSION 3.18)

project(MakeVideo CXX)
set(CMAKE_CXX_STANDARD 20)
#link_directories(dependencies)

add_subdirectory(subprojects/glfw)
add_subdirectory(subprojects/libwebm)
#add_subdirectory(subprojects/libvpx)
add_executable(${PROJECT_NAME} src/main.cpp src/shader.hpp src/pch.hpp src/pch.cpp src/stb_image_writer.h src/stb_image.h src/glad.h)

target_link_libraries(${PROJECT_NAME} glfw wemb)
target_include_directories(${PROJECT_NAME} PRIVATE dependencies/include subprojects/glm subprojects/libwebm subprojects/libvpx)