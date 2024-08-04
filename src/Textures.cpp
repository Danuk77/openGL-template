#include <iostream>
#include <stdexcept>
#include <textures/TextureLoader.hpp>
#include <stb/stb_image.h>

/**
 * Old contructor, no longer used (might be useful later on though)
 * Constructor for a 2d RGB texture object
 * @param loc The path to the texture image
 */
TextureLoader::TextureLoader(const char *loc, int color_mode, int input_color_mode, bool switch_orientation)
{
    stbi_set_flip_vertically_on_load(switch_orientation);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

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

TextureLoader::TextureLoader(const char *path, const string &directory){
  string filename = string(path);
  filename = directory + "/" + filename;

  load_file_data(filename.c_str());
}

void TextureLoader::load_file_data(const char *path){
  int width, height, number_of_channels;
  unsigned char* texture_data = stbi_load(path, &width, &height, &number_of_channels, 0);
  GLenum format;

  // Fail early check to see if the data was loaded
  if(!texture_data){
    throw std::runtime_error("Failed to load texture at " + string(path));
    stbi_image_free(texture_data);
  }

  try{
    format = identify_format(number_of_channels);
  }catch(std::runtime_error e){
    cout << e.what() << std::endl; 
  }

  bind_texture(texture_data, format, width, height);
  stbi_image_free(texture_data);

}

GLenum TextureLoader::identify_format(unsigned int number_of_channels){
  switch (number_of_channels) {
    case 1:
      return GL_RED;
    case 3:
      return GL_RGB;
    case 4:
      return GL_RGBA;
    default:
      throw std::runtime_error("Unsupported texture type");
  }
}

void TextureLoader::bind_texture(unsigned char *texture_data, GLenum format, int width, int height){
  glGenTextures(1, &id);

  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture_data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

/**
 * Binds the texture to a given gl enum
 * @param property The enum to bind to
 */
void TextureLoader::bind(int property)
{
    glBindTexture(property, id);
}
