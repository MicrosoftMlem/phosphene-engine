
when u make a shader u can choose not to do the whole:
glActiveTexture(GL_TEXTURE0);

texture.bind(); //YOU DO HAVE TO DO THIS

glUniform1i(glGetUniformLocation(shader.ID, "u_textureMap"), 0);

because when u bind a texture it defaults to slot 0 which is what the sampler2D also defaults to reading.
but if we want to put multiple textures u need to put in seperate slots.
so glActiveTexture says what slot to put it in
and glUniform1i says what slow this variable reads from
