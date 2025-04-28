#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

class Mesh
{
public:

	Mesh();
	~Mesh();

	void loadFileObj(const std::string&& filename);

	
protected:

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int[3]> faces;

};
