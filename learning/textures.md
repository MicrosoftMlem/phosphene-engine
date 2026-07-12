READ AS RAW TEXT NOT MD

THIS NEEDS BETTER NOTES ON THE ACTUAL PROCESS OF LOADING A TEXTURE IN

The gpu needs to know which part of an image goes on which part of a surface (when texturing an object)
That mapping is done with:

UV coordinates: each vertex gets, alongside its 3D position, a 2D coordinate which says what pixel of the image does it correspond
to.

UV space runs 0 - 1 on each axis:

(0,0) bottom left of the image
(1,1) top right of the image
(0.5, 0.5) centre of the image

The GPU interpolates between the UV coords across a triangle so the whole image stretches smoothly across
the face. I would define UV's per vertex and the GPU stretches the image between the surface between them.


To generate a placeholder checker texture:

magick -size 512x512 pattern:checkerboard -auto-level -colorspace sRGB +level-colors "#9BC9D0","#F0EDE6" checker.png

and just change the hexcodes for the colour


GL doesnt understand pngs etc so we use stb_image to convert pngs to raw RGB(A) hex.


Mipmaps are pre-shrunk versions of textures, kinda like LODS that stop shimmering/antialiasing when viewing a texture
from far away

To add UV coords we have to add it to the vertex data and then update the VAO layout.
A vertex already has 3 floats, x/y/z
so we add 2UV floats per vertex (x/y)

So for a face, the vertex thats visually bottom left gets UV coord (0,0) etc

If all shapes follow the same A, B, C, D pattern we can follow the rule:

A = 0,0
B = 1,0
C = 1,1
then triangle 2:
A = 0,0
C = 1,1
D = 0,1