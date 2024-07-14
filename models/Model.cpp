#include "assimp/Importer.hpp"
#include "assimp/mesh.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/vector3.h"
#include "models/Mesh.hpp"
#include <models/Model.hpp>
#include <stdexcept>

Assimp::Importer model_importer;

Model::Model(char *path){
  set_model_directory(path);
  const aiScene* scene = read_model_file(path);
  check_model_read_errors(scene);
  parse_model_content(scene);
}

void Model::set_model_directory(string path){
  model_directory = path.substr(0, path.find_last_of('/'));
}

const aiScene* Model::read_model_file(string path){
  auto model_load_options = aiProcess_Triangulate | aiProcess_FlipUVs;
  const aiScene *scene = model_importer.ReadFile(path, model_load_options);
  
  return scene;
}

void Model::check_model_read_errors(const aiScene* scene){
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
    throw std::runtime_error(model_importer.GetErrorString());
  }
}

void Model::parse_model_content(const aiScene* scene){  
  aiNode* root_node = scene->mRootNode;

  recursively_parse_model_meshses(root_node, scene);
  recursively_parse_model_materials(root_node, scene);
}

void Model::recursively_parse_model_meshses(aiNode* node, const aiScene* scene){
  unsigned int number_of_meshes = node->mNumMeshes;
  unsigned int number_of_children_nodes = node->mNumChildren;

  for(unsigned int i = 0; i < number_of_meshes; i++){
    parse_mesh_in_node(node, scene, i);
  }

  for(unsigned int i = 0; i < number_of_children_nodes; i++){
    aiNode* node_to_parse = node->mChildren[i];
    recursively_parse_model_meshses(node_to_parse, scene);
  }
}

void Model::parse_mesh_in_node(aiNode* node, const aiScene* scene, unsigned int index){
  unsigned int mesh_index = node->mMeshes[index];
  aiMesh *loaded_assimp_mesh = scene->mMeshes[mesh_index];
  Mesh converted_mesh = convert_assimp_mesh_to_engine_mesh(loaded_assimp_mesh, scene);
  meshes.push_back(converted_mesh);
}

Mesh Model::convert_assimp_mesh_to_engine_mesh(aiMesh* mesh, const aiScene* scene){
  vector<Vertex> vertices = parse_vertices(mesh);
  vector<unsigned int> indices = parse_indices(mesh);
  parse_textures();

}

vector<Vertex> Model::parse_vertices(aiMesh* mesh){
  vector<Vertex> vertices;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++){
    Vertex vertex;

    vertex.position = parse_position(mesh->mVertices[i]);
    vertex.normal = parse_normal(mesh->mNormals[i]);
    vertex.texture_coordinates = parse_texture_coordinates(mesh->mTextureCoords[0], i);

    vertices.push_back(vertex);
  }  

  return vertices;
}

glm::vec3 Model::parse_position(aiVector3D vertex){
  glm::vec3 position;

  position.x = vertex.x;
  position.y = vertex.y;
  position.z = vertex.z;

  return position;
}

glm::vec3 Model::parse_normal(aiVector3D vertex){
  glm::vec3 normal;

  normal.x = vertex.x;
  normal.y = vertex.y;
  normal.z = vertex.z;

  return normal;
}

glm::vec3 Model::parse_texture_coordinates(aiVector3D* vertex, unsigned int i){
  glm::vec2 texture_coordinates;

  // Check if the model has any texture coordinates specified
  if(vertex){
    texture_coordinates.x = vertex[i].x;
    texture_coordinates.y = vertex[i].y;

  }else{
    texture_coordinates = glm::vec2(0.0f);
  }
}

vector<unsigned int> Model::parse_indices(aiMesh* mesh){
  vector<unsigned int> indices;

  for (unsigned int i = 0; i < mesh->mNumFaces; i++){
    aiFace face = mesh->mFaces[i];
    parse_index(face, indices);
  }
}

void Model::parse_face(aiFace face, vector<unsigned int> &indices){
  for (unsigned int i = 0; i < face.mNumIndices; i++){
    unsigned int index = face.mIndices[i]; 
    indices.push_back(index);
  }
}

void Model::draw(Shader &shader){
  vector<Mesh>::iterator mesh_iterator;
  

  for(mesh_iterator = meshes.begin(); mesh_iterator != meshes.end(); mesh_iterator++){
    mesh_iterator->draw(shader);
  }
}
