#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>

class Model
{
public:

	Model();
	~Model();

	void loadFileObj(const std::string& filename);
	void drawVertexArray();
	
protected:

	std::vector<float> vertices; // triples (x, y, z)
	std::vector<unsigned int> triangles; // triples

	GLuint varr;
	GLuint vbuf;
	GLuint vcount;
	GLuint ebuf;

	std::vector<std::string> splitBySpaces(std::string& in);
};
