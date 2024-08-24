#pragma once
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/vector3.h"
#include <models/Mesh.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <shader/Shader.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <map>

class Model{
public:
  Model(const char *path);
  void draw(Shader &shader);
  unsigned int number_of_meshes();
  Mesh peek();

private:
  vector<Mesh> meshes;
  std::string model_directory;
  std::vector<Texture> loaded_textures;
  std::map<TextureType, std::string> stringyfied_texture_types{{diffuse_map, "diffuse_map"}, {specular_map, "specular_map"}};

  void set_model_directory(string path);
  const aiScene* read_model_file(string path);
  void check_model_read_errors(const aiScene* scene);
  void parse_model_content(const aiScene* scene);

  void parse_model_meshes();
  void recursively_parse_model_meshses(aiNode* node, const aiScene* scene);
  void parse_mesh_in_node(aiNode* node, const aiScene* scene, unsigned int index);
  Mesh convert_assimp_mesh_to_engine_mesh(aiMesh* mesh, const aiScene* scene);

  vector<Vertex> parse_vertices(aiMesh* mesh);
  glm::vec3 parse_position(aiVector3D vertex);
  glm::vec3 parse_normal(aiVector3D vertex);
  glm::vec2 parse_texture_coordinates(aiVector3D* vertex, unsigned int i);

  vector<unsigned int> parse_indices(aiMesh* mesh);
  void parse_face(aiFace face, vector<unsigned int> &indices);
  std::vector<Texture> parse_textures(aiMesh *mesh, const aiScene *scene);
  void parse_material_texture_by_type(aiMaterial *material, aiTextureType texture_type, TextureType internal_texture_type, std::vector<Texture> &textures); 
  void load_texture(aiString texture_path, unsigned int texture_index, TextureType internal_texture_type, std::vector<Texture> textures);
  bool check_if_texture_already_loaded(aiString texture_path);
  unsigned int find_loaded_model_index(aiString texture_path);
  Texture parse_aiTexture_into_engine_texture(aiString aiTexture_path, TextureType internal_texture_type, unsigned int index);
  };
