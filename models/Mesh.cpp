#include "shader/Shader.hpp"
#include <models/Mesh.hpp>
#include <stdexcept>

Mesh::Mesh(std::vector<Vertex> vertices,
           vector<unsigned int> indices, 
           vector<Texture> textures): vertices(vertices),indices(indices) 
{
  store_textures(textures);
  initialize_mesh();
}

void Mesh::store_textures(vector<Texture> textures){
  std::vector<Texture>::iterator texture_iterator;

  for(texture_iterator = textures.begin(); texture_iterator != textures.end(); texture_iterator++){
    store_texture(*texture_iterator);
  }
}

void Mesh::store_texture(Texture texture){
  switch (texture.texture_type) {
    case diffuse_map:
      diffuse_maps.push_back(texture);
      break;

    case specular_map:
      specular_maps.push_back(texture);
      break;

    default:
      throw std::runtime_error("Unaccepted texture type");
  }
}

void Mesh::initialize_mesh(){
  generate_buffers();
  populate_buffers();
  link_vertex_attributes();
}

void Mesh::generate_buffers(){
  glGenVertexArrays(1, &vertex_array_object);
  glGenBuffers(1, &vertex_buffer_object);
  glGenBuffers(1, &element_buffer_object);
}

void Mesh::populate_buffers(){
  glBindVertexArray(vertex_array_object);
  
  populate_vertex_buffer();
  populate_element_buffer();

  // Bind to empty array so that elsewhere will not be modifying the current vertex array object
  glBindVertexArray(0);
}

void Mesh::populate_vertex_buffer(){
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

  float size_of_vertex = sizeof(Vertex);
  float size_of_all_vertices = size_of_vertex * vertices.size();
  glBufferData(GL_ARRAY_BUFFER, size_of_all_vertices, &vertices[0], GL_STATIC_DRAW);
}

void Mesh::populate_element_buffer(){
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);

  float size_of_index = sizeof(unsigned int);
  float size_of_buffer = size_of_index * indices.size();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_of_buffer, &indices[0], GL_STATIC_DRAW);
}

void Mesh::link_vertex_attributes(){
  link_vertex_positions();
  link_vertex_normals();
  link_texture_coordinates();
}

void Mesh::link_vertex_positions(){
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
}

void Mesh::link_vertex_normals(){
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
}

void Mesh::link_texture_coordinates(){
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coordinates));
}

void Mesh::draw(Shader &shader){
  load_textures_into_buffers(shader);
  draw_elements();
}

// The convention used for a texture uniform in the GLSL shader -> <diffuse/specular>_<texture_name>
// TODO: incomplete 
void Mesh::load_textures_into_buffers(Shader shader){
  load_diffuse_maps(shader);
  load_specular_maps(shader);
}

void Mesh::load_diffuse_maps(Shader shader){
  std::vector<Texture>::iterator diffuse_map_iterator;

  for(diffuse_map_iterator = diffuse_maps.begin(); diffuse_map_iterator != diffuse_maps.end(); diffuse_map_iterator++){
    shader.set_int(diffuse_map_iterator->texture_name, diffuse_map_iterator->texture_id);
  }
}

void Mesh::load_specular_maps(Shader shader){
  std::vector<Texture>::iterator specular_map_iterator;

  for(specular_map_iterator = specular_maps.begin(); specular_map_iterator != specular_maps.end(); specular_map_iterator++){
    shader.set_int(specular_map_iterator->texture_name, specular_map_iterator->texture_id);
  }
}

void Mesh::draw_elements(){

}
