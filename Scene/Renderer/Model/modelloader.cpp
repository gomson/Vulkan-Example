#include "modelloader.hpp"
#include "modelimporter.hpp"

MeshLoader_t::MeshLoader_t(Buffer &vbo, Buffer &ibo, uint32_t &firstIndex, uint32_t &vertexOffset, const Mesh &mesh, Transferer &transferer) :
    indexCount(mesh.numberIndices),
    firstIndex(firstIndex),
    vertexOffset(vertexOffset) {
    transferer.transfer(vbo, vertexOffset * sizeof(Vertex), mesh.numberVertices * sizeof(Vertex), mesh.vertices.data());
    transferer.transfer(ibo, firstIndex * sizeof(uint32_t), mesh.numberIndices * sizeof(uint32_t), mesh.indices.data());

    firstIndex += mesh.numberIndices;
    vertexOffset += mesh.numberVertices;
}

ModelLoader_t::ModelLoader_t(std::string const &path, Buffer &vbo, Buffer &ibo, uint32_t &firstIndex, uint32_t &vertexOffset, Transferer &transferer) {
    ModelImporter modelImporter(path);

    for(auto m : modelImporter.mMeshes)
        mesheLoaders.emplace_back(vbo, ibo, firstIndex, vertexOffset, m, transferer);
}

ModelLoader_t ModelLoader::open(std::string const &path) {
    auto it = mModelLoaders.find(path);

    assert(it != mModelLoaders.end());
    return it->second;
}

ModelLoader_t ModelLoader::open(std::string const &path, Buffer &vbo, Buffer &ibo, uint32_t &firstIndex, uint32_t &vertexOffset, Transferer &transferer) {
    if(mModelLoaders.find(path) == mModelLoaders.end())
        mModelLoaders[path] = ModelLoader_t(path, vbo, ibo, firstIndex, vertexOffset, transferer);
    return mModelLoaders[path];
}
