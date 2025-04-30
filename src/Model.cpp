#include <fstream>
#include <sstream>

#include <iostream>

#include "Model.h"

Model::Model()
{
	vertices = std::vector<float>();
	triangles = std::vector<unsigned int>();

	varr = 0;
	vbuf = 0;
	vcount = 0;
	ebuf = 0;
}

Model::~Model()
{

}

void Model::loadFileObj(const std::string& filename)
{
	// Prepare file
	std::ifstream objFile(filename);
	if (!objFile) {
		// File error
		exit(1);
	}

	std::string line;
	std::getline(objFile, line);

	// Loop line by line to parse the file
	while (!(objFile.eof()))
	{
		// Split up line into its elements
		std::vector<std::string> elems = splitBySpaces(line);

		printf("%d\n", elems.size());

		// Make sure line isn't empty
		if (elems.size() == 0) {
			std::getline(objFile, line);
			continue;
		}

		// Check what line describes
		if (elems[0].compare("v") == 0)
		{
			// Vertex
			vertices.push_back(std::stof(elems[1]));
			vertices.push_back(std::stof(elems[2]));
			vertices.push_back(std::stof(elems[3]));
		}
		else if (elems[0].compare("f") == 0)
		{
			// Face - only supports convex shapes for now
			if (elems.size() == 4)
			{
				// Triangle
				triangles.push_back((unsigned int) std::stoi(elems[1].substr(0, elems[1].find('/'))) - 1);
				triangles.push_back((unsigned int) std::stoi(elems[2].substr(0, elems[2].find('/'))) - 1);
				triangles.push_back((unsigned int) std::stoi(elems[3].substr(0, elems[3].find('/'))) - 1);
			}
			else
			{
				// N-gon
				for (size_t i = 1; i < elems.size() - 2; i++)
				{
					// Fan triangulation only does convex shapes
					triangles.push_back((unsigned int) std::stoi(elems[1].substr(0, elems[1].find('/'))) - 1);
					triangles.push_back((unsigned int) std::stoi(elems[i+1].substr(0, elems[i+1].find('/'))) - 1);
					triangles.push_back((unsigned int) std::stoi(elems[i+2].substr(0, elems[i+2].find('/'))) - 1);
				}
			}
		}
		
		// Next line
		std::getline(objFile, line);
	}

	// Test prints
	for (size_t i = 0; i < vertices.size() - 2; i += 3) {
		printf("v %f %f %f\n", vertices[i], vertices[i + 1], vertices[i + 2]);
	}
	for (size_t i = 0; i < triangles.size() - 2; i += 3) {
		printf("f %d %d %d\n", triangles[i], triangles[i + 1], triangles[i + 2]);
	}

	// Send obj data to OpenGL for rendering
	glGenVertexArrays(1, &varr);
	glGenBuffers(1, &vbuf);
	glGenBuffers(1, &ebuf);
	glBindVertexArray(varr);

	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(unsigned int), triangles.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Model::drawVertexArray()
{
	glBindVertexArray(varr);
	glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

std::vector<std::string> Model::splitBySpaces(std::string& in)
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
