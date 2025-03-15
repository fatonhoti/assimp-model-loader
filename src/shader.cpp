#include "shader.hpp"

#include <iostream>
#include <stdexcept>

Shader::~Shader()
{
    glDeleteProgram(ID);
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::check_compile_error(const GLuint shader, const std::string &&type)
{
    GLint success;
    GLchar infoLog[1024];

    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: "
                      << type << "\n"
                      << infoLog
                      << "\n -- --------------------------------------------------- -- "
                      << std::endl;
            throw std::runtime_error("Failed to compile shader.");
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: "
                      << type << "\n"
                      << infoLog
                      << "\n -- --------------------------------------------------- -- "
                      << std::endl;
            throw std::runtime_error("Failed to link shader.");
        }
    }
}

void Shader::set_bool(const std::string &name, bool value) const
{
    glProgramUniform1i(ID, glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::set_int(const std::string &name, int value) const
{
    glProgramUniform1i(ID, glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set_uint(const std::string &name, uint value) const
{
    glProgramUniform1ui(ID, glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set_float(const std::string &name, float value) const
{
    glProgramUniform1f(ID, glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set_vec2(const std::string &name, const glm::vec2 &value) const
{
    glProgramUniform2fv(ID, glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::set_vec3(const std::string &name, const glm::vec3 &value) const
{
    glProgramUniform3fv(ID, glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::set_vec4(const std::string &name, const glm::vec4 &value) const
{
    glProgramUniform4fv(ID, glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &value) const
{
    glProgramUniformMatrix4fv(ID, glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
