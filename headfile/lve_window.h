#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace lve {

	class LveWindow {

	public:
		LveWindow(int w, int h, std::string name);
		~LveWindow();//����������
		

		bool shouldClose() {
			return glfwWindowShouldClose(window);
		}//�������캯����ȡ�����ڳ�ʼ����Ա������ֵ

		VkExtent2D getExtent() {
			return { static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
		}

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:

		void initWindow();//��ʼ�����ں���

		const int width;
		const int height;

		std::string windowName;//��Ա�������洰�ڿ���������
		GLFWwindow* window; //ָ��GLFWwindow��ָ��
	};//����LveWindow���װһЩ���ڴ��ڵĹ���
}