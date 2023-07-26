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
		~SimpleRenderSystem();//SimpleRenderSystem���ڹ���ܵ����ֺ�command buffer��vulkan����//SimpleRenderSystem���ڹ���ܵ����ֺ�command buffer��vulkan����

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
		//ɾ�����ƹ��캯���븴�������

		void renderGameObjects(VkCommandBuffer commandBuffer, 
			std::vector<LveGameObject> &gameObjects,
			const LveCamera &camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		LveDevice& lveDevice;

		std::unique_ptr<LvePipeline> lvePipeline;//����ָ��
		VkPipelineLayout pipelineLayout;
		//ʹ�ܵ������ΪΨһ��ָ��
		//LvePipeline lvePipeline{lveDevice, 
		//	"shaders/simple_shader.vert.spv", 
		//	"shaders/simple_shader.frag.spv", 
		//	LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};//ʵ�����ܵ�,ע���ȡ.spv�ļ�
	};
}