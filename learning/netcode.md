
ENetHost - the server

ENetPeer - a connection to someone else. server has 2 (1 to each client), clients have 1 (to server)

enet_host_service(host, &event, timeoutMs) - run every frame and is what runs the server. returns something other than 0
                                             if something happens, and fills event with the type.

ENetPacket - the bytes of the packet + a flag (such as ENET_PACKET_FLAG_RELIABLE)


we cant send GameState over packets bc it has pointers which will mean nothing on other computers, and things like vectors
which isnt fixed size.
So we have snapshot.h which is a packet safe version