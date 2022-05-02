#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    aiString path;
};

struct HitBox {
    float xMin = 2000.0f, yMin = 2000.0f, zMin = 2000.0f;
    float xMax = -2000.0f, yMax = -2000.0f, zMax = -2000.0f;

    bool lessThenX(HitBox& other) {
        return xMin < other.xMin;
    }

    bool lessThenY(HitBox& other) {
        return yMin < other.yMin;
    }

    bool lessThenZ(HitBox& other) {
        return zMin < other.zMin;
    }

    void scale(float scale) {
        xMin *= scale; yMin *= scale; zMin *= scale;        
        xMax *= scale; yMax *= scale; zMax *= scale;
    }


};
class Mesh {
public:
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

    void Draw(Shader* shader);

    HitBox getHitBox();
private:
    unsigned int VBO, EBO;

    void setupMesh();
};
#endif