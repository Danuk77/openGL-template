#pragma once
#include <shader/Shader.hpp>
#include <models/Texture.hpp>
#include <models/Vertex.hpp>
#include <vector>
#include <glad/glad.h>

class Mesh{
public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> diffuse_maps;
  std::vector<Texture> specular_maps;
 
  Mesh(std::vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
  void draw(Shader &shader);

private:
  unsigned int vertex_array_object;
  unsigned int vertex_buffer_object;
  unsigned int element_buffer_object;
  
  void store_textures(vector<Texture> textures);
  void store_texture(Texture texture);

  void initialize_mesh();
  void generate_buffers();
  void populate_buffers();
  void populate_element_buffer();
  void populate_vertex_buffer();
  void link_vertex_attributes();
  void link_vertex_positions();
  void link_vertex_normals();
  void link_texture_coordinates();

  void load_textures_into_buffers(Shader shader);
  void load_diffuse_maps(Shader shader);
  void load_texture(Texture texture, unsigned int texture_index, Shader shader);
  void load_specular_maps(Shader shader);

  void draw_elements();
};
