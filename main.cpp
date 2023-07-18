#include "first_app.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main() {
	lve::FirstApp app{};//创建应用程序实例

	try {
		app.run();
	}
	catch (const std::exception& e) {//抛出错误
		std::cerr << e.what() << '\n';//如果有错误，输出到控制台
		return EXIT_FAILURE;
	}
	//try catch 调用run函数

	return EXIT_SUCCESS;
}