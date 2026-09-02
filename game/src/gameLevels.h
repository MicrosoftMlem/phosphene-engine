#pragma once

#include <vector>
#include <string>

// this should be free of rendering code since the server includes this

// this is hardcoded. as long as client and server have identical lists,
// it should work. We need to add verification later (will be easy)
// path is relative to game/src/

//inline so that it doesnt cause an ODR violation
const inline std::vector<std::string> levelList = {"test1v1level.level.json"};
