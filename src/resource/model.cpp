#include "model.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "global.h"

namespace RealmEngine
{
    unsigned int loadTextureFromFile(const char* path);

    Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> inds, std::vector<Texture> texs) :
        m_verts(verts), m_inds(inds), m_texs(texs)
    {
        setupMesh();
    }

    void Mesh::setupMesh()
    {
        glGenVertexArrays(1, &m_vao_id);
        glGenBuffers(1, &m_vbo_id);
        glGenBuffers(1, &m_ebo_id);

        glBindVertexArray(m_vao_id);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, m_verts.size() * sizeof(Vertex), &m_verts[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_inds.size() * sizeof(unsigned int), &m_inds[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tex_coords)));

        glBindVertexArray(0);
    }

    void Mesh::draw(unsigned int shader_program)
    {
        unsigned int diffuse_nr {1};
        unsigned int normal_nr {1};

        for (unsigned int i = 0; i < m_texs.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string name;
            if (m_texs[i].type == Texture::Type::Diffuse)
            {
                name   = "texture_diffuse";
                number = std::to_string(diffuse_nr++);
            }
            else if (m_texs[i].type == Texture::Type::Normal)
            {
                name   = "texture_normal";
                number = std::to_string(normal_nr++);
            }

            std::string uniform_name = name + number;
            glUniform1i(glGetUniformLocation(shader_program, uniform_name.c_str()), i);
            glBindTexture(GL_TEXTURE_2D, m_texs[i].id);
        }

        glBindVertexArray(m_vao_id);
        glDrawElements(GL_TRIANGLES, m_inds.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    Model::Model() = default;

    Model::Model(const std::string& path) { loadModel(path); }

    void Model::loadModel(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene*   scene =
            importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LOG_ERROR("ERROR::ASSIMP::", importer.GetErrorString());
            return;
        }

        m_store_dir = path.substr(0, path.find_last_of('/'));

        // recursively process node in scene graph
        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode* ai_node, const aiScene* ai_scene)
    {
        // front-order dfs
        for (unsigned int i = 0; i < ai_node->mNumMeshes; i++)
        {
            aiMesh* mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
            m_meshes.push_back(processMesh(mesh, ai_scene));
        }

        // recurse here
        for (unsigned int i = 0; i < ai_node->mNumChildren; i++)
        {
            processNode(ai_node->mChildren[i], ai_scene);
        }
    }

    Mesh Model::processMesh(aiMesh* ai_mesh, const aiScene* ai_scene)
    {
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;

        Vertex    vertex;
        glm::vec3 vec3;
        glm::vec2 vec2;
        for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++)
        {
            vec3.x          = ai_mesh->mVertices[i].x;
            vec3.y          = ai_mesh->mVertices[i].y;
            vec3.z          = ai_mesh->mVertices[i].z;
            vertex.position = vec3;

            if (ai_mesh->HasNormals())
            {
                vec3.x        = ai_mesh->mNormals[i].x;
                vec3.y        = ai_mesh->mNormals[i].y;
                vec3.z        = ai_mesh->mNormals[i].z;
                vertex.normal = vec3;
            }

            if (ai_mesh->mTextureCoords[0])
            {
                vec2.x            = ai_mesh->mTextureCoords[0][i].x;
                vec2.y            = ai_mesh->mTextureCoords[0][i].y;
                vertex.tex_coords = vec2;
            }
            else
                vertex.tex_coords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        aiFace face;
        for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++)
        {
            face = ai_mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        std::vector<Texture> textures;
        aiMaterial*          material     = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
        std::vector<Texture> diffuse_maps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
        std::vector<Texture> normal_maps = loadMaterialTextures(material, aiTextureType_HEIGHT);
        textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
        std::vector<Texture> specular_maps = loadMaterialTextures(material, aiTextureType_SPECULAR);
        textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> Model::loadMaterialTextures(aiMaterial* ai_mat, aiTextureType ai_type)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < ai_mat->GetTextureCount(ai_type); i++)
        {
            aiString      str;
            Texture::Type tex_type;
            ai_mat->GetTexture(ai_type, i, &str);

            if (ai_type == aiTextureType_DIFFUSE)
                tex_type = Texture::Type::Diffuse;
            else if (ai_type == aiTextureType_HEIGHT)
                tex_type = Texture::Type::Normal;
            else if (ai_type == aiTextureType_SPECULAR)
                tex_type = Texture::Type::Specular;

            // if loaded manually , skip this step
            bool skip = false;
            for (auto& m_texture : m_textures)
            {
                if (m_texture.path == str.C_Str())
                {
                    textures.push_back(m_texture);
                    skip = true;
                    break;
                }
            }

            if (!skip)
            {
                Texture     texture;
                std::string full_path = m_store_dir + "/" + str.C_Str();
                texture.id            = loadTextureFromFile(full_path.c_str());
                texture.type          = tex_type;
                texture.path          = str.C_Str();
                textures.push_back(texture);
                m_textures.push_back(texture);
            }
        }
        return textures;
    }

    void Model::draw(unsigned int shader_program)
    {
        for (auto& m_mesh : m_meshes)
            m_mesh.draw(shader_program);
    }

    unsigned int loadTextureFromFile(const char* path)
    {
        unsigned int texture_id;
        glGenTextures(1, &texture_id);

        int            width, height, nr_components;
        unsigned char* data = stbi_load(path, &width, &height, &nr_components, 0);

        if (data)
        {
            GLenum format;
            if (nr_components == 1)
                format = GL_RED;
            else if (nr_components == 3)
                format = GL_RGB;
            else if (nr_components == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            LOG_ERROR("Texture failed to load at path", path);
            stbi_image_free(data);
        }

        return texture_id;
    }
} // namespace RealmEngine