READ AS RAW TEXT NOT MD

so a matrix is used to describe and apply rotations and transformations.
if u multiply a vector by a matrix it will apply the transformation to the vector.

so if i have an object with a vec3 position: (x, y, z), a transformation will produce
three more numbers, (x', y', z')

A key note is to remember that each new number (the ones i did above with a ' ) is built from a mix of the
old ones. The 'mixing' is the whole game.
For example we can scale a vector by 2:

x' = 2*x + 0*y + 0*z
y' = 0*x + 2*y + 0*z
z' = 0*x + 0*y + 2*z

Each output is a weighted combination of all three inputs. So in the example we just weighted the
same axis for each axis so that we're just multiplying the original vector by 2.
The weights are the transformation
So to properly make a matrix we would put it in a grid:

      from x   from y   from z
x' is   [2       0       0]
y' is   [0       2       0]
z' is   [0       0       2]

or properly written without the helper notes:

[2  0  0]
[0  2  0]
[0  0  2]

As you can see in a matrix we only have the weights.
So in a matrix we are mapping every input to every output

To multiply matrix's:

              matrix      vector     calculation   result
row for x': [2, 0, 0] * (x, y, z) = 2x + 0y + 0z = x'
row for y': [0, 2, 0] * (x, y, z) = 0x + 2y + 0z = y'
row for z': [0, 0, 2] * (x, y, z) = 0x + 0y + 2z = z'

Matrix Rotation:

A rotation uses the same system but different weights. To rotate around the Z axis by angle θ:
Remember θ is just a common variable for unknown angles in mathmatics. It doesnt have a special meaning

[ cos θ   -sin θ   0 ]
[ sin θ    cos θ   0 ]
[  0        0      1 ]

The top row:   x' =  cos θ * x  -  sin θ * y
So thinking back to rotating around Z, we know that that would change both the X and Y positions, which
you can see is exactly whats done there. The sin and cos are the weights that produce rotation and
glm::rotate does those automatically so we dont have to write that

In 3D:

We will use gld::mat4, a 4x4 grid not 3x3. This is because in 3D we can't move positions
with multiplication, we need addition for movement, and the current 3x3 grids only support weights which apply multiplication.
So we have a forth row/column as a 'mathmatical trick' that allows the matrix to do addition alongside the multiplication,
so that one 4x4 grid can rotate, scale and move all at once. The 4th coordinate is 'w',
which we've seen in basic.vert as vec4(aPos, 1.0) (1.0 is w).
I don't know the actual mechanism on how it works and is processed but all I need to know is that its 4x4 so we can also move.

To combine matrixes:

Matrixes are supposed to be used with one matrix doing rotration, scale and movement.
The way we do that is we make 3 matrixes first:
S - Scale matrix
R - rotate matrix
T - translate matrix

then we just do:

M = S * R * T

to get M, the final transformation matrix.
This math is done for use, but the process is:
T * (R * (S * v))
with v being the vector.
This is because we Scale it first, then we rotate it and finally move it

Its important that the order is right:
T * R is translate then rotate
R * T is rotate then translate

This is because ITS READ RIGHT TO LEFT and this is important to remember to make things work how you want

The first will:
move away, then rotate around the ORIGINAL origin, in a big arc
the second will:
rotate in the spot (as its on its origin rn) then move away
So doing the wrong one WILL cause unexpected behaviour.

How movement actually works:

w is NOT where the translation is stored.
w is a 'component of the vector' (as the vector is (x, y, z, w)) and for a position w = 1
The actual translation lives in the matrix, in the 4th column

A matrix for:  +5 on x, +2 on y, +0 on z

[1  0  0  5]
[0  1  0  2]
[0  0  1  0]
[0  0  0  1]

The top left 3x3 is 'dont rotate or scale', a identity
The way it shows dont rotate or scale is:
on the x' row (the first one) we apply 1 to x, which will leave x as it is (so wont scale or rotate) and then
we do 0 to y and z because we dont want to mix those into x, bc we are leaving it as it is.

The forth column (5, 2, 0) is the translation

And the 4th row is for w mechanism stuff
So its almost always [0  0  0  1]
but then can be changed for depth and stuff
But doing stuff like [0  1  0  1] will warp shapes and a bit odd.
it should usually stay [0  0  0  1] so stuff doesnt get warped, and w = w' because you dont really want that to change (see at bottom of file)

So the calculation:

x' = 1*x + 0*y + 0*z + 5*1 = x + 5 (so you can see the translation applied) (the 5*1 is same as 5*w)
y' = 0*x + 1*y + 0*z + 2*1 = x + 2
z' = 0*x + 0*y + 1*z + 0*1 = z



So we do translation by multipling the value in the forth column by W (1) which equals the same as the value, so when we add it to x'
we have managed to do addition while also doing multiplication. if w was eg 2 this would all break and as far as im aware we never do that

Added note:

w=1 means this is a position, translate it
w=0 means its a direction and directions dont move they just rotate
Setting w=0 multiplies the 4th column by 0 and obviously it then just dissappears, so directions ignore translations automatically

W isnt like a switch
When a vector is made you decide if it represents a postion, or a direction (w=1 or w=0) and then w never changes

MVP matrix concept:

Model - places/rotates/scales an object in the world
View - The camera. Moves the world so the camera is at the origin. (needs clarification)
Projection - perspective, distant = smaller etc
