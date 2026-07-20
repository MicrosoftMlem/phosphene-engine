#pragma once

//probably move to AABB.h idk

inline bool hasLineOfSight(const glm::vec3& from, const glm::vec3& to, const std::vector<AABB>& colliders) {
    glm::vec3 delta = to - from; //the vector pointing from 'from' to 'to'
    float distance = glm::length(delta); // its length
    if (distance < 1e-6f) return true; // if the dist is basically 0.

    glm::vec3 direction = delta / distance; //dividing a vector by its length just normalises it

    for (const AABB& box : colliders) {
        float tHit = FLT_MAX;
        if (rayIntersectsAABB(from, direction, box, tHit) && tHit < distance) {
            return false; //somethings in the way
        }
    }
    return true; //hit nothing
}