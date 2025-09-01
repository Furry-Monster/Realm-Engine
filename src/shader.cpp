#include "shader.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <fstream>
#include <iostream>
#include <sstream>

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

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    checkCompileErrors(id, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() { glDeleteProgram(id); }

void Shader::use() const { glUseProgram(id); }

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
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