#pragma once

#include "lve_device.h"
#include "lve_game_object.h"
#include "lve_pipeline.h"
#include "lve_camera.h"

#include <memory>
#include <vector>

namespace lve {
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass);
		~SimpleRenderSystem();//SimpleRenderSystem正在管理管道布局和command buffer的vulkan对象//SimpleRenderSystem正在管理管道布局和command buffer的vulkan对象

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
		//删除复制构造函数与复制运算符

		void renderGameObjects(VkCommandBuffer commandBuffer, 
			std::vector<LveGameObject> &gameObjects,
			const LveCamera &camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		LveDevice& lveDevice;

		std::unique_ptr<LvePipeline> lvePipeline;//智能指针
		VkPipelineLayout pipelineLayout;
		//使管道对象成为唯一的指针
		//LvePipeline lvePipeline{lveDevice, 
		//	"shaders/simple_shader.vert.spv", 
		//	"shaders/simple_shader.frag.spv", 
		//	LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};//实例化管道,注意读取.spv文件
	};
}