#pragma once
#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class Texture
{
public:
    unsigned int ID;

    Texture(const char *loc, int color_mode, int input_color_mode, bool switch_orientation);
    void bind(int property);
};