#include "resource.h"
#include "global.h"
#include "model.h"
#include "shader.h"

namespace RealmEngine
{
    void Resource::initialize() { LOG_INFO("Resource Manager initialized"); }

    void Resource::terminate()
    {
        clearCache();

        LOG_INFO("Resource Manager terminated");
    }

    std::shared_ptr<Texture> Resource::loadTexture(const std::string& path)
    {
        auto it = m_texture_cache.find(path);
        if (it != m_texture_cache.end())
        {
            return it->second;
        }

        auto texture          = std::make_shared<Texture>();
        texture->path         = path;
        m_texture_cache[path] = texture;
        return texture;
    }

    std::shared_ptr<Model> Resource::loadModel(const std::string& path)
    {
        auto it = m_model_cache.find(path);
        if (it != m_model_cache.end())
        {
            return it->second;
        }

        auto model = std::make_shared<Model>();
        model->loadFromFile(path);
        m_model_cache[path] = model;
        return model;
    }

    std::shared_ptr<Shader> Resource::loadShader(const std::string& name)
    {
        auto it = m_shader_cache.find(name);
        if (it != m_shader_cache.end())
        {
            return it->second;
        }

        auto shader          = std::make_shared<Shader>(name + ".vert", name + ".frag");
        m_shader_cache[name] = shader;
        return shader;
    }

    void Resource::clearCache()
    {
        m_texture_cache.clear();
        m_model_cache.clear();
        m_shader_cache.clear();

        LOG_INFO("Resource cache cleared");
    }
} // namespace RealmEngine