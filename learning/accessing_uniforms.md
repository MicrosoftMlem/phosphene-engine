READ AS RAW TEXT NOT MD

here is an example (was used in main.cpp before) on how to access and modify shader uniforms (specifically matrixes):



        glm::mat4 transform = glm::mat4(1.0f); //we always start by making an identity matrix which is a matrix with 1s on the diagonals,
                                                // which basically does nothing. we then add transforms after

        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        //glm::rotate(transform, angle, axis) takes the matrix, applies a rotation of 'angle' radians around axis, returns the result
        //(float)glfwGetTime() is the seconds since start and we feed it as an angle so it continuously spins
        //glm::vec3(0, 0, 1) is the axis to spin on. this one specifically is the Z axis (which points at the camera/screen). this builds all the cos/sin stuff automatically

        unsigned int loc = glGetUniformLocation(shader.ID, "transform"); //asks where the uniform named "transform" is and returns its location (an ID)
        //we look it up using the name we used in the shader. (thats why Shader.ID is public rn bc we reach in to get it. Its temporary since ID should be private)

        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));
        //loc = where to put it (from above), 1 = sending 1 matrix, GL_FALSE = 'dont transpose it' (glm and GL agree on layout so dont flip it)
        // glm::value_ptr(transform) = glm stores the matrix as a c++ object, but OpenGL wants a raw pointer to the 16 floats. value_ptr is what gives that ptr