#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace RealmEngine
{
    class Model;
    class Shader;

    struct Texture
    {
        enum class Type : uint8_t
        {
            Diffuse,
            Normal,
            Specular,
        };

        unsigned int id {0};
        Type         type {Type::Diffuse};
        std::string  path {0};
    };

    class Resource
    {
    public:
        void initialize();
        void terminate();

        std::shared_ptr<Texture> loadTexture(const std::string& path);
        std::shared_ptr<Model>   loadModel(const std::string& path);
        std::shared_ptr<Shader>  loadShader(const std::string& name);

        void clearCache();

    private:
        std::unordered_map<std::string, std::shared_ptr<Texture>> m_texture_cache;
        std::unordered_map<std::string, std::shared_ptr<Model>>   m_model_cache;
        std::unordered_map<std::string, std::shared_ptr<Shader>>  m_shader_cache;
    };
} // namespace RealmEngine