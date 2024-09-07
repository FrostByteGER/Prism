#pragma once
#include <utility>

#include "VulkanBuffer.hpp"
#include "../../Core/Mesh.hpp"

namespace Prism::Rendering::Vulkan
{
    class VulkanMesh
    {
    public:
        VulkanMesh(const RawPtr<vk::UniqueDevice> logicalDevice, std::string meshAssetName,
                   const std::vector<uint64_t>& meshIds,
                   std::unique_ptr<VulkanBuffer> vertexBuffer,
                   std::unique_ptr<VulkanBuffer> indexBuffer) :
            logicalDevice(logicalDevice),
            meshAssetName(std::move(meshAssetName)),
            meshIds(meshIds),
            vertexBuffer(std::move(vertexBuffer)),
            indexBuffer(std::move(indexBuffer))
        {
        }

        ~VulkanMesh()
        {
            logicalDevice->get().destroyBuffer(vertexBuffer->getBuffer());
            logicalDevice->get().freeMemory(vertexBuffer->getBufferMemory());
            logicalDevice->get().destroyBuffer(indexBuffer->getBuffer());
            logicalDevice->get().freeMemory(indexBuffer->getBufferMemory());
        }

        [[nodiscard]] std::string getMeshAssetName() const
        {
            return meshAssetName;
        }

        [[nodiscard]] std::vector<uint64_t> getMeshIds() const
        {
            return meshIds;
        }

        [[nodiscard]] bool hasMeshIdAssociated(const uint64_t meshId)
        {
            return std::ranges::find(meshIds, meshId) != meshIds.end();
        }

        [[nodiscard]] bool associateMeshId(const uint64_t meshId)
        {
            if (hasMeshIdAssociated(meshId))
            {
                return false;
            }

            meshIds.emplace_back(meshId);
            return true;
        }

        void unassociateMeshId(const uint64_t meshId)
        {
            std::erase_if(meshIds, [meshId](const uint64_t& id) { return id == meshId; });
        }

        [[nodiscard]] const VulkanBuffer& getVertexBuffer() const
        {
            return *vertexBuffer;
        }

        [[nodiscard]] const VulkanBuffer& getIndexBuffer() const
        {
            return *indexBuffer;
        }

    private:
        RawPtr<vk::UniqueDevice> logicalDevice;
        std::string meshAssetName;
        std::vector<uint64_t> meshIds;
        std::unique_ptr<VulkanBuffer> vertexBuffer;
        std::unique_ptr<VulkanBuffer> indexBuffer;
    };;
}
