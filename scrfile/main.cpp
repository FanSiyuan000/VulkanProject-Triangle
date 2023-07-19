#include "first_app.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main() {
	lve::FirstApp app{};//����Ӧ�ó���ʵ��

	try {
		app.run();
	}
	catch (const std::exception& e) {//�׳�����
		std::cerr << e.what() << '\n';//����д������������̨
		return EXIT_FAILURE;
	}
	//try catch ����run����

	return EXIT_SUCCESS;
}