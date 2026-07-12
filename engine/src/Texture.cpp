#define STB_IMAGE_IMPLEMENTATION //needed in only 1 cpp file to make STB work
#include <stb/stb_image.h>
#include "Texture.h"
#include <iostream>

Texture::Texture(const char* path) {
    glGenTextures(1, &ID); //generate texture ID
    glBindTexture(GL_TEXTURE_2D, ID); //bind texture. kinda like VBO/VAO
    //GL_TEXTURE_2D means its a normal texture

    //These make the texture tile (GL_REPEAT tiles it) S = horizontal, T = vertical. they're texture space names for x/y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //MIN_FILTER/MAG_FILTER are minifying (texture smaller than screen area) and magnifying (bigger). GL_LINEAR = smooth blend
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); //image store rows top to bottom but OpenGL's UV (0,0) is bottom left so we need to flip the data
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0); //decodes the data and stores it into the references variables
    //nrChannels = 3 for RGB, 4 for RGBA

    if (data) { //stb returns null on failure so we check it worked
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB; //check if image has an alpha channel. means: 'if 4 channels, RGBA, else RGB'
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); //give the pixel data to the GPU as a texture
        //the args for above are: target, mipmap level (0 = base), how GL stores it, width, height, 0 (legacy border, always 0)
        //the source format, the data type (GL_UNSIGNED_BYTE == each channel is a 0 - 255 byte), and the pixel pointer
        glGenerateMipmap(GL_TEXTURE_2D); //auto creates mipmaps - read textures.md
    }
    else {
        std::cout << "TEXTURE FAILED TO LOAD: " << path << "\n";
    }

    stbi_image_free(data); //stb put all that data on the heap, so now thats its in the GPU, free it from the heap
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, ID);
}