#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <string>

namespace RealmEngine
{
    class Shader
    {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        ~Shader();

        void         use() const;
        unsigned int getShaderProgram() const;

        void setBool(const std::string& name, bool value);
        void setInt(const std::string& name, int value);
        void setFloat(const std::string& name, float value);
        void setVec2(const std::string& name, const glm::vec2& value);
        void setVec3(const std::string& name, const glm::vec3& value);
        void setVec4(const std::string& name, const glm::vec4& value);
        void setMat2(const std::string& name, const glm::mat2& mat);
        void setMat3(const std::string& name, const glm::mat3& mat);
        void setMat4(const std::string& name, const glm::mat4& mat);

    private:
        unsigned int               m_program {0};
        std::map<std::string, int> m_uniform_cache;

        static void        checkCompileErrors(unsigned int shader, const std::string& type);
        static std::string loadShaderSource(const std::string& path);
        int                getUniformLocation(const std::string& name);
    };
} // namespace RealmEngine