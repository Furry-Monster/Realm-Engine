#include "model.h"
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
  unsigned int diffuseNr = 1;
  unsigned int normalNr = 1;

  for (unsigned int i = 0; i < m_texs.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);

    std::string number;
    std::string name;
    if (m_texs[i].type == tex_diffuse) {
      name = "texture_diffuse";
      number = std::to_string(diffuseNr++);
    } else if (m_texs[i].type == tex_normal) {
      name = "texture_normal";
      number = std::to_string(normalNr++);
    }

    glUniform1i(glGetUniformLocation(shader_program, (name + number).c_str()),
                i);
    glBindTexture(GL_TEXTURE_2D, m_texs[i].id);
  }

  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, m_inds.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glActiveTexture(GL_TEXTURE0);
}

Model::Model(const std::string &path) { loadModelFrom(path); }

void Model::loadModelFrom(const std::string &path) {
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                  aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
    return;
  }

  m_store_dir = path.substr(0, path.find_last_of('/'));

  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    m_meshes.push_back(processMesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vector;

    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.position = vector;

    if (mesh->HasNormals()) {
      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      vertex.normal = vector;
    }

    if (mesh->mTextureCoords[0]) {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.tex_coords = vec;
    } else
      vertex.tex_coords = glm::vec2(0.0f, 0.0f);

    vertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

  std::vector<Texture> diffuseMaps =
      loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

  std::vector<Texture> normalMaps =
      loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

  return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat,
                                                 aiTextureType type,
                                                 const std::string &typeName) {
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);

    bool skip = false;
    for (unsigned int j = 0; j < m_textures_loaded.size(); j++) {
      if (m_textures_loaded[j].path == str.C_Str()) {
        textures.push_back(m_textures_loaded[j]);
        skip = true;
        break;
      }
    }
    if (!skip) {
      Texture texture;
      texture.id = textureFromFile(str.C_Str(), this->m_store_dir);
      texture.type = (typeName == "texture_diffuse") ? tex_diffuse : tex_normal;
      texture.path = str.C_Str();
      textures.push_back(texture);
      m_textures_loaded.push_back(texture);
    }
  }
  return textures;
}

unsigned int Model::textureFromFile(const char *path,
                                    const std::string &directory) {
  std::string filename = std::string(path);
  filename = directory + '/' + filename;

  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
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

  return textureID;
}

void Model::draw(unsigned int shader_program) {
  for (unsigned int i = 0; i < m_meshes.size(); i++)
    m_meshes[i].draw(shader_program);
}