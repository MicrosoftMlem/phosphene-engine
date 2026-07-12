READ AS RAW TEXT NOT MD

When making a cube, you can't just do 4 vertices per face.
A face of a cube is a square, and a square is actually 2 triangles.
I call them triangle A and triangle B
Triangle A should have vertices A, B, C
and Trangle B should have vertices A, C, D
For A, B, C and D you can pick what vertices they correspond do, but Triangle B must have A, B and a vertex
Triangle A doesnt have, which is D. This makes lots of sense if you look at a diagram.
Also you have to keep in mind 'winding'.
The order that you declare vertices matter as it is what makes the GPU decide what direction a face is facing.
So A, C, D and A, D, C are geometrically identical but will face different directions.

Heres a cube (defined in main.cpp):

    float vertices[] = { //cube vertices in local space (the cubes shape defined around its own origin)

        // BACK
        -0.5, -0.5, -0.5, //back triangle A
        0.5, -0.5, -0.5,
        0.5, 0.5, -0.5,

        -0.5, -0.5, -0.5, //back triangle B
        0.5, 0.5, -0.5,
        -0.5, 0.5, -0.5,

        // FRONT
        -0.5, -0.5, 0.5, //front trangle A
        0.5, -0.5, 0.5,
        0.5, 0.5, 0.5,

        -0.5, -0.5, 0.5, //front trangle B
        0.5, 0.5, 0.5,
        -0.5, 0.5, 0.5,

        // LEFT
        -0.5, -0.5, -0.5, //A //left triangle A
        -0.5, -0.5, 0.5,  //B
        -0.5, 0.5, 0.5,   //C

        -0.5, -0.5, -0.5, //A //left triangle B
        -0.5, 0.5, 0.5,   //C
        -0.5, 0.5, -0.5,  //D
        
        // RIGHT
        0.5, -0.5, -0.5, //A //right triangle A
        0.5, -0.5, 0.5,  //B
        0.5, 0.5, 0.5,   //C

        0.5, -0.5, -0.5, //A //right triangle B
        0.5, 0.5, 0.5,   //C
        0.5, 0.5, -0.5,  //D

        // BOTTOM
        -0.5, -0.5, -0.5, //A //bottom triangle A
        -0.5, -0.5, 0.5,  //B
        0.5, -0.5, 0.5,   //C

        -0.5, -0.5, -0.5, //A //bottom triangle B
        0.5, -0.5, 0.5,   //C
        0.5, -0.5, -0.5,  //D

        //TOP
        -0.5, 0.5, -0.5, //top triangle A
        -0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,

        -0.5, 0.5, -0.5, //top triangle B
        -0.5, 0.5, 0.5,
        0.5, 0.5, -0.5,
    };