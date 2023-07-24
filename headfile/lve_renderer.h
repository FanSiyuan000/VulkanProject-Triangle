#pragma once

#include "lve_device.h"
#include "lve_swap_chain.h"
#include "lve_window.h"

#include <cassert>
#include <memory>
#include <vector>

namespace lve {
	class LveRenderer {
	public:

		LveRenderer(LveWindow &window, LveDevice &device);
		~LveRenderer();//LveRenderer正在管理管道布局和command buffer的vulkan对象

		LveRenderer(const LveRenderer&) = delete;
		LveRenderer& operator=(const LveRenderer&) = delete;
		//删除复制构造函数与复制运算符

		VkRenderPass getSwapChainRenderPass() const { return lveSwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:

		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		LveWindow& lveWindow;//创建并打开一个窗口，销毁时，window自动破坏
		LveDevice& lveDevice;
		std::unique_ptr<LveSwapChain> lveSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;
		//使管道对象成为唯一的指针
		//LvePipeline lvePipeline{lveDevice, 
		//	"shaders/simple_shader.vert.spv", 
		//	"shaders/simple_shader.frag.spv", 
		//	LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};//实例化管道,注意读取.spv文件
	};
}