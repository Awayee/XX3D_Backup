#include <iostream>
#include "Core/Math/CoreMath.h"
#include "Render/RenderInterface.h"
#include "Core/File/CoreFile.h"
#include "Core/Json/CoreJson.h"
#include "Core/Container/container.h"
int main() {
	std::cout << "Hello xxEngine!" << std::endl;
	MATH::Vector3 v(0, 1, 2);
	int minVal = MATH::Min((int)v.x, (int)v.y);
	std::cout << "MIN OF nums is " << minVal << std::endl;
	
	rapidjson::Document doc;
	Json::ParseFile("hhh", doc);

	RI::Render();
	system("pause");
}