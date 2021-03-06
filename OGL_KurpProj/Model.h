#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Shader.h"
#include <string>
#include <vector>


using namespace std;


struct ModelTransform
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    void setScale(float s)
    {
        scale.x = s;
        scale.y = s;
        scale.z = s;
    }
};


class Model
{
public:
    vector<Texture> textures_loaded;
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    Model(string const& path, bool isUV_flipped = true, bool gamma = false);
    void Draw(Shader* shader);
    HitBox getHitBox();
private:
    void loadModel(string const& path, bool isUV_flipped);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

#endif