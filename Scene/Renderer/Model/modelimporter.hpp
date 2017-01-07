#pragma once
#include "mesh.hpp"

class ModelImporter
{
public:
    ModelImporter(std::string const &path);

public:
    std::vector<Mesh> mMeshes;
};
