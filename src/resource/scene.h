#pragma once

#include "resource/model.h"

#include <cstddef>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace RealmEngine
{
    class Camera;

    struct RenderObject
    {
        std::shared_ptr<Model> model;
        glm::mat4              transform;

        RenderObject() : transform(1.0f) {}
        explicit RenderObject(std::shared_ptr<Model> m, const glm::mat4& t = glm::mat4(1.0f)) : model(m), transform(t)
        {}
    };

    struct DirectionalLight
    {
        glm::vec3 direction;
        glm::vec3 color;
        float     intensity;

        DirectionalLight() : direction(0.0f, -1.0f, 0.0f), color(1.0f), intensity(1.0f) {}
        DirectionalLight(const glm::vec3& dir, const glm::vec3& col, float inten = 1.0f) :
            direction(dir), color(col), intensity(inten)
        {}
    };

    struct PointLight
    {
        glm::vec3 position;
        glm::vec3 color;
        float     intensity;

        float constant  = 1.0f;
        float linear    = 0.09f;
        float quadratic = 0.032f;

        PointLight() : position(0.0f), color(1.0f), intensity(1.0f) {}
        PointLight(const glm::vec3& pos, const glm::vec3& col, float inten = 1.0f) :
            position(pos), color(col), intensity(inten)
        {}
    };

    class Scene
    {
    public:
        explicit Scene(const std::string& name = "DefaultScene");
        ~Scene();

        void                    setCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }
        std::shared_ptr<Camera> getCamera() const { return m_camera; }

        void addRenderObject(std::shared_ptr<Model> model, const glm::mat4& transform = glm::mat4(1.0f));
        void removeRenderObject(size_t index);
        const std::vector<RenderObject>& getAllRenderObjects() const { return m_render_objects; }

        void addDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity = 1.0f);
        void removeDirectionalLight(size_t index);
        void addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity = 1.0f);
        void removePointLight(size_t index);

        const std::vector<DirectionalLight>& getDirectionalLights() const { return m_directional_lights; }
        const std::vector<PointLight>&       getPointLights() const { return m_point_lights; }

        void clear();

    private:
        std::string                   m_name;
        std::shared_ptr<Camera>       m_camera;
        std::vector<RenderObject>     m_render_objects;
        std::vector<DirectionalLight> m_directional_lights;
        std::vector<PointLight>       m_point_lights;
    };
} // namespace RealmEngine