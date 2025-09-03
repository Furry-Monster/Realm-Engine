#include "scene.h"

namespace RealmEngine
{
    Scene::Scene(const std::string& name) : m_name(name) {}

    Scene::~Scene() { clear(); }

    void Scene::addRenderObject(std::shared_ptr<Model> model, const glm::mat4& transform)
    {
        if (model)
        {
            m_render_objects.emplace_back(model, transform);
        }
    }

    void Scene::removeRenderObject(size_t index)
    {
        if (index < m_render_objects.size())
        {
            m_render_objects.erase(m_render_objects.begin() + index);
        }
    }

    void Scene::addDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
    {
        m_directional_lights.emplace_back(direction, color, intensity);
    }

    void Scene::removeDirectionalLight(size_t index)
    {
        if (index < m_directional_lights.size())
        {
            m_directional_lights.erase(m_directional_lights.begin() + index);
        }
    }

    void Scene::addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity)
    {
        m_point_lights.emplace_back(position, color, intensity);
    }

    void Scene::removePointLight(size_t index)
    {
        if (index < m_point_lights.size())
        {
            m_point_lights.erase(m_point_lights.begin() + index);
        }
    }

    void Scene::clear()
    {
        m_render_objects.clear();
        m_directional_lights.clear();
        m_point_lights.clear();
        m_camera.reset();
    }
} // namespace RealmEngine