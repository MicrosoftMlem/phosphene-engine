#pragma once
#include <glm/glm.hpp>
#include <cfloat>

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


//origin = where ray starts
//tHit = a reference which, if the raycast hits, the function will set to the distance away.
//if it hit is returned
inline bool rayIntersectsAABB(const glm::vec3& origin, const glm::vec3& direction, const AABB& box, float & tHit) {
    float tMin = 0.0f; //min and max are the 2 planes of the slab on this axis. so eg on x, its a boxes left and right faces.
    float tMax = FLT_MAX;

    for (int axis = 0; axis < 3; axis++) { //0 = x, 1 = y, 2 = z, glm supports indexing for axis

        if (fabs(direction[axis]) < 1e-6f) {
            //means ray doesnt move along axis at all (like perfectly straight in a specific direcition)
            if (origin[axis] < box.min[axis] || origin[axis] > box.max[axis]) {
                return false; //outside this slab so it can never get in anyway so just return false.
            }
            continue; //means we already know its in this slab so skip the rest of the checking for this axis
        }

        float t1 = (box.min[axis] - origin[axis]) / direction[axis]; // how far along the ray do i have to travel to reach the plane (t1 is the min plane)
        float t2 = (box.max[axis] - origin[axis]) / direction[axis]; //how far along to reach the max plane (t2)

        if (t1 > t2) {
            std::swap(t1, t2); //it goes in negative direction so we just swap them
        }

        tMin = glm::max(tMin, t1);
        tMax = glm::min(tMax, t2);

        if (tMin > tMax) { return false; }
    }

    tHit = tMin;
    return true;
}
