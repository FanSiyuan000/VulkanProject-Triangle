#pragma once

#include "lve_device.h"

namespace lve {
	class LveModel {
	public:
		LveModel();
		~LveModel();//��first_app.h 18��ע��		
		//ģ�������vulkan���������ڴ����		

		LveModel(const LveModel&) = delete;
		LveModel& operator = (const LveModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
		

	private:
		LveModel& lveDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		//����vulkan�Ļ��������󣬻��������������ڴ������������Ķ��󣬶������Զ�Ϊ�����������ڴ�
		uint32_t vertexCount;
	};
}