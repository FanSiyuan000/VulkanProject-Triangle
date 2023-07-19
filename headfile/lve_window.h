#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace lve {

	class LveWindow {

	public:
		LveWindow(int w, int h, std::string name);
		~LveWindow();//析构器清理
		

		bool shouldClose() {
			return glfwWindowShouldClose(window);
		}//公共构造函数获取将用于初始化成员变量的值

		VkExtent2D getExtent() {
			return { static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
		}

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:

		void initWindow();//初始化窗口函数

		const int width;
		const int height;

		std::string windowName;//成员变量储存窗口宽，长，名字
		GLFWwindow* window; //指向GLFWwindow的指针
	};//创建LveWindow类包装一些基于窗口的功能
}