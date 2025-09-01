#pragma once

#include <assimp/material.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

enum TexType
{
    texDiffuse,
    texNormal,
    texAO,
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

struct Texture
{
    unsigned int id;
    TexType      type;
    std::string  path;
};

class Mesh
{
public:
    std::vector<Vertex>       m_verts;
    std::vector<unsigned int> m_inds;
    std::vector<Texture>      m_texs;
    unsigned int              m_vao_id;
    unsigned int              m_vbo_id;
    unsigned int              m_ebo_id;

    Mesh(std::vector<Vertex> verts, std::vector<unsigned int> inds, std::vector<Texture> texs);

    void draw(unsigned int shader_program);

private:
    void setupMesh();
};

class Model
{
public:
    std::vector<Texture> m_textures;
    std::vector<Mesh>    m_meshes;
    std::string          m_store_dir;

    explicit Model(const std::string& path);
    void draw(unsigned int shader_program);

private:
    void                 loadModelFrom(const std::string& path);
    void                 processNode(aiNode* node, const aiScene* scene);
    Mesh                 processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type);
};