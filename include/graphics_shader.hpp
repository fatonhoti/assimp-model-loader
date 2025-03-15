#pragma once

#include "shader.hpp"

class GraphicsShader : public Shader
{
public:
    GraphicsShader(const char *vert_path, const char *frag_path);

private:
    void compile(const std::vector<std::pair<GLenum, std::string>> &sources) override;
};
