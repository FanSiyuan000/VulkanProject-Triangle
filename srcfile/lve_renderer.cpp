#include "lve_renderer.h"

#include <array>
#include <cassert>
#include <stdexcept>

namespace lve {

    LveRenderer::LveRenderer(LveWindow& window, LveDevice& device) : lveWindow{ window }, lveDevice{device} {
        recreateSwapChain();
        createCommandBuffers();//调用
    }

    LveRenderer::~LveRenderer() { freeCommandBuffers(); }

    void LveRenderer::recreateSwapChain() {
        auto extent = lveWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = lveWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(lveDevice.device());

        if (lveSwapChain == nullptr) {
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
        }
        else {
            std::shared_ptr<LveSwapChain> oldSwapChain = std::move(lveSwapChain);
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*lveSwapChain.get())) {
                throw std::runtime_error("Swap chain image/depth format has changed!");
            }
        }

        //
    }

    void LveRenderer::createCommandBuffers() {
        commandBuffers.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);//imageCount的值可能是2或3，取决于设备是否支持双缓冲或是三重缓冲

        VkCommandBufferAllocateInfo allocInfo{};//分配命令缓冲区
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        //primary命令缓冲区可以提交到队列执行，但不能被其他命令缓冲区调用，secondary命令缓冲区不能提交，但能被其它命令缓冲区调用
        allocInfo.commandPool = lveDevice.getCommandPool();
        //从命令池中分配和释放命令缓冲区，减少资源创建成本
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void LveRenderer::freeCommandBuffers() {
        vkFreeCommandBuffers(
            lveDevice.device(),
            lveDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        commandBuffers.clear();
    }


    VkCommandBuffer LveRenderer::beginFrame() {
        assert(!isFrameStarted && "Can't call beginFrame while already in progress");

        
        auto result = lveSwapChain->acquireNextImage(&currentImageIndex);
        //获取应该渲染到下一帧的索引，自动处理CPU与GPU围绕双缓冲或三重缓冲返回的结果值确定过程是否成功
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        return commandBuffer;
    }
    void LveRenderer::endFrame() {
        assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

        auto result = lveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        //将提供的命令缓冲区提交到设备图形队列，同时处理CPU,GPU同步
        //然后执行命令缓冲区，swapchain将根据所选的当前模式在适当的时间将关联的颜色附件视图呈现给显示器
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
            lveWindow.wasWindowResized()) {
            lveWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % LveSwapChain::MAX_FRAMES_IN_FLIGHT;
    };

    void LveRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lveSwapChain->getRenderPass();//设置渲染通道成员
        renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(currentImageIndex);//渲染通道写入哪个帧缓冲区

        renderPassInfo.renderArea.offset = { 0, 0 };//设置渲染区域（着色器加载和储存的区域）
        renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};//清除帧缓冲区附件的初始值
        clearValues[0].color = { 0.5f, 0.5f, 0.5f, 1.0f };//RGBA值
        clearValues[1].depthStencil = { 1.0f, 0 };//深度值 1-0
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, lveSwapChain->getSwapChainExtent() };
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }
    void LveRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
        vkCmdEndRenderPass(commandBuffer);//完成记录
    }
}