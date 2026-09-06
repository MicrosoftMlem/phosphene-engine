cmake_minimum_required(VERSION 3.16)
project(phosphene_engine)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#find package finds the packaged i have installed (with yay). cmake needs their CMake config files.
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
find_package(unofficial-enet CONFIG QUIET)


#build glad as a small static library
add_library(glad STATIC vendor/glad/src/glad.c) #static == baked into final exe
target_include_directories(glad PUBLIC vendor/glad/include) #public == anything that links glad also gets this include path


# ok then have miniaudio as a small static library:
add_library(ma STATIC vendor/miniaudio/miniaudio.c)
target_include_directories(ma PUBLIC vendor/miniaudio)
# miniaudio also wants:
target_link_libraries(ma PUBLIC pthread m dl)

# engine:
add_library(engine STATIC engine/src/Shader.cpp engine/src/Texture.cpp engine/src/Camera.cpp engine/src/Mesh.cpp
                            engine/src/GameObject.cpp engine/src/Simulation.cpp engine/src/Level.cpp engine/src/OBJLoader.cpp
                            engine/src/UIRenderer.cpp engine/src/Bot.cpp engine/src/AudioInstance.cpp) #compile engine files into libengine.a


target_include_directories(engine PUBLIC engine/include vendor) #public == anything that links engine also gets this include path // make libs in vendor be included in engine
target_link_libraries(engine PUBLIC glad ma) #engine needs glad

#final step:

# 'game' is an executable build from main.cpp
add_executable(game game/src/main.cpp game/src/Pistol.cpp game/src/PlayingState.cpp game/src/ButterflyKnife.cpp
               game/src/TrafficLight.cpp game/src/TrafficLightEntity.cpp game/src/Server.cpp game/src/NetworkClient.cpp
               game/src/RoundItems.cpp game/src/StateManager.cpp game/src/MenuState.cpp game/src/textureStore.cpp 
	       game/src/shaderStore.cpp game/src/FootstepSystem.cpp)
# connect the game to everything it needs
target_link_libraries(game PRIVATE engine glfw OpenGL::GL glm::glm enet ma) #the game needs: glad (GL loader), Open::GL (the actual GL lib),
# glm::glm (math)
# engine (for engine). engine includes glad so glad will be linked aswell
# enet for networking
# private == the game uses these but doesn't 're-expose' them
