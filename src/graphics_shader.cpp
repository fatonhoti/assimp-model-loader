#include "graphics_shader.hpp"
#include "util.hpp"

GraphicsShader::GraphicsShader(const char *vert_path, const char *frag_path)
{
    std::string vert_code, frag_code;
    util::read_shader_file(vert_path, vert_code);
    util::read_shader_file(frag_path, frag_code);

    std::vector<std::pair<GLenum, std::string>> sources = {
        {GL_VERTEX_SHADER, vert_code},
        {GL_FRAGMENT_SHADER, frag_code}};

    compile(sources);
}

void GraphicsShader::compile(const std::vector<std::pair<GLenum, std::string>> &sources)
{
    ID = glCreateProgram();
    std::vector<GLuint> shaders;

    for (const auto &[type, src] : sources)
    {
        GLuint shader = glCreateShader(type);
        const char *code = src.c_str();
        glShaderSource(shader, 1, &code, nullptr);
        glCompileShader(shader);
        check_compile_error(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");

        glAttachShader(ID, shader);
        shaders.push_back(shader);
    }

    glLinkProgram(ID);
    check_compile_error(ID, "PROGRAM");

    for (GLuint shader : shaders)
    {
        glDeleteShader(shader);
    }
}
