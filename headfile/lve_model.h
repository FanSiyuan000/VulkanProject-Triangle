#pragma once

#include "lve_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace lve {
    class LveModel {
    public:
        struct Vertex {
            glm::vec3 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct Builder{
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
        };

        LveModel(LveDevice& device, const LveModel::Builder &builder);
        ~LveModel();//见first_app.h 18行注释
        //模型类管理vulkan缓冲区和内存对象

        LveModel(const LveModel&) = delete;
        LveModel& operator=(const LveModel&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);

        LveDevice& lveDevice;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        //对于vulkan的缓冲区对象，缓冲区及其分配的内存是两个单独的对象，而不是自动为缓冲区分配内存
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;
    };
}