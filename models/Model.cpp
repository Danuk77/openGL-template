#include "assimp/Importer.hpp"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "assimp/vector3.h"
#include "models/Mesh.hpp"
#include "textures/TextureLoader.hpp"
#include <models/Model.hpp>
#include <stdexcept>

Assimp::Importer model_importer;

Model::Model(const char *path){
  set_model_directory(path);
  const aiScene* scene = read_model_file(path);
  check_model_read_errors(scene);

  parse_model_content(scene);
}

void Model::set_model_directory(string path){
  model_directory = path.substr(0, path.find_last_of('\\'));
}

const aiScene* Model::read_model_file(string path){
  const aiScene *scene = model_importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
  
  return scene;
}

void Model::check_model_read_errors(const aiScene* scene){
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
    std::cout << model_importer.GetErrorString() << std::endl;
    throw std::runtime_error(model_importer.GetErrorString());
  }
}

void Model::parse_model_content(const aiScene* scene){  
  aiNode* root_node = scene->mRootNode;

  recursively_parse_model_meshses(root_node, scene);
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
  vector<Texture> textures = parse_textures(mesh, scene);
  
  Mesh engine_mesh(vertices, indices, textures);

  return engine_mesh;
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

glm::vec2 Model::parse_texture_coordinates(aiVector3D* vertex, unsigned int i){
  glm::vec2 texture_coordinates;

  // Check if the model has any texture coordinates specified
  if(vertex){
    texture_coordinates.x = vertex[i].x;
    texture_coordinates.y = vertex[i].y;

  }else{
    texture_coordinates = glm::vec2(0.0f);
  }

  return texture_coordinates;
}

vector<unsigned int> Model::parse_indices(aiMesh* mesh){
  std::vector<unsigned int> indices;
  
  for (unsigned int i = 0; i < mesh->mNumFaces; i++){
    aiFace face = mesh->mFaces[i];
    parse_face(face, indices);
  }

  return indices;
}

void Model::parse_face(aiFace face, std::vector<unsigned int> &indices){
  for (unsigned int i = 0; i < face.mNumIndices; i++){
    unsigned int index = face.mIndices[i]; 
    indices.push_back(index);
  }
}

std::vector<Texture> Model::parse_textures(aiMesh *mesh, const aiScene *scene){
  std::vector<Texture> textures;
  unsigned int material_index = mesh->mMaterialIndex;

  // Fail early if no textures are specified in the mesh
  if(material_index < 0){
    return textures;
  } 

  aiMaterial *material = scene->mMaterials[material_index];
  parse_material_texture_by_type(material, aiTextureType_DIFFUSE, diffuse_map, textures);
  parse_material_texture_by_type(material, aiTextureType_SPECULAR, specular_map, textures);

  return textures;
}

void Model::parse_material_texture_by_type(aiMaterial *material, aiTextureType texture_type, TextureType internal_texture_type, std::vector<Texture> &textures){
  unsigned int number_of_textures = material->GetTextureCount(texture_type);

  for (unsigned int i = 0; i < number_of_textures; i++){
    aiString texture_path;
    material->GetTexture(texture_type, i, &texture_path);
    load_texture(texture_path, i, internal_texture_type, textures);
  }
}

void Model::load_texture(aiString texture_path, unsigned int texture_index, TextureType internal_texture_type, std::vector<Texture> textures){
  // Some materials may reuse the same textures
  // TODO: Need to implement a better system with a hashmap. Store the key as the path to the texture and the value as the index of the texture
  // in the loaded_texture vector so that it is much much faster.
  bool is_texture_already_loaded = check_if_texture_already_loaded(texture_path);

  if(is_texture_already_loaded){
    unsigned int loaded_texture_index = find_loaded_model_index(texture_path); 
    textures.push_back(loaded_textures[loaded_texture_index]);
  }else{
    Texture parsed_texture = parse_aiTexture_into_engine_texture(texture_path, internal_texture_type, texture_index);
    textures.push_back(parsed_texture);
    loaded_textures.push_back(parsed_texture);
  }
}

bool Model::check_if_texture_already_loaded(aiString texture_path){
  // TODO: Improve this algorithm, instead of iterating through the list, maybe we can do something more performant
  vector<Texture>::iterator texture_iterator;

  for (texture_iterator = loaded_textures.begin(); texture_iterator != loaded_textures.end(); texture_iterator++){
    bool texture_is_loaded = std::strcmp(texture_iterator->path.data(), texture_path.C_Str());

    if(texture_is_loaded){
      return true;
    }
  }

  return false;
}

unsigned int Model::find_loaded_model_index(aiString texture_path){ 
  for (unsigned int i = 0; i < loaded_textures.size(); i++){
    bool is_same_texture = std::strcmp(loaded_textures[i].path.data(), texture_path.C_Str());

    if(is_same_texture){
      return i; 
    }
  }
  
  // Never reaches here as we did a prior check to see if the texture is in the list 
  return -1; 
}

Texture Model::parse_aiTexture_into_engine_texture(aiString aiTexture_path, TextureType internal_texture_type, unsigned int index){
  TextureLoader loader(aiTexture_path.C_Str(), model_directory);

  Texture texture;
  texture.texture_id = loader.id;
  texture.texture_type = internal_texture_type;
  texture.texture_name = stringyfied_texture_types[internal_texture_type] + std::to_string(index); 
 
  return texture;
}

void Model::draw(Shader &shader){
  vector<Mesh>::iterator mesh_iterator;
  

  for(mesh_iterator = meshes.begin(); mesh_iterator != meshes.end(); mesh_iterator++){
    mesh_iterator->draw(shader);
  }
}
