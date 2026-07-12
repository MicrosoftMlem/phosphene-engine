cmake_minimum_required(VERSION 3.16)
project(phosphene_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#find package finds the packaged i have installed (with yay). cmake needs their CMake config files.
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)


#build glad as a small static library
add_library(glad STATIC vendor/glad/src/glad.c) #static == baked into final exe
target_include_directories(glad PUBLIC vendor/glad/include) #public == anything that links glad also gets this include path


# engine:
add_library(engine STATIC engine/src/Shader.cpp engine/src/Texture.cpp engine/src/Camera.cpp) #compile engine files into libengine.a
target_include_directories(engine PUBLIC engine/include vendor) #public == anything that links engine also gets this include path // make libs in vendor be included in engine
target_link_libraries(engine PUBLIC glad) #engine needs glad

#final step:

# 'game' is an executable build from main.cpp
add_executable(game game/src/main.cpp)
# connect the game to everything it needs
target_link_libraries(game PRIVATE engine glfw OpenGL::GL glm::glm) #the game needs: glad (GL loader), Open::GL (the actual GL lib),
                                                                        # glm::glm (math)
                                                                        # engine (for engine). engine includes glad so glad will be linked aswell
                                                                        # private == the game uses these but doesn't 're-expose' them