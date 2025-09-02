#include "shader.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <fstream>
#include <iostream>
#include <sstream>

namespace RealmEngine
{
    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        std::string vertex_code   = loadShaderSource(vertexPath);
        std::string fragment_code = loadShaderSource(fragmentPath);

        const char* v_shader_code = vertex_code.c_str();
        const char* f_shader_code = fragment_code.c_str();

        unsigned int vertex, fragment;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &v_shader_code, nullptr);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &f_shader_code, nullptr);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        m_program = glCreateProgram();
        glAttachShader(m_program, vertex);
        glAttachShader(m_program, fragment);
        glLinkProgram(m_program);
        checkCompileErrors(m_program, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    Shader::~Shader() { glDeleteProgram(m_program); }

    void Shader::use() const { glUseProgram(m_program); }

    unsigned int Shader::getShaderProgram() const { return m_program; }

    void Shader::setBool(const std::string& name, bool value)
    {
        glUniform1i(getUniformLocation(name), static_cast<int>(value));
    }

    void Shader::setInt(const std::string& name, int value) { glUniform1i(getUniformLocation(name), value); }

    void Shader::setFloat(const std::string& name, float value) { glUniform1f(getUniformLocation(name), value); }

    void Shader::setVec2(const std::string& name, const glm::vec2& value)
    {
        glUniform2fv(getUniformLocation(name), 1, &value[0]);
    }

    void Shader::setVec3(const std::string& name, const glm::vec3& value)
    {
        glUniform3fv(getUniformLocation(name), 1, &value[0]);
    }

    void Shader::setVec4(const std::string& name, const glm::vec4& value)
    {
        glUniform4fv(getUniformLocation(name), 1, &value[0]);
    }

    void Shader::setMat2(const std::string& name, const glm::mat2& mat)
    {
        glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::setMat3(const std::string& name, const glm::mat3& mat)
    {
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::setMat4(const std::string& name, const glm::mat4& mat)
    {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::checkCompileErrors(unsigned int shader, const std::string& type)
    {
        int  success;
        char info_log[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, nullptr, info_log);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << info_log << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, nullptr, info_log);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << info_log << std::endl;
            }
        }
    }

    std::string Shader::loadShaderSource(const std::string& path)
    {
        std::ifstream     shader_file(path);
        std::stringstream shader_stream;
        shader_stream << shader_file.rdbuf();
        shader_file.close();
        return shader_stream.str();
    }

    int Shader::getUniformLocation(const std::string& name)
    {
        auto it = m_uniform_cache.find(name);
        if (it != m_uniform_cache.end())
            return it->second;

        int location          = glGetUniformLocation(m_program, name.c_str());
        m_uniform_cache[name] = location;
        return location;
    }
} // namespace RealmEngine