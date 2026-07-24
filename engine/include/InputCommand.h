#pragma once
#include <glm/glm.hpp>

struct InputCommand {
    unsigned int sequence = 0; // 0 means 'empty slot'

    //movement intent:
    bool moveForward = false;
    bool moveBack = false;
    bool moveLeft = false;
    bool moveRight = false;
    bool jump = false;
    glm::vec3 lookDirection = glm::vec3(0.0f, 0.0f, -1.0f);

    bool equipWeapon = false;
    bool equipAbility = false;
    bool primaryPressed = false; //if it was just clicked this frame
    bool primaryHeld = false; //if its held in general
    bool secondaryPressed = false;
    bool secondaryHeld = false;
    bool dash = false; //edge triggered
    bool crouch = false;
};