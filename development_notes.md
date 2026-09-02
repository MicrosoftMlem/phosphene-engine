
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

** TODOS:

*** TODO Make player 'freeze' effect, be also considered by the client.
*** TODO Implement a material class and refactor to use it
*** TODO Go over the UIRenderer class, and then expand it
*** TODO Implement text in the UI
*** TODO Implement IDE tool for going over 'ISSUE' comments
*** TODO Sync server and client level loading, - both can load different levels (desync)
*** TODO Randomise player spawn locations
*** TODO Plan the round win phase
*** TODO Plan the match win phase
*** TODO Fix WorldEntity class so we can access the inheriting classes methods (Server.cpp:216)
