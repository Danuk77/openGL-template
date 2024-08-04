#pragma once
#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>

using namespace std;

class TextureLoader
{
public:
    unsigned int id;

    TextureLoader(const char *loc, int color_mode, int input_color_mode, bool switch_orientation);
    TextureLoader(const char *path, const string &directory);

    void load_file_data(const char* path);
    GLenum identify_format(unsigned int number_of_channels);
    void bind_texture(unsigned char *texture_data, GLenum format, int width, int height);

    void bind(int property);
};
