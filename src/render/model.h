#pragma once

#include <assimp/material.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace RealmEngine
{

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coords;
    };

    struct Texture
    {
        enum class Type : uint8_t
        {
            Diffuse,
            Normal,
            Specular,
        };

        unsigned int id;
        Type         type;
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

        Model();
        explicit Model(const std::string& path);
        void draw(unsigned int shader_program);

    private:
        void                 loadModel(const std::string& path);
        std::vector<Texture> loadMaterialTextures(aiMaterial* ai_mat, aiTextureType ai_type);
        void                 processNode(aiNode* ai_node, const aiScene* ai_scene);
        Mesh                 processMesh(aiMesh* ai_mesh, const aiScene* ai_scene);
    };
} // namespace RealmEngine