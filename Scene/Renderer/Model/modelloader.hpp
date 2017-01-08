#pragma once
#include <unordered_map>
#include "mesh.hpp"
#include "material.hpp"
#include "VkTools/Buffer/buffer.hpp"
#include "VkTools/Command/transferer.hpp"
#include "Renderer/Material/materialdescriptorsetpool.hpp"

struct MeshLoader_t {
    MeshLoader_t(Buffer &vbo, Buffer &ibo, uint32_t &firstIndex, uint32_t &vertexOffset, Mesh const &mesh, Transferer &transferer);

    uint32_t indexCount;
    uint32_t firstIndex;
    uint32_t vertexOffset;
    uint32_t materialIndex;
};

struct MaterialLoader_t {
    MaterialLoader_t(Device &device, MaterialDescriptorSetManager &manager, Material const &material);

    vk::DescriptorSet descriptorSet;
    Material material;
};

struct ModelLoader_t
{
    ModelLoader_t() = default;
    ModelLoader_t(std::string const &path, Buffer &vbo, Buffer &ibo, uint32_t &firstIndex, uint32_t &vertexOffset, Transferer &transferer, MaterialDescriptorSetManager &materialManager);
    ModelLoader_t(ModelLoader_t const&) = default;
    ModelLoader_t &operator=(ModelLoader_t const&) = default;

    std::vector<MeshLoader_t> mesheLoaders;
    std::vector<MaterialLoader_t> materialLoaders;
};

class ModelLoader {
public:
    ModelLoader_t open(std::string const &path);
    ModelLoader_t open(std::string const &path, Buffer &vbo, Buffer &ibo, uint32_t &firstIndex, uint32_t &vertexOffset, Transferer &transferer, MaterialDescriptorSetManager &materialManager);

private:
    std::unordered_map<std::string, ModelLoader_t> mModelLoaders;
};

