#include "Player.h"

Player::Player(glm::vec3 startPos) {
    position = startPos;
    velocity = glm::vec3(0.0f);
    moveSpeed = 8.0f;
    gravity = -20.0f;
    jumpStrength = 8.0f;
    grounded = false;
    groundAccel = 12.0f;
    airAccel = 2.0f;
}

void Player::update(GLFWwindow* window, Camera& camera, const std::vector<AABB>& colliders, float deltaTime) {
    handleInput(window, camera, deltaTime);

    velocity.y += gravity * deltaTime; //we do gravity here now
    
    resolveCollisions(colliders, deltaTime); //actually moves position while resolving collisions

    camera.position = position; // camera follows player
}


void Player::handleInput(GLFWwindow* window, Camera& camera, float deltaTime) {
    glm::vec3 flatFront = camera.front; //flatfront is forward dir WITHOUT up/down tilt
    flatFront.y = 0.0f; //remove like up/down tilt
    flatFront = glm::normalize(flatFront);
    glm::vec3 flatRight = glm::normalize(glm::cross(flatFront, glm::vec3(0, 1, 0))); //get flatRight from cross product

    glm::vec3 moveDir = glm::vec3(0.0f); //a vector of our movement input (will be normalized)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){ //polls if W is held down
        moveDir += flatFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        moveDir -= flatFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        moveDir -= flatRight;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        moveDir += flatRight;
    }

    if (glm::length(moveDir) > 0.0f) { //if there is movement input
        moveDir = glm::normalize(moveDir);
    }
    
    //where we want horizontal velocity to be
    glm::vec3 targetVel = moveDir * moveSpeed;

    //how fast we reach that target
    float accel = grounded ? groundAccel : airAccel; //if grounded, accell = groundedAccell, else accel = airAccel
    float t = accel * deltaTime;
    if (t > 1.0f) { //clamp it to 1.0
        t = 1.0f; //how far towards target we move this frame
    }

    velocity.x = glm::mix(velocity.x, targetVel.x, t); //like lerp
    velocity.z = glm::mix(velocity.z, targetVel.z, t);


    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && grounded) { //jump
        velocity.y = jumpStrength;
    }
}


AABB Player::getAABB() {
    AABB box; //the collision box for player
    //player is 1.8 tall and width of 0.6
    box.min = position + glm::vec3(-0.3f, 0.0f, -0.3f); //bottom corner of box
    box.max = position + glm::vec3(0.3f, 1.8f, 0.3f); //top corner of box
    //position is the players feet.
    return box;
}

void Player::resolveCollisions(const std::vector<AABB>& colliders, float deltaTime) {
    position += velocity * deltaTime; //move on all axis first

    grounded = false; //assume airborne

    for (const AABB& c : colliders) { //for each collider
        AABB box = getAABB(); //get our players AABB collider
        if (!aabbOverlap(box, c)) {
            continue; //no overlap so this this in the for loop
        }

        //get the depth of penetration on each axis, positive means its penetrating
        float px = glm::min(box.max.x, c.max.x) - glm::max(box.min.x, c.min.x);
        float py = glm::min(box.max.y, c.max.y) - glm::max(box.min.y, c.min.y);
        float pz = glm::min(box.max.z, c.max.z) - glm::max(box.min.z, c.min.z);


        //get the collder centres to decide the push direction
        glm::vec3 boxCentre = (box.min + box.max) * 0.5f; //get our players AABB box centre
        glm::vec3 cCentre = (c.min + c.max) * 0.5f; //get the colliders AABB box centre

        if (px < py && px < pz) { //if x is the shallowest penetration
            if (boxCentre.x < cCentre.x) { //player is on -X side
                position.x -= px;
            }
            else { //player is on +X side
                position.x += px;
            }
            velocity.x = 0.0f;
        }
        else if (py < pz) { //if Y is shallowest
            if (boxCentre.y < cCentre.y) { //player is below so push down
                position.y -= py;
            }
            else {
                position.y += py; //the player is grounded so much up
                grounded = true;
            }
            velocity.y = 0.0f;
        }
        else { //z is the shallowest
            if (boxCentre.z < cCentre.z) {
                position.z -= pz;
            }
            else {
                position.z += pz;
            }
            velocity.z = 0.0f;
        }
    }
}