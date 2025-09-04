#pragma once

#include "resource/resource.h"
#include <assimp/material.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>

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

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex>& verts, const std::vector<unsigned int>& inds, const std::vector<Texture>& texs);
        Mesh(std::vector<Vertex>&& verts, std::vector<unsigned int>&& inds, std::vector<Texture>&& texs);
        ~Mesh();

        Mesh(const Mesh&)            = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        void draw(unsigned int shader_program) const;

        const std::vector<Vertex>&       getVertices() const { return m_verts; }
        const std::vector<unsigned int>& getIndices() const { return m_inds; }
        const std::vector<Texture>&      getTextures() const { return m_texs; }

    private:
        std::vector<Vertex>       m_verts;
        std::vector<unsigned int> m_inds;
        std::vector<Texture>      m_texs;
        unsigned int              m_vao_id {0};
        unsigned int              m_vbo_id {0};
        unsigned int              m_ebo_id {0};

        void setupMesh();
        void cleanup();
    };

    class Model
    {
    public:
        Model() = default;
        explicit Model(const std::string& path) { loadModel(path); };
        ~Model() = default;

        Model(const Model&)            = delete; // copy construct not allowed
        Model& operator=(const Model&) = delete;
        Model(Model&&)                 = default; // move construct allowed
        Model& operator=(Model&&)      = default;

        void draw(unsigned int shader_program) const;
        bool loadFromFile(const std::string& path);

        const std::vector<Mesh>&    getMeshes() const { return m_meshes; }
        const std::vector<Texture>& getTextures() const { return m_textures; }
        const std::string&          getDirectory() const { return m_store_dir; }

    private:
        std::vector<Texture> m_textures;
        std::vector<Mesh>    m_meshes;
        std::string          m_store_dir;

        void                 loadModel(const std::string& path);
        std::vector<Texture> loadMaterialTextures(aiMaterial* ai_mat, aiTextureType ai_type);
        void                 processNode(aiNode* ai_node, const aiScene* ai_scene);
        Mesh                 processMesh(aiMesh* ai_mesh, const aiScene* ai_scene);
    };
} // namespace RealmEngine