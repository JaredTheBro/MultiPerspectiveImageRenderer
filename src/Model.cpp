#include <fstream>
#include <sstream>

#include "Model.h"

Model::Model()
{
	vertices = std::vector<glm::vec3>();
	triangles = std::vector<std::vector<int>>();
}

Model::~Model()
{

}

void Model::loadFileObj(const std::string&& filename)
{
	// Prepare file
	std::ifstream objFile(filename);
	if (!objFile) {
		// File error
		return;
	}

	std::string line;
	std::getline(objFile, line);

	// Loop line by line to parse the file
	while (!objFile.eof())
	{
		// Split up line into its elements
		std::vector<std::string> elems = splitBySpaces(line);

		// Check what line describes
		if (elems[0].compare("v") == 0)
		{
			// Vertex
			vertices.push_back(glm::vec3(
				std::stof(elems[1]),
				std::stof(elems[2]),
				std::stof(elems[3])
			));
		}
		else if (elems[0].compare("f") == 0 && elems.size() <= 5)
		{
			// Face - only supports convex shapes for now
			if (elems.size() == 4)
			{
				// Triangle
				std::vector<int> verts = std::vector<int>();
				verts.push_back(std::stoi(elems[1].substr(0, elems[1].find('/'))) - 1);
				verts.push_back(std::stoi(elems[2].substr(0, elems[2].find('/'))) - 1);
				verts.push_back(std::stoi(elems[3].substr(0, elems[3].find('/'))) - 1);
				triangles.push_back(verts);
			}
			else
			{
				// N-gon
				for (size_t i = 1; i < elems.size() - 2; i++)
				{
					// Fan triangulation only does convex shapes
					std::vector<int> verts = std::vector<int>();
					verts.push_back(std::stoi(elems[i].substr(0, elems[i].find('/'))) - 1);
					verts.push_back(std::stoi(elems[i+1].substr(0, elems[i+1].find('/'))) - 1);
					verts.push_back(std::stoi(elems[i+2].substr(0, elems[i+2].find('/'))) - 1);
					triangles.push_back(verts);
				}
			}
		}
		else {
			// Line type not handled, skip
			std::getline(objFile, line);
		}
	}
}

std::vector<std::string> splitBySpaces(const std::string& in)
{
	std::istringstream ss(in);
	std::vector<std::string> elements;
	std::string item;

	// Split string by one or more whitespace characters
	while (ss >> item) {
		elements.push_back(item);
	}
	return elements;
}
