#include <textures/Textures.hpp>
#include <stb/stb_image.h>

/**
 * Constructor for a 2d RGB texture object
 * @param loc The path to the texture image
 */
Texture::Texture(const char *loc, int color_mode, int input_color_mode, bool switch_orientation)
{
    stbi_set_flip_vertically_on_load(switch_orientation);

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(loc, &width, &height, &nrChannels, 0);

    if (data)
    {
        // load the data on to the texture
        glTexImage2D(GL_TEXTURE_2D, 0, color_mode, width, height, 0, input_color_mode, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Error loading texture" << endl;
    }

    // Free up the memory for the loaded image
    stbi_image_free(data);
}

/**
 * Binds the texture to a given gl enum
 * @param property The enum to bind to
 */
void Texture::bind(int property)
{
    glBindTexture(property, ID);
}