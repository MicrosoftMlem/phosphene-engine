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