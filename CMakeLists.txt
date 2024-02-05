cmake_minimum_required(VERSION 3.12)
project(test_engine)
# Include all the header files for glad, glfw and others

# Add the executable
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(engine ${SOURCES})

# Find the opengl library
find_package(OpenGL REQUIRED)

if(${OPENGL_FOUND})

    target_include_directories(engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(engine PRIVATE ${OPENGL_LIBRARIES} ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3.lib)

else()

    message("OPENGL Not found")

endif()

