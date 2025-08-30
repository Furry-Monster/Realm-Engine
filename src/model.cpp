#include "model.h"
#include "assimp/material.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glad/gl.h>
#include <iostream>
#include <stb_image.h>

Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> inds,
           std::vector<Texture> texs)
    : m_verts(verts), m_inds(inds), m_texs(texs) {
  setupMesh();
}

void Mesh::setupMesh() {
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);

  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, m_verts.size() * sizeof(Vertex), &m_verts[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_inds.size() * sizeof(unsigned int),
               &m_inds[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, tex_coords));

  glBindVertexArray(0);
}

void Mesh::draw(unsigned int shader_program) {
  unsigned int diffuse_nr{1};
  unsigned int normal_nr{1};

  for (unsigned int i = 0; i < m_texs.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);

    std::string number;
    std::string name;
    if (m_texs[i].type == texDiffuse) {
      name = "texture_diffuse";
      number = std::to_string(diffuse_nr++);
    } else if (m_texs[i].type == texNormal) {
      name = "texture_normal";
      number = std::to_string(normal_nr++);
    }

    std::string uniformName = name + number;
    glUniform1i(glGetUniformLocation(shader_program, uniformName.c_str()), i);
    glBindTexture(GL_TEXTURE_2D, m_texs[i].id);
  }

  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, m_inds.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

Model::Model(const std::string &path) { loadModelFrom(path); }

void Model::loadModelFrom(const std::string &path) {
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(path, aiProcess_Triangulate | 
                              aiProcess_CalcTangentSpace |
                              aiProcess_GenNormals);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
    return;
  }

  m_store_dir = path.substr(0, path.find_last_of('/'));

  // recursively process node in scene graph
  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  // front-order dfs
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    m_meshes.push_back(processMesh(mesh, scene));
  }

  // recurse here
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  Vertex vertex;
  glm::vec3 vec3;
  glm::vec2 vec2;
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    vec3.x = mesh->mVertices[i].x;
    vec3.y = mesh->mVertices[i].y;
    vec3.z = mesh->mVertices[i].z;
    vertex.position = vec3;

    if (mesh->HasNormals()) {
      vec3.x = mesh->mNormals[i].x;
      vec3.y = mesh->mNormals[i].y;
      vec3.z = mesh->mNormals[i].z;
      vertex.normal = vec3;
    }

    if (mesh->mTextureCoords[0]) {
      vec2.x = mesh->mTextureCoords[0][i].x;
      vec2.y = mesh->mTextureCoords[0][i].y;
      vertex.tex_coords = vec2;
    } else
      vertex.tex_coords = glm::vec2(0.0f, 0.0f);

    vertices.push_back(vertex);
  }

  aiFace face;
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
  std::vector<Texture> diffuseMaps =
      loadMaterialTextures(material, aiTextureType_DIFFUSE);
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
  std::vector<Texture> normalMaps =
      loadMaterialTextures(material, aiTextureType_HEIGHT);
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

  return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat,
                                                 aiTextureType type) {
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    texType tex_type;
    mat->GetTexture(type, i, &str);

    if (type == aiTextureType_DIFFUSE)
      tex_type = texDiffuse;
    else if (type == aiTextureType_HEIGHT)
      tex_type = texNormal;

    // if loaded manually , skip this step
    bool skip = false;
    for (unsigned int j = 0; j < m_textures.size(); j++) {
      if (m_textures[j].path == str.C_Str()) {
        textures.push_back(m_textures[j]);
        skip = true;
        break;
      }
    }

    if (!skip) {
      Texture texture;
      std::string fullPath = m_store_dir + "/" + str.C_Str();
      texture.id = textureFromFile(fullPath.c_str());
      texture.type = tex_type;
      texture.path = str.C_Str();
      textures.push_back(texture);
      m_textures.push_back(texture);
    }
  }
  return textures;
}

unsigned int Model::textureFromFile(const char *path) {
  unsigned int texture_id;
  glGenTextures(1, &texture_id);

  int width, height, nr_components;
  unsigned char *data = stbi_load(path, &width, &height, &nr_components, 0);
  if (data) {
    GLenum format;
    if (nr_components == 1)
      format = GL_RED;
    else if (nr_components == 3)
      format = GL_RGB;
    else if (nr_components == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return texture_id;
}

void Model::draw(unsigned int shader_program) {
  for (unsigned int i = 0; i < m_meshes.size(); i++)
    m_meshes[i].draw(shader_program);
}