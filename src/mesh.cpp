#include "mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
    clear();
}

bool Mesh::load_mesh(const char *filename)
{
    clear();

    auto path_model = std::filesystem::current_path() / "assets" / "models" / filename;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(BUFFER_TYPE::NUM_BUFFERS, buffers);

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path_model.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if (!scene) {
        printf("Error parsing '%s': '%s'\n", filename, importer.GetErrorString());
        return false;
    }

    meshes.resize(scene->mNumMeshes);

    GLuint num_vertices = 0;
    GLuint num_indices = 0;
    for (GLuint i = 0; i < meshes.size(); i++) {
        meshes[i].material_index = scene->mMeshes[i]->mMaterialIndex;
        meshes[i].num_indices = scene->mMeshes[i]->mNumFaces * 3;
        meshes[i].base_vertex = num_vertices;
        meshes[i].base_index = num_indices;

        num_vertices += scene->mMeshes[i]->mNumVertices;
        num_indices += meshes[i].num_indices;
    }

    positions.reserve(num_vertices);
    normals.reserve(num_vertices);
    texCoords.reserve(num_vertices);
    indices.reserve(num_vertices);

    for (GLuint i = 0; i < meshes.size(); i++) {
        const auto aiMesh = scene->mMeshes[i];

        // populate vertex attribute vectors
        for (GLuint j = 0; j < aiMesh->mNumVertices; j++) {
            const auto& pos = aiMesh->mVertices[j];

            aiVector3D normal;
            if (aiMesh->mNormals) {
                normal = aiMesh->mNormals[j];
            } else {
                normal = aiVector3D(0.0f, 1.0f, 0.0f);
            }

            const auto& texCoord = aiMesh->HasTextureCoords(0) ? aiMesh->mTextureCoords[0][j] : aiVector3D(0.0f);

            positions.push_back(glm::vec3(pos.x, pos.y, pos.z));
            normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
            texCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
        }

        // populate index buffer
        for (GLuint j = 0; j < aiMesh->mNumFaces; j++) {
            const auto& face = aiMesh->mFaces[j];
            assert(face.mNumIndices == 3);
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

    }

    // TODO: grab materials too (textures)
    
    glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFFER_TYPE::POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFFER_TYPE::NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFFER_TYPE::TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords[0]) * texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[BUFFER_TYPE::INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    return true;
}

void Mesh::render()
{
    glBindVertexArray(vao);

    for (GLuint i = 0; i < meshes.size(); i++) {
        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            meshes[i].num_indices,
            GL_UNSIGNED_INT,
            (void*)(sizeof(GLuint) * meshes[i].base_index),
            meshes[i].base_vertex
        );
    }

    glBindVertexArray(0);
}

void Mesh::clear()
{
    if (buffers[0] != 0) {
        glDeleteBuffers(BUFFER_TYPE::NUM_BUFFERS, buffers);
    }

    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
}
