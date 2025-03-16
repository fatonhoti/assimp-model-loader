#pragma once

#include "glad/glad.h"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

constexpr GLuint POSITION_LOCATION = 0;
constexpr GLuint NORMAL_LOCATION = 1;
constexpr GLuint TEX_COORD_LOCATION = 2;

class Mesh {
public:
    Mesh();
    ~Mesh();
    bool load_mesh(const char* filename);
    void render();

private:

    enum BUFFER_TYPE {
        INDEX_BUFFER = 0,
        POS_VB       = 1,
        NORMAL_VB    = 2,
        TEXCOORD_VB  = 3,
        NUM_BUFFERS  = 4
    };

    GLuint vao = 0;
    GLuint buffers[BUFFER_TYPE::NUM_BUFFERS] = { 0 };

    struct MeshEntry {
        GLuint num_indices = 0;
        GLuint base_vertex = 0;
        GLuint base_index = 0;
        GLuint material_index = 0;
    };

    std::vector<MeshEntry> meshes;
    std::vector<GLuint> indices;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;

    void clear();
};