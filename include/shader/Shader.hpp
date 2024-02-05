#pragma once
#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class Shader
{
public:
    // The shader program's ID
    unsigned int id;

    // Constructor for the shader program
    Shader(const char *vertex_path, const char *fragment_path);

    // use/activate the shader
    void use();

    // utility uniform functions
    void set_bool(const std::string &uniform_name, bool value) const;
    void set_int(const std::string &uniform_name, int value) const;
    void set_float(const std::string &uniform_name, float value) const;
    void set_mat4(const std::string &uniform_name, glm::mat4 value) const;
};