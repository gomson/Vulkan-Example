#include "modelloader.hpp"
#include "modelimporter.hpp"

MeshLoader_t::MeshLoader_t(Buffer &vbo, Buffer &ibo, uint32_t &firstIndex, uint32_t &vertexOffset, const Mesh &mesh, Transferer &transferer) :
    indexCount(mesh.numberIndices),
    firstIndex(firstIndex),
    vertexOffset(vertexOffset) {
    transferer.transfer(vbo, vertexOffset * sizeof(Vertex), mesh.numberVertices * sizeof(Vertex), mesh.vertices.data(), vk::PipelineStageFlagBits::eTransfer | vk::PipelineStageFlagBits::eVertexInput);
    transferer.transfer(ibo, firstIndex * sizeof(uint32_t), mesh.numberIndices * sizeof(uint32_t), mesh.indices.data(), vk::PipelineStageFlagBits::eTransfer | vk::PipelineStageFlagBits::eVertexInput);

    firstIndex += mesh.numberIndices;
    vertexOffset += mesh.numberVertices;
    materialIndex = mesh.materialIndex;
}

MaterialLoader_t::MaterialLoader_t(Device &device, MaterialDescriptorSetManager &manager, const Material &material) : material(material) {
    descriptorSet = manager.allocate();

    MaterialUniform materialUniform;
    materialUniform.color = glm::vec4(material.color, 1.0f);
    materialUniform.hasDiffuseTexture = material.useTexture;

    auto buffer = manager.addMaterialToBuffer(materialUniform);

    vk::DescriptorBufferInfo bufferInfo(buffer.first, buffer.second, sizeof(materialUniform));
    vk::WriteDescriptorSet write(descriptorSet, 1, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo, nullptr);
    device.updateDescriptorSets(write, nullptr);

    if(material.useTexture == true) {
        vk::DescriptorImageInfo imageInfo(material.diffuseTexture.sampler, material.diffuseTexture.imageView, vk::ImageLayout::eShaderReadOnlyOptimal);
        vk::WriteDescriptorSet write(descriptorSet, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo, nullptr, nullptr);

        device.updateDescriptorSets(write, nullptr);
    }
}

ModelLoader_t::ModelLoader_t(std::string const &path, Buffer &vbo, Buffer &ibo, uint32_t &firstIndex, uint32_t &vertexOffset, Transferer &transferer, MaterialDescriptorSetManager &materialManager) {
    ModelImporter modelImporter(path, transferer);

    for(auto m : modelImporter.mMeshes)
        mesheLoaders.emplace_back(vbo, ibo, firstIndex, vertexOffset, m, transferer);

    for(auto m : modelImporter.mMaterials)
        materialLoaders.emplace_back(transferer.getAllocator()->getDevice(), materialManager, m);
}

ModelLoader_t ModelLoader::open(std::string const &path) {
    auto it = mModelLoaders.find(path);

    assert(it != mModelLoaders.end());
    return it->second;
}

ModelLoader_t ModelLoader::open(std::string const &path, Buffer &vbo, Buffer &ibo, uint32_t &firstIndex, uint32_t &vertexOffset, Transferer &transferer, MaterialDescriptorSetManager &materialManager) {
    if(mModelLoaders.find(path) == mModelLoaders.end())
        mModelLoaders[path] = ModelLoader_t(path, vbo, ibo, firstIndex, vertexOffset, transferer, materialManager);
    return mModelLoaders[path];
}
