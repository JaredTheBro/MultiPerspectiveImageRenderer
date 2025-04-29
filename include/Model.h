#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

class Model
{
public:

	Model();
	~Model();

	void loadFileObj(const std::string&& filename);

	
protected:

	std::vector<glm::vec3> vertices;
	std::vector<std::vector<int>> triangles;

};
