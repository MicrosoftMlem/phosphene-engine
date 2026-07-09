the depth buffer (aka z-buffer) is a single image that goes over the entire screen, and for each pixel records
how far away the thing currently drawn there is.

Depth testing is:

before the GPU is about to draw a pixel, it compares the pixels depth to whats already stored. If the new pixel is closer
then it draws it and updates the stored depth. If its farther away then it throws it away. So it wont draw things that should
be behind something, infront.