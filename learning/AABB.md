AABB is axis-aligning bounding boxes.

it can detect collision when two surfaces that are aligned on their axis collide

we then use the penetration depth to figure out which axis was collided with

the axis with the least penetration is the collided face.
the reason why its least is bc:
- lets say we're on a floor, colliding on its top face (y axis)
- we are deep in the x and z bc we're eg in the centre of the horizontal plane
- however we are not far in y (bc we only just touched it)

so y is what we've colliding with