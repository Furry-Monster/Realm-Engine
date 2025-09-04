#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace RealmEngine
{
    class Model;

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

    class ResourceManager
    {
    public:
    private:
        std::unordered_map<std::string, std::shared_ptr<Texture>> m_texture_cache;
        std::unordered_map<std::string, std::shared_ptr<Model>>   m_model_cache;
    };
} // namespace RealmEngine