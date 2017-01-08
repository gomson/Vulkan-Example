#include "material.hpp"

Material::Material(aiMaterial *mtl, std::string const &globalPath, Transferer &transferer) {
    aiString texPath;

    if(mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
        std::string path = texPath.C_Str();

        if(path[0] == '.')
            path.erase(0, 1);

        if(path[0] == '/' || path[0] == '\\')
            path.erase(0, 1);

        for(auto &v: path)
            if(v == '\\')
                v = '/';

        path = globalPath + path;
        diffuseTexture = Texture(path, transferer);
        useTexture = true;
    }

    else {
        aiColor3D color;

        if(mtl->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
            this->color = glm::vec3(color.r, color.g, color.b);
        useTexture = false;
    }
}
