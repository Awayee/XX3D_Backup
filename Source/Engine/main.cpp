#include <iostream>
#include "Core/CoreMath.h"
#include "Render/RenderInterface.h"
int main() {
	std::cout << "Hello xxEngine!" << std::endl;
	MATH::Vector3 v(0, 1, 2);
	int minVal = MATH::min((int)v.x, (int)v.y);
	std::cout << "MIN OF nums is " << minVal << std::endl;
	RI::Render();
	system("pause");
}