#pragma once
#include <glm/glm.hpp>

struct AABB { //a struct bc its just data - no behaviour (func etc)
    //struct defaults to public
    glm::vec3 min; //one corner of box
    glm::vec3 max; //other corner of box
};


//inline says: since this is a .h, it could be included in multiple files.
// so it tells the compiler not to complain if theres duplicate definitions

inline bool aabbOverlap(const AABB& a, const AABB& b) { //& to avoid copying the struct (so we get reference) and const = we only read it, we dont change it
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) && //if x is colliding
           (a.min.y <= b.max.y && a.max.y >= b.min.y) && //if y is colliding
           (a.min.z <= b.max.z && a.max.z >= b.min.z); //if z is colliding
}