#include <String>

enum TextureType{
  diffuse_map,
  specular_map
};


struct Texture{
  unsigned int texture_id;
  std::string texture_name;
  TextureType texture_type;
  std::string path;
};

