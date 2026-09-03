
* Notes to help remember stuff about the codebase

** Client-side vs Server-side logic

*** Game Simulation

Simulation.cpp is the server-side simulation
PlayingState.cpp is the client-side simulation

*** Game States:

StateManager.h has an enum named Pending, which stores
all the states that the game can transition to (which
should be all the states in the game).

StateManager.h also stores the NetworkClient network
variable which means that the network will persist
between state swaps.

** Unclear code:

- StateManager::PerformTransition() needs documentation

** Current notes:

GameObject.cpp/h currently correctly uses material class.
Im looking for any places that don't use material class but should.
GameObject.draw() now uses stored Shader in material instead of arg

Added textureStore.cpp/h which registers textures on init and has the
getTexture method.

No classes should have a Texture member now as they should pull from
the textureStore.

levelMaterial.texture is used in PlayingState.cpp (replacing Texture
texture)

However, while levelMaterial.shader is used, it seg faults. We do
levelMaterial.shader = &tempShader which is in the member
initialisation. We need to replace that with a shaderStore at some
point.

GameObject.cpp has both a material, and takes a shader as an arg. I am
removing that shader arg as the segfault could be caused by it
somehow.

Level.cpp has a material, but never asks for or sets the materials
shader so it just stays as a nullptr. this is the likely cause of the
segfault. i am making it so you must supply a shader when running
loadLevel.

Server gives loadLevel nullptrs for the mesh, texture args.
However loadlevel doesnt seem to handle this? I havent tested yet but
ive added a nullptr for the new shader arg, but im assuming it will
crash.

There is now a shaderStore that PlayingState now uses

** TODOS:

*** TODO Make player 'freeze' effect, be also considered by the client.
*** TODO Implement a material class and refactor to use it
*** TODO Go over the UIRenderer class, and then expand it
*** TODO Implement text in the UI
*** TODO Implement IDE tool for going over 'ISSUE' comments
*** TODO Randomise player spawn locations
*** TODO Plan the round win phase
*** TODO Plan the match win phase
*** TODO Implement the 'Transition' RoundPhase
*** TODO Fix WorldEntity class so we can access the inheriting classes methods (Server.cpp:216)
*** TODO Implement disconnect handling

*** TODO Add player items to the snapshot

** Fixes:

The numerical code at the end is for my own reference for personal documentation

This list/section has been replaced for the CHANGELOG and might contain outdated info

- Updated default player health const to 120 to match ui and round health - 14.5
- Made client read snap.phase after match end  - 14.1
- Removed connectToServer call in main, as StateManager handles that already - 14.7
- Updated GameState.h's dashCharges from 3 to 2 to match Simulation. - 14.6
- Made server tell client what level to load, and added gameLevels.h which
  keeps a vector of level paths. Server.cpp picks a level randomly,
  and sends that level to clients through the welcome packet. - 14.3
