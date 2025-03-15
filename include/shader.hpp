#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>

class Shader
{
public:
    virtual ~Shader();
    void use() const;

    void set_bool(const std::string &name, bool value) const;
    void set_int(const std::string &name, int value) const;
    void set_uint(const std::string &name, uint value) const;
    void set_float(const std::string &name, float value) const;
    void set_vec2(const std::string &name, const glm::vec2 &value) const;
    void set_vec3(const std::string &name, const glm::vec3 &value) const;
    void set_vec4(const std::string &name, const glm::vec4 &value) const;
    void set_mat4(const std::string &name, const glm::mat4 &value) const;

protected:
    GLuint ID;
    mutable std::unordered_map<std::string, GLint> uniform_location_cache;

    virtual void compile(const std::vector<std::pair<GLenum, std::string>> &sources) = 0;
    void check_compile_error(const GLuint shader, const std::string &&type);
};
