#pragma once
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

class Model{
public:
  Model(char *path);
  void draw(Shader &shader);

private:
  vector<Mesh> meshes;
  std::string model_directory;

  void set_model_directory(string path);
  const aiScene* read_model_file(string path);
  void check_model_read_errors(const aiScene* scene);
  void parse_model_content(const aiScene* scene);

  void parse_model_meshes();
  void recursively_parse_model_meshses(aiNode* node, const aiScene* scene);
  void recursively_parse_model_materials(aiNode* node, const aiScene* scene);
  void parse_mesh_in_node(aiNode* node, const aiScene* scene, unsigned int index);
  Mesh convert_assimp_mesh_to_engine_mesh(aiMesh* mesh, const aiScene* scene);

  vector<Vertex> parse_vertices(aiMesh* mesh);
  glm::vec3 parse_position(aiVector3D vertex);
  glm::vec3 parse_normal(aiVector3D vertex);
  glm::vec3 parse_texture_coordinates(aiVector3D* vertex, unsigned int i);

  vector<unsigned int> parse_indices(aiMesh* mesh);
  void parse_face(aiFace face, vector<unsigned int> &indices);
  //parse_textures();

  };
